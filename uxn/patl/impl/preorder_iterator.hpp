/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PREORDER_ITERATOR_HPP
#define PATL_IMPL_PREORDER_ITERATOR_HPP

#include "vertex.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class const_preorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
    typedef const_preorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_preorder_iterator_generic(const vertex &vtx = vertex())
        : vtx_(vtx)
    {
    }

    bool operator==(const this_t &it) const
    {
        return vtx_ == it.vtx_;
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
        return vtx_;
    }

    this_t &operator++()
    {
        if (vtx_.get_qtag())
            next_subtree();
        else
            vtx_.iterate(0);
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
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.template descend<1>();
        }
        else
            vtx_.ascend();
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
        vtx_.template move_subtree<1>();
    }
    template <typename Callback>
    void next_subtree(Callback &cb)
    {
        vtx_.template move_subtree<1>(cb);
    }

    void increment(word_t limit)
    {
        if (vtx_.limited(limit))
            next_subtree();
        else
            vtx_.iterate(0);
    }
    template <typename Callback>
    void increment(Callback &cb)
    {
        if (vtx_.get_qtag())
            next_subtree(cb);
        else
            vtx_.iterate(0);
    }
    template <typename Callback>
    void increment(word_t limit, Callback &cb)
    {
        if (vtx_.limited(limit))
            next_subtree(cb);
        else
            vtx_.iterate(0);
    }

    void decrement(word_t limit)
    {
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.template descend<1>(limit);
        }
        else
            vtx_.ascend();
    }
    template <typename Callback>
    void decrement(Callback &cb)
    {
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.template descend<1>(cb);
        }
        else
            vtx_.ascend();
    }
    template <typename Callback>
    void decrement(word_t limit, Callback &cb)
    {
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.template descend<1>(limit, cb);
        }
        else
            vtx_.ascend();
    }

protected:
    vertex vtx_;
};

template <typename Vertex>
class preorder_iterator_generic
    : public const_preorder_iterator_generic<Vertex>
{
    typedef const_preorder_iterator_generic<Vertex> super;
    typedef preorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef typename vertex::const_vertex const_vertex;
    typedef const_preorder_iterator_generic<const_vertex> const_preorder_iterator;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef vertex *pointer;
    typedef vertex &reference;

    explicit preorder_iterator_generic(const vertex &vtx = vertex())
        : super(vtx)
    {
    }

    operator const_preorder_iterator() const
    {
        return const_preorder_iterator(this->vtx_);
    }

    const_pointer operator->() const
    {
        return &**this;
    }
    const_reference operator*() const
    {
        return this->vtx_;
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return this->vtx_;
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
