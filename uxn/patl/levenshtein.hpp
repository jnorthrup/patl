/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_LEVENSHTEIN_HPP
#define PATL_LEVENSHTEIN_HPP

#include "impl/partial_base.hpp"

namespace uxn
{
namespace patl
{

/// Levenshtein distance decision functor
/// based on Levenshtein Automata algorithm
/// see Schulz, Mihov, `Fast String Correction with Levenshtein-Automata'
template <typename Container, bool SameLength = false>
class levenshtein_distance
    : public impl::levenshtein_generic<
        levenshtein_distance<Container, SameLength>,
        Container,
        SameLength>
{
    typedef levenshtein_distance<Container, SameLength> this_t;
    typedef impl::levenshtein_generic<this_t, Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<std::pair<word_t, word_t> > states_vector;

public:
    levenshtein_distance(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, dist, mask, mask_len, terminator)
    {
    }

    void transitions(
        word_t i, word_t e, const char_type ch,
        std::back_insert_iterator<states_vector> next)
    {
        const word_t
            b0 = i,
            b1 = impl::get_min(super::mask_len_, b0 + (super::dist_ + 1 - e));
        if (e < super::dist_)
        {
            if (i < super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, e);
                else
                {
                    word_t j = b0 + 1;
                    for (; j != b1; ++j)
                    {
                        if (ch == super::mask_[j])
                        {
                            next = std::make_pair(i, e + 1);
                            next = std::make_pair(i + 1, e + 1);
                            next = std::make_pair(
                                i + (j - b0 + 1),
                                e + (j - b0));
                            break;
                        } }
                    if (j == b1)
                    {
                        next = std::make_pair(i, e + 1);
                        next = std::make_pair(i + 1, e + 1);
                    } } }
            else if (i == super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, e);
                else
                {
                    next = std::make_pair(i, e + 1);
                    next = std::make_pair(i + 1, e + 1);
                } }
            else // i == super::mask_len_
                next = std::make_pair(super::mask_len_, e + 1);
        }
        else if (i < super::mask_len_ && ch == super::mask_[b0])
            next = std::make_pair(i + 1, super::dist_);
    }
};

/// Levenshtein distance with transpositions decision functor
template <typename Container, bool SameLength = false>
class levenshtein_tp_distance
    : public impl::levenshtein_generic<
        levenshtein_tp_distance<Container, SameLength>,
        Container,
        SameLength>
{
    typedef levenshtein_tp_distance<Container, SameLength> this_t;
    typedef impl::levenshtein_generic<this_t, Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<std::pair<word_t, word_t> > states_vector;

public:
    levenshtein_tp_distance(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, dist, mask, mask_len, terminator)
    {
    }

    void transitions(
        word_t i, word_t e, const char_type ch,
        std::back_insert_iterator<states_vector> next)
    {
        const bool transp_state = (i & highest_bit) != 0;
        i = impl::bits_but_highest(i);
        const word_t
            b0 = i,
            b1 = impl::get_min(super::mask_len_, b0 + (super::dist_ + 1 - e));
        if (e == super::dist_)
        {
            if (transp_state)
            {
                if (i < super::mask_len_ - 1 && ch == super::mask_[b0])
                    next = std::make_pair(i + 2, super::dist_);
            }
            else
            {
                if (i < super::mask_len_ && ch == super::mask_[b0])
                    next = std::make_pair(i + 1, super::dist_);
            } }
        else if (e == 0) // && e < super::dist_
        {
            if (i < super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, 0);
                else if (ch == super::mask_[b0 + 1])
                {
                    next = std::make_pair(i, 1);
                    next = std::make_pair(i | highest_bit, 1);
                    next = std::make_pair(i + 1, 1);
                    next = std::make_pair(i + 2, 1);
                }
                else
                {
                    word_t j = b0 + 2;
                    for (; j != b1; ++j)
                    {
                        if (ch == super::mask_[j])
                        {
                            next = std::make_pair(i, 1);
                            next = std::make_pair(i + 1, 1);
                            next = std::make_pair(i + (j - b0 + 1), j - b0);
                            break;
                        } }
                    if (j == b1)
                    {
                        next = std::make_pair(i, 1);
                        next = std::make_pair(i + 1, 1);
                    } } }
            else if (i == super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, 0);
                else
                {
                    next = std::make_pair(i, 1);
                    next = std::make_pair(i + 1, 1);
                } }
            else // i == super::mask_len_
                next = std::make_pair(i, 1);
        }
        else // 0 < e && e < super::dist_
        {
            if (i < super::mask_len_ - 1)
            {
                if (transp_state)
                {
                    if (ch == super::mask_[b0])
                        next = std::make_pair(i + 2, e);
                }
                else
                {
                    if (ch == super::mask_[b0])
                        next = std::make_pair(i + 1, e);
                    else if (ch == super::mask_[b0 + 1])
                    {
                        next = std::make_pair(i, e + 1);
                        next = std::make_pair(i | highest_bit, e + 1);
                        next = std::make_pair(i + 1, e + 1);
                        next = std::make_pair(i + 2, e + 1);
                    }
                    else
                    {
                        word_t j = b0 + 2;
                        for (; j != b1; ++j)
                        {
                            if (ch == super::mask_[j])
                            {
                                next = std::make_pair(i, e + 1);
                                next = std::make_pair(i + 1, e + 1);
                                next = std::make_pair(
                                    i + (j - b0 + 1),
                                    e + (j - b0));
                                break;
                            } }
                        if (j == b1)
                        {
                            next = std::make_pair(i, e + 1);
                            next = std::make_pair(i + 1, e + 1);
                        } } } }
            else if (i == super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, e);
                else
                {
                    next = std::make_pair(i, e + 1);
                    next = std::make_pair(i + 1, e + 1);
                } }
            else // i == super::mask_len_
                next = std::make_pair(i, e + 1);
        }
    }
};

