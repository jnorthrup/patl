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
    : public std::iterator<std::bidirectional_iterator_tag, Vertex>
{
    using this_t = const_postorder_iterator_generic<Vertex>;

public:
    using vertex = Vertex;
    using const_pointer = const vertex*;
    using const_reference = const vertex&;
    using pointer = const_pointer;
    using reference = const_reference;

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
    using super = const_postorder_iterator_generic<Vertex>;
    using this_t = postorder_iterator_generic<Vertex>;

public:
    using vertex = Vertex;
    using const_vertex = typename vertex::const_vertex;
    using const_postorder_iterator = const_postorder_iterator_generic<const_vertex>;
    using const_pointer = const vertex*;
    using const_reference = const vertex&;
    using pointer = vertex*;
    using reference = vertex&;

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
