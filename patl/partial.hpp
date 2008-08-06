#ifndef PATL_PARTIAL_HPP
#define PATL_PARTIAL_HPP

#include <vector>
#include <map>
#include "impl/partial_base.hpp"

namespace uxn
{
namespace patl
{

/// параметры шаблона: класс контейнера и флаг совпадения длин маски и ключа
/// (для суффикс-контейнеров имеет смысл только false)
template <typename Container, bool SameLength = false>
class partial_match
    : public impl::partial_base<Container, SameLength>
{
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    partial_match(
        const Container &cont,              // экземпляр контейнера
        const key_type &mask,               // маска с джокерами типа '?'
        word_t mask_len = ~word_t(0),       // длина маски в символах (для бесконечных строк)
        const char_type &joker = '?',       // символ джокера
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, mask, mask_len, terminator)
        , joker_(joker)
    {
    }

    void init() const
    {
    }

    bool operator()(word_t i) const
    {
        return i <= super::mask_len_;
    }

    // проверка на возможность присутствия символа ch в позиции i образца
    bool operator()(word_t i, const char_type &ch) const
    {
        return i < super::mask_len_
            ? ch != super::terminator_ && (super::mask_[i] == joker_ || super::mask_[i] == ch)
            : !SameLength || i == super::mask_len_ && ch == super::terminator_;
    }

private:
    char_type joker_;
};

/// Hamming distance decision functor
template <typename Container, bool SameLength = false>
class hamming_distance
    : public impl::partial_base<Container, SameLength>
{
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    hamming_distance(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, mask, mask_len, terminator)
        , dist_(dist)
    {
    }

    word_t distance() const
    {
        return diff_.size();
    }

    void init()
    {
        diff_.clear();
    }

    bool operator()(word_t i) const
    {
        return i <= super::mask_len_;
    }

    // проверка на возможность присутствия символа ch в позиции i образца
    bool operator()(word_t i, const char_type &ch)
    {
        while (!diff_.empty() && i <= diff_.back())
            diff_.pop_back();
        if (i < super::mask_len_ && ch != super::terminator_)
        {
            if (super::mask_[i] == ch)
                return true;
            if (diff_.size() == dist_)
                return false;
            diff_.push_back(i);
            return true;
        }
        return
            !SameLength && i >= super::mask_len_ ||
            i == super::mask_len_ && ch == super::terminator_;
    }

private:
    word_t dist_;
    std::vector<word_t> diff_;
};

/// Levenshtein distance decision functor
/// based on Levenshtein Automata algorithm
/// see Schulz, Mihov, `Fast String Correction with Levenshtein-Automata'
template <typename Container, bool SameLength = false>
class levenshtein_distance
    : public impl::partial_base<Container, SameLength>
{
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<bool> bit_vector;
    typedef std::map<char_type, bit_vector> char_bits_map;
    typedef std::vector<std::pair<word_t, word_t> > states_vector;
    typedef std::vector<states_vector> states_sequence;

public:
    levenshtein_distance(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, mask, mask_len, terminator)
        , dist_(dist)
        , zero_bits_(super::mask_len_) // all false init
        , states_seq_(1, states_vector(1, std::make_pair(0, 0)))
    {
        for (word_t i = 0; i != super::mask_len_; ++i)
        {
            const char_type ch = super::mask_[i];
            if (char_map_.find(ch) == char_map_.end())
            {
                const std::pair<typename char_bits_map::iterator, bool> ins_pair =
                    char_map_.insert(std::make_pair(ch, bit_vector(super::mask_len_)));
                bit_vector &hi = ins_pair.first->second;
                for (word_t i = 0; i != super::mask_len_; ++i)
                    hi[i] =  ch == super::mask_[i];
            }
        }
    }

    word_t distance() const
    {
        return cur_dist_;
    }

    void init()
    {
        states_seq_.clear();
        states_seq_.push_back(states_vector(1, std::make_pair(0, 0)));
    }

    bool operator()(word_t i)
    {
        while (states_seq_.size() > i + 1)
            states_seq_.pop_back();
        return !states_seq_.back().empty();
    }

    bool operator()(word_t, const char_type &ch)
    {
        states_seq_.push_back(states_vector());
        const states_vector &current = states_seq_[states_seq_.size() - 2];
        if (ch == super::terminator_)
        {
            cur_dist_ = ~word_t(0);
            for (states_vector::const_iterator it = current.begin()
                ; it != current.end()
                ; ++it)
            {
                if (it->first == super::mask_len_ && it->second < cur_dist_)
                    cur_dist_ = it->second;
            }
            return cur_dist_ != ~word_t(0);
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
                b1 = impl::get_min(super::mask_len_, b0 + (dist_ + 1 - e));
// C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable : 4127)
            if (i == super::mask_len_ && !SameLength)
#pragma warning(pop)
            {
                next.clear();
                cur_dist_ = e;
                for (
                    ; it != current.end()
                    ; ++it)
                {
                    if (it->first == super::mask_len_ && it->second < cur_dist_)
                        cur_dist_ = it->second;
                }
                return true;
            }
            if (e < dist_)
            {
                if (i < super::mask_len_ - 1)
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
                else if (i == super::mask_len_ - 1)
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
                    next.push_back(std::make_pair(super::mask_len_, e + 1));
            }
            else if (i < super::mask_len_ && hi[b0])
                next.push_back(std::make_pair(i + 1, dist_));
        }
        return !next.empty();
    }

private:
    word_t dist_;
    char_bits_map char_map_;
    bit_vector zero_bits_;
    states_sequence states_seq_;
    word_t cur_dist_;
};

} // namespace patl
} // namespace uxn

#endif
