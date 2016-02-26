/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_POSTORDER_ITERATOR_HPP
#define PATL_IMPL_POSTORDER_ITERATOR_HPP

#include "algorithm.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Algorithm>
class const_postorder_iterator_generic
    : public std::iterator<std::bidirectional_iterator_tag, Algorithm>
{
    typedef const_postorder_iterator_generic<Algorithm> this_t;

public:
    typedef Algorithm algorithm;
    typedef const algorithm *const_pointer;
    typedef const algorithm &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_postorder_iterator_generic(const algorithm &pal = algorithm())
        : pal_(pal)
    {
    }

    bool operator==(const this_t &it) const
    {
        return pal_ == it.pal_;
    }
    bool operator!=(const this_t &it) const
    {
        return !(*this == it);
    }

    const_pointer operator->() const
    {
        return &**this;
    }
    const_reference operator*() const
    {
        return pal_;
    }

    this_t &operator++()
    {
        if (pal_.get_qid())
            pal_.ascend();
        else
        {
            pal_.toggle();
            pal_.template descend<0>();
        }
        return *this;
    }
    this_t operator++(int)
    {
        this_t it(*this);
        ++*this;
        return it;
    }

    this_t &operator--()
    {
        if (pal_.get_qtag())
        {
            if (!pal_.get_qid())
                pal_.ascend();
            pal_.toggle();
        }
        else
            pal_.iterate(1);
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

protected:
    vertex pal_; // const_vertex OR vertex actually
};

template <typename Algorithm>
class postorder_iterator_generic
    : public const_postorder_iterator_generic<Algorithm>
{
    typedef const_postorder_iterator_generic<Algorithm> super;
    typedef postorder_iterator_generic<Algorithm> this_t;

public:
    typedef Algorithm algorithm;
    typedef const_postorder_iterator_generic<const_vertex> const_postorder_iterator;
    typedef const algorithm *const_pointer;
    typedef const algorithm &const_reference;
    typedef algorithm *pointer;
    typedef algorithm &reference;

    explicit postorder_iterator_generic(const algorithm &pal = algorithm())
        : super(pal)
    {
    }

    operator const_postorder_iterator() const
    {
        return const_postorder_iterator(this->pal_);
    }

    const_pointer operator->() const
    {
        return &**this;
    }
    const_reference operator*() const
    {
        return this->pal_;
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return this->pal_;
    }

    this_t &operator++()
    {
        ++(*(super*)this);
        return *this;
    }
    this_t operator++(int)
    {
        this_t it(*this);
        ++*this;
        return it;
    }

    this_t &operator--()
    {
        --(*(super*)this);
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
