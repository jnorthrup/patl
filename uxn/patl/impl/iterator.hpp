#ifndef PATL_IMPL_ITERATOR_HPP
#define PATL_IMPL_ITERATOR_HPP

#include "postorder_iterator.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class const_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        typename Vertex::value_type>
{
    typedef const_iterator_generic<Vertex> this_t;

protected:
    typedef Vertex vertex;
    typedef typename Vertex::const_vertex const_vertex;
    typedef postorder_iterator_generic<vertex> postorder_iterator;
    typedef const_postorder_iterator_generic<vertex> const_postorder_iterator;

public:
    typedef typename Vertex::value_type value_type;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_iterator_generic(const const_vertex &vtx = const_vertex())
        : pit_(vtx)
    {
    }

    operator const const_vertex&() const
    {
        return *static_cast<const const_postorder_iterator&>(pit_);
    }

    bool operator==(const this_t &it) const
    {
        return pit_ == it.pit_;
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
        return static_cast<const const_postorder_iterator&>(pit_)->value();
    }

    this_t &operator++()
    {
        do ++pit_;
        while (!pit_->leaf());
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
        do --pit_;
        while (!pit_->leaf());
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

protected:
    postorder_iterator pit_;
};

template <typename Vertex>
class iterator_generic
    : public const_iterator_generic<Vertex>
{
    typedef const_iterator_generic<Vertex> super;
    typedef iterator_generic<Vertex> this_t;
    typedef Vertex vertex;

public:
    typedef typename super::value_type value_type;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit iterator_generic(const vertex &vtx = vertex())
        : super(vtx)
    {
    }

    operator vertex&()
    {
        return *this->pit_;
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return this->pit_->value();
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