/// Levenshtein distance with merges and splits decision functor
template <typename Container, bool SameLength = false>
class levenshtein_ms_distance
    : public impl::levenshtein_generic<
        levenshtein_ms_distance<Container, SameLength>,
        Container,
        SameLength>
{
    typedef levenshtein_ms_distance<Container, SameLength> this_t;
    typedef impl::levenshtein_generic<this_t, Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<std::pair<word_t, word_t> > states_vector;

public:
    levenshtein_ms_distance(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // символ окончания строки
        : super(cont, dist, mask, mask_len, terminator)
    {
    }

    void transitions(
        word_t i, word_t e, const char_type ch,
        std::back_insert_iterator<states_vector> next)
    {
        const bool split_state = (i & highest_bit) != 0;
        i = impl::bits_but_highest(i);
        const word_t b0 = i;
        if (e == super::dist_)
        {
            if (i < super::mask_len_)
            {
                if (split_state)
                    next = std::make_pair(i + 1, e);
                else if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, super::dist_);
            } }
        else if (e == 0) // && e < super::dist_
        {
            if (i < super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, e);
                else
                {
                    next = std::make_pair(i, e + 1);
                    next = std::make_pair(i | highest_bit, e + 1);
                    next = std::make_pair(i + 1, e + 1);
                    next = std::make_pair(i + 2, e + 1);
                } }
            else if (i == super::mask_len_ - 1)
            {
                if (ch == super::mask_[b0])
                    next = std::make_pair(i + 1, e);
                else
                {
                    next = std::make_pair(i, e + 1);
                    next = std::make_pair(i | highest_bit, e + 1);
                    next = std::make_pair(i + 1, e + 1);
                } }
            else // i == super::mask_len_
                next = std::make_pair(i, e + 1);
        }
        else // 0 < e && e < super::dist_
        {
            if (i < super::mask_len_ - 1)
            {
                if (split_state)
                    next = std::make_pair(i + 1, e);
                else
                {
                    if (ch == super::mask_[b0])
                        next = std::make_pair(i + 1, e);
                    else
                    {
                        next = std::make_pair(i, e + 1);
                        next = std::make_pair(i | highest_bit, e + 1);
                        next = std::make_pair(i + 1, e + 1);
                        next = std::make_pair(i + 2, e + 1);
                    } } }
            else if (i == super::mask_len_ - 1)
            {
                if (split_state)
                    next = std::make_pair(i + 1, e);
                else
                {
                    if (ch == super::mask_[b0])
                        next = std::make_pair(i + 1, e);
                    else
                    {
                        next = std::make_pair(i, e + 1);
                        next = std::make_pair(i | highest_bit, e + 1);
                        next = std::make_pair(i + 1, e + 1);
                    } } }
            else // i == super::mask_len_
                next = std::make_pair(i, e + 1);
        }
    }
};

} // namespace patl
} // namespace uxn

#endif
