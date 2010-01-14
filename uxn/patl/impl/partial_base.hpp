/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
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

#if 1 // optimized implementation of levenshtein_generic

template <typename This, typename Container, bool SameLength>
class levenshtein_generic
    : public partial_base<Container, SameLength>
{
    typedef This this_t;
    typedef partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<std::pair<word_t, word_t> > states_vector;

public:
    levenshtein_generic(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = char_type()) // sentinel character
        : super(cont, mask, mask_len, terminator)
        , dist_(dist)
        , begins_(1)
        , states_(1)
    {
    }

    void init()
    {
        begins_.clear();
        begins_.push_back(0);
        states_.clear();
        states_.push_back(std::make_pair(0, 0));
    }

    word_t distance() const
    {
        return cur_dist_;
    }

    bool operator()(word_t i)
    {
        word_t trunc = states_.size();
        while (begins_.size() > i + 1)
        {
            trunc = begins_.back();
            begins_.pop_back();
        }
        states_.resize(trunc);
        return states_.size() != begins_.back();
    }

    bool operator()(word_t, const char_type &ch)
    {
        const word_t
            current_beg = begins_.back(),
            current_end = states_.size();
        begins_.push_back(current_end);
        if (ch == super::terminator_)
        {
            cur_dist_ = ~word_t(0);
            for (word_t n = current_beg; n != current_end; ++n)
            {
                const word_t rest_len =
                    super::mask_len_ - bits_but_highest(states_[n].first);
                if (rest_len <= dist_ - states_[n].second &&
                    states_[n].second + rest_len < cur_dist_)
                    cur_dist_ = states_[n].second + rest_len;
            }
            return cur_dist_ != ~word_t(0);
        }
        for (word_t n = current_beg; n != current_end; ++n)
        {
            const word_t
                i = states_[n].first,
                e = states_[n].second;
// C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable : 4127)
            if (!SameLength && bits_but_highest(i) == super::mask_len_)
#pragma warning(pop)
            {
                states_.resize(current_end);
                cur_dist_ = e;
                for (++n; n != current_end; ++n)
                {
                    if (bits_but_highest(states_[n].first) == super::mask_len_ &&
                        states_[n].second < cur_dist_)
                        cur_dist_ = states_[n].second;
                }
                return true;
            }
            std::back_insert_iterator<states_vector> next_ins(states_);
            static_cast<this_t*>(this)->transitions(i, e, ch, next_ins);
            // sort & remove duplicates
            std::sort(
                &states_[current_end],
                &states_[states_.size()]);
            states_.resize(std::unique(
                &states_[current_end],
                &states_[states_.size()]) - &states_[0]);
        }
        return states_.size() != current_end;
    }

protected:
    word_t dist_;

private:
    std::vector<word_t> begins_;
    states_vector states_;
    word_t cur_dist_;
};

#else

template <typename This, typename Container, bool SameLength>
class levenshtein_generic
    : public partial_base<Container, SameLength>
{
    typedef This this_t;
    typedef partial_base<Container, SameLength> super;
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
        , states_(1, states_vector(1, std::make_pair(0, 0)))
    {
    }

    word_t distance() const
    {
        return cur_dist_;
    }

    void init()
    {
        states_.clear();
        states_.push_back(states_vector(1, std::make_pair(0, 0)));
    }

    bool operator()(word_t i)
    {
        while (states_.size() > i + 1)
            states_.pop_back();
        return !states_.back().empty();
    }

    bool operator()(word_t, const char_type &ch)
    {
        states_.push_back(states_vector());
        const states_vector &current = states_[states_.size() - 2];
        if (ch == super::terminator_)
        {
            cur_dist_ = ~word_t(0);
            for (states_vector::const_iterator it = current.begin()
                ; it != current.end()
                ; ++it)
            {
                const word_t rest_len = super::mask_len_ - bits_but_highest(it->first);
                if (rest_len <= dist_ - it->second && it->second + rest_len < cur_dist_)
                    cur_dist_ = it->second + rest_len;
            }
            return cur_dist_ != ~word_t(0);
        }
        states_vector &next = states_.back();
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
            if (!SameLength && bits_but_highest(i) == super::mask_len_)
#pragma warning(pop)
            {
                next.clear();
                cur_dist_ = e;
                for (++it; it != current.end(); ++it)
                {
                    if (bits_but_highest(it->first) == super::mask_len_ &&
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
    states_sequence states_;
    word_t cur_dist_;
};

#endif

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
