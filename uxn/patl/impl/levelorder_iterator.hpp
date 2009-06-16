#ifndef PATL_IMPL_LEVELORDER_ITERATOR_HPP
#define PATL_IMPL_LEVELORDER_ITERATOR_HPP

#include "preorder_iterator.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class const_levelorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
    typedef const_levelorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_levelorder_iterator_generic(
        word_t limit,
        const vertex &vtx = vertex())
        : limit_(limit)
        , vtx_(vtx)
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
        vtx_.template move_subtree<1>();
        vtx_.template descend<0>(limit_);
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
        vtx_.template move_subtree<0>();
        vtx_.template descend<1>(limit_);
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

private:
    word_t limit_;

protected:
    vertex vtx_;
};

template <typename Vertex>
class levelorder_iterator_generic
    : public const_levelorder_iterator_generic<Vertex>
{
    typedef const_levelorder_iterator_generic<Vertex> super;
    typedef levelorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef typename vertex::const_vertex const_vertex;
    typedef const_levelorder_iterator_generic<const_vertex> const_levelorder_iterator;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef vertex *pointer;
    typedef vertex &reference;

    explicit levelorder_iterator_generic(
        word_t limit,
        const vertex &vtx = vertex())
        : super(limit, vtx)
    {
    }

    operator const_levelorder_iterator() const
    {
        return const_levelorder_iterator(this->vtx_);
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
