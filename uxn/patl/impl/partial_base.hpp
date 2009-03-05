#ifndef PATL_PARTIAL_BASE_HPP
#define PATL_PARTIAL_BASE_HPP

#include <vector>
#include <map>
#include <algorithm>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Container, bool SameLength>
class partial_base
{
protected:
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    partial_base(
        const Container &cont,              // экземпляр контейнера
        const key_type &mask,               // маска поиска (образец)
        word_t mask_len = ~word_t(0),       // длина маски в символах (для бесконечных строк)
        const char_type &terminator = '\0') // символ окончания строки
        : mask_(mask)
        , mask_len_(get_min(
            mask_len,
            cont.bit_comp().bit_length(mask) / bit_compare::bit_size - 1))
        , mask_bit_len_(mask_len_ * bit_compare::bit_size)
        , terminator_(terminator)
        , bit_comp_(cont.bit_comp())
    {
    }

    static const bool accept_subtree = !SameLength;

    //void init();

    bool operator()(word_t i) const
    {
        return i <= mask_len_;
    }

    //bool operator()(word_t i, const char_type &ch);

    /// bit-level optimization; implementation by default
    bool bit_level(word_t, word_t) const
    {
        return true;
    }

    const key_type &mask() const
    {
        return mask_;
    }

protected:
    key_type mask_;
    word_t
        mask_len_,
        mask_bit_len_;
    char_type terminator_;
    bit_compare bit_comp_;
};

template <typename This, typename Container, bool SameLength>
class levenshtein_generic
    : public partial_base<Container, SameLength>
{
    typedef This this_t;
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<std::pair<word_t, word_t> > states_vector;
    typedef std::vector<states_vector> states_sequence;

public:
    levenshtein_generic(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, mask, mask_len, terminator)
        , dist_(dist)
        , states_seq_(1, states_vector(1, std::make_pair(0, 0)))
    {
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
                if (impl::bits_but_highest(it->first) == super::mask_len_ && it->second < cur_dist_)
                    cur_dist_ = it->second;
            }
            return cur_dist_ != ~word_t(0);
        }
        states_vector &next = states_seq_.back();
        std::back_insert_iterator<states_vector> next_ins(next);
        for (states_vector::const_iterator it = current.begin()
            ; it != current.end()
            ; ++it)
        {
            const word_t
                i = it->first,
                e = it->second;
// C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable : 4127)
            if (!SameLength && impl::bits_but_highest(i) == super::mask_len_)
#pragma warning(pop)
            {
                next.clear();
                cur_dist_ = e;
                for (++it; it != current.end(); ++it)
                {
                    if (impl::bits_but_highest(it->first) == super::mask_len_ &&
                        it->second < cur_dist_)
                        cur_dist_ = it->second;
                }
                return true;
            }
            static_cast<this_t*>(this)->transitions(i, e, ch, next_ins);
            // sort & remove duplicates
            std::sort(next.begin(), next.end());
            next.resize(std::distance(
                next.begin(),
                std::unique(next.begin(), next.end())));
        }
        return !next.empty();
    }

protected:
    word_t dist_;

private:
    states_sequence states_seq_;
    word_t cur_dist_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
