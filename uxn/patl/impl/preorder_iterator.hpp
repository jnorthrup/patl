/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PREORDER_ITERATOR_HPP
#define PATL_IMPL_PREORDER_ITERATOR_HPP

#include "algorithm.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Algorithm>
class const_preorder_iterator_generic
    : public std::iterator<std::bidirectional_iterator_tag, Algorithm>
{
    typedef const_preorder_iterator_generic<Algorithm> this_t;

public:
    typedef Algorithm algorithm;
    typedef const algorithm *const_pointer;
    typedef const algorithm &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_preorder_iterator_generic(const algorithm &pal = algorithm())
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
        if (pal_.get_qtag())
            next_subtree();
        else
            pal_.iterate(0);
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
        if (pal_.get_qid())
        {
            pal_.toggle();
            pal_.template descend<1>();
        }
        else
            pal_.ascend();
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

    void next_subtree()
    {
        pal_.template move_subtree<1>();
    }
    template <typename Callback>
    void next_subtree(Callback &cb)
    {
        pal_.template move_subtree<1>(cb);
    }

    void increment(word_t limit)
    {
        if (pal_.limited(limit))
            next_subtree();
        else
            pal_.iterate(0);
    }
    template <typename Callback>
    void increment(Callback &cb)
    {
        if (pal_.get_qtag())
            next_subtree(cb);
        else
            pal_.iterate(0);
    }
    template <typename Callback>
    void increment(word_t limit, Callback &cb)
    {
        if (pal_.limited(limit))
            next_subtree(cb);
        else
            pal_.iterate(0);
    }

    void decrement(word_t limit)
    {
        if (pal_.get_qid())
        {
            pal_.toggle();
            pal_.template descend<1>(limit);
        }
        else
            pal_.ascend();
    }
    template <typename Callback>
    void decrement(Callback &cb)
    {
        if (pal_.get_qid())
        {
            pal_.toggle();
            pal_.template descend<1>(cb);
        }
        else
            pal_.ascend();
    }
    template <typename Callback>
    void decrement(word_t limit, Callback &cb)
    {
        if (pal_.get_qid())
        {
            pal_.toggle();
            pal_.template descend<1>(limit, cb);
        }
        else
            pal_.ascend();
    }

protected:
    algorithm pal_;
};

template <typename Algorithm>
class preorder_iterator_generic
    : public const_preorder_iterator_generic<Algorithm>
{
    typedef const_preorder_iterator_generic<Algorithm> super;
    typedef preorder_iterator_generic<Algorithm> this_t;

public:
    typedef Algorithm algorithm;
    typedef const_preorder_iterator_generic<algorithm> const_preorder_iterator;
    typedef const algorithm *const_pointer;
    typedef const algorithm &const_reference;
    typedef algorithm *pointer;
    typedef algorithm &reference;

    explicit preorder_iterator_generic(const algorithm &pal = algorithm())
        : super(pal)
    {
    }

    operator const_preorder_iterator() const
    {
        return const_preorder_iterator(this->pal_);
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
