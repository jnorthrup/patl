/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_POSTORDER_ITERATOR_HPP
#define PATL_IMPL_POSTORDER_ITERATOR_HPP

#include "vertex.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class const_postorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
    typedef const_postorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_postorder_iterator_generic(const vertex &vtx = vertex())
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
        if (vtx_.get_qid())
            vtx_.ascend();
        else
        {
            vtx_.toggle();
            vtx_.template descend<0>();
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
        if (vtx_.get_qtag())
        {
            if (!vtx_.get_qid())
                vtx_.ascend();
            vtx_.toggle();
        }
        else
            vtx_.iterate(1);
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

protected:
    vertex vtx_; // const_vertex OR vertex actually
};

template <typename Vertex>
class postorder_iterator_generic
    : public const_postorder_iterator_generic<Vertex>
{
    typedef const_postorder_iterator_generic<Vertex> super;
    typedef postorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef typename vertex::const_vertex const_vertex;
    typedef const_postorder_iterator_generic<const_vertex> const_postorder_iterator;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef vertex *pointer;
    typedef vertex &reference;

    explicit postorder_iterator_generic(const vertex &vtx = vertex())
        : super(vtx)
    {
    }

    operator const_postorder_iterator() const
    {
        return const_postorder_iterator(this->vtx_);
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
