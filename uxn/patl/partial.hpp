/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_PARTIAL_HPP
#define PATL_PARTIAL_HPP

#include "impl/partial_base.hpp"

namespace uxn
{
namespace patl
{

/// параметры шаблона: класс контейнера и флаг совпадени€ длин маски и ключа
/// (дл€ суффикс-контейнеров имеет смысл только false)
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
        const Container &cont,              // экземпл€р контейнера
        const key_type &mask,               // маска с джокерами типа '?'
        word_t mask_len = ~word_t(0),       // длина маски в символах (дл€ бесконечных строк)
        const char_type &joker = '?',       // символ джокера
        const char_type &terminator = char_type()) // sentinel character
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

    /// проверка на возможность присутстви€ символа ch в позиции i образца
    bool operator()(word_t i, const char_type &ch) const
    {
        return i < super::mask_len_
            ? (!SameLength || ch != super::terminator_) && (super::mask_[i] == joker_ || super::mask_[i] == ch)
            : !SameLength || i == super::mask_len_ && ch == super::terminator_;
    }

    /// проверка на возможность присутстви€ бита со значением id в позиции skip
    bool bit_level(word_t skip, word_t id) const
    {
        return skip < super::mask_bit_len_
            ? super::mask_[skip / super::bit_compare::bit_size] == joker_ ||
              super::bit_comp_.get_bit(super::mask_, skip) == id
            : true;
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
        const char_type &terminator = char_type()) // sentinel character
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

    /// проверка на возможность присутстви€ символа ch в позиции i образца
    bool operator()(word_t i, const char_type &ch)
    {
        // pop_back-while removed to determ(...)
        if (i < super::mask_len_ && ch != super::terminator_)
        {
            if (super::mask_[i] == ch)
                return true;
            if (diff_.size() == dist_)
                return false;
            diff_.push_back(i);
            return true;
        }
        return !SameLength && i >= super::mask_len_ ||
               i == super::mask_len_ && ch == super::terminator_;
    }

    /// проверка на возможность присутстви€ бита со значением id в позиции skip
    bool bit_level(word_t skip, word_t id)
    {
        while (!diff_.empty() &&
               skip / super::bit_compare::bit_size <= diff_.back())
            diff_.pop_back();
        return diff_.size() == dist_ && skip < super::mask_bit_len_
            ? super::bit_comp_.get_bit(super::mask_, skip) == id
            : true;
    }

private:
    word_t dist_;
    std::vector<word_t> diff_;
};

} // namespace patl
} // namespace uxn

#endif
