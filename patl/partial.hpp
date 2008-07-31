#ifndef PATL_PARTIAL_HPP
#define PATL_PARTIAL_HPP

#include <vector>
#include <map>

namespace uxn
{
namespace patl
{

/// параметры шаблона: класс контейнера и флаг совпадения длин маски и ключа
/// (для суффикс-контейнеров имеет смысл только false)
template <typename Container, bool SameLength = false>
class partial_match
{
    typedef Container cont_type;
    typedef typename cont_type::key_type key_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    partial_match(
        const cont_type &cont,              // экземпляр контейнера
        const key_type &mask,               // маска с джокерами типа '?'
        word_t mask_len = ~word_t(0),       // длина маски в символах (для бесконечных строк)
        const char_type &joker = '?',       // символ джокера
        const char_type &terminator = '\0') // символ окончания строки
        : mask_(mask)
        , mask_len_(impl::get_min(
            mask_len,
            cont.bit_comp().bit_length(mask) / bit_compare::bit_size - 1))
        , joker_(joker)
        , terminator_(terminator)
    {
    }

    void init() const
    {
    }

    // проверка на возможность присутствия символа ch в позиции i образца
    bool operator()(word_t i, const char_type &ch) const
    {
        return i < mask_len_
            ? ch != terminator_ && (mask_[i] == joker_ || mask_[i] == ch)
            : !SameLength || i == mask_len_ && ch == terminator_;
    }

private:
    key_type mask_;
    word_t mask_len_;
    char_type joker_, terminator_;
};

/// Hamming distance decision functor
template <typename Container, bool SameLength = false>
class hamming_distance
{
    typedef Container cont_type;
    typedef typename cont_type::key_type key_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    hamming_distance(
        const cont_type &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : mask_(mask)
        , dist_(dist)
        , mask_len_(impl::get_min(
            mask_len,
            cont.bit_comp().bit_length(mask) / bit_compare::bit_size - 1))
        , terminator_(terminator)
    {
    }

    void init()
    {
        diff_.clear();
    }

    // проверка на возможность присутствия символа ch в позиции i образца
    bool operator()(word_t i, const char_type &ch)
    {
        while (!diff_.empty() && i <= diff_.back())
            diff_.pop_back();
        if (i < mask_len_ && ch != terminator_)
        {
            if (mask_[i] == ch)
                return true;
            if (diff_.size() == dist_)
                return false;
            diff_.push_back(i);
            return true;
        }
        return (!SameLength && i >= mask_len_) || (i == mask_len_ && ch == terminator_);
    }

private:
    word_t dist_;
    key_type mask_;
    word_t mask_len_;
    char_type terminator_;
    std::vector<word_t> diff_;
};

/// Levenshtein distance decision functor
/// based on Levenshtein Automata algorithm
/// see Schulz, Mihov, `Fast String Correction with Levenshtein-Automata'
template <typename Container, bool SameLength = false>
class levenshtein_distance
{
    typedef Container cont_type;
    typedef typename cont_type::key_type key_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<bool> bit_vector;
    typedef std::map<char_type, bit_vector> char_bits_map;
    typedef std::vector<std::pair<word_t, word_t> > states_vector;
    typedef std::vector<states_vector> states_sequence;

public:
    levenshtein_distance(
        const cont_type &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : mask_(mask)
        , dist_(dist)
        , mask_len_(impl::get_min(
            mask_len,
            cont.bit_comp().bit_length(mask) / bit_compare::bit_size - 1))
        , terminator_(terminator)
        , zero_bits_(mask_len_) // all false init
        , states_seq_(1, states_vector(1, std::make_pair(0, 0)))
    {
        for (word_t i = 0; i != mask_len_; ++i)
        {
            const char_type ch = mask_[i];
            if (char_map_.find(ch) == char_map_.end())
            {
                const std::pair<typename char_bits_map::iterator, bool> ins_pair =
                    char_map_.insert(std::make_pair(ch, bit_vector()));
                bit_vector &hi = ins_pair.first->second;
                for (word_t i = 0; i != mask_len_; ++i)
                    hi.push_back(ch == mask_[i]);
            }
        }
    }

    void init()
    {
        states_seq_.clear();
        states_seq_.push_back(states_vector(1, std::make_pair(0, 0)));
    }

    bool operator()(word_t i, const char_type &ch)
    {
        while (states_seq_.size() > i + 1)
            states_seq_.pop_back();
        if (states_seq_.back().empty()) // && !SameLength
            return true;
        states_seq_.push_back(states_vector());
        const states_vector &current = states_seq_[states_seq_.size() - 2];
        if (ch == terminator_)
        {
            for (states_vector::const_iterator it = current.begin()
                ; it != current.end()
                ; ++it)
            {
                if (it->first == mask_len_)
                    return true;
            }
            return false;
        }
        states_vector &next = states_seq_.back();
        const typename char_bits_map::const_iterator hi_it = char_map_.find(ch);
        const bit_vector &hi = hi_it != char_map_.end()
            ? hi_it->second
            : zero_bits_;
        for (states_vector::const_iterator it = current.begin()
            ; it != current.end()
            ; ++it)
        {
            const word_t
                i = it->first,
                e = it->second,
                b0 = i,
                b1 = impl::get_min(mask_len_, b0 + (dist_ + 1 - e));
            if (e < dist_)
            {
                if (i < mask_len_ - 1)
                {
                    if (hi[b0])
                        next.push_back(std::make_pair(i + 1, e));
                    else
                    {
                        word_t j = b0 + 1;
                        for (; j != b1; ++j)
                        {
                            if (hi[j])
                            {
                                next.push_back(std::make_pair(i, e + 1));
                                next.push_back(std::make_pair(i + 1, e + 1));
                                next.push_back(std::make_pair(
                                    i + (j - b0 + 1),
                                    e + (j - b0)));
                                break;
                            }
                        }
                        if (j == b1)
                        {
                            next.push_back(std::make_pair(i, e + 1));
                            next.push_back(std::make_pair(i + 1, e + 1));
                        }
                    }
                }
                else if (i == mask_len_ - 1)
                {
                    if (hi[b0])
                        next.push_back(std::make_pair(i + 1, e));
                    else
                    {
                        next.push_back(std::make_pair(i, e + 1));
                        next.push_back(std::make_pair(i + 1, e + 1));
                    }
                }
                else
                    next.push_back(std::make_pair(mask_len_, e + 1));
            }
            else if (i < mask_len_ && hi[b0])
                next.push_back(std::make_pair(i + 1, dist_));
        }
        if (next.empty())
        {
// C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable : 4127)
            if (SameLength)
                return false;
#pragma warning(pop)
            for (states_vector::const_iterator it = current.begin()
                ; it != current.end()
                ; ++it)
            {
                if (it->first == mask_len_)
                    return true;
            }
            states_seq_.pop_back();
            return false;
        }
        return true;
    }

private:
    word_t dist_;
    key_type mask_;
    word_t mask_len_;
    char_type terminator_;
    char_bits_map char_map_;
    bit_vector zero_bits_;
    states_sequence states_seq_;
};

} // namespace patl
} // namespace uxn

#endif
