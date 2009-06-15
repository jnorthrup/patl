#ifndef PATL_IMPL_PREORDER_ITERATOR_HPP
#define PATL_IMPL_PREORDER_ITERATOR_HPP

#include "const_preorder_iterator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class preorder_iterator_generic
    : public const_preorder_iterator_generic<Vertex>
{
    typedef const_preorder_iterator_generic<Vertex> super;
    typedef preorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;
    typedef vertex *pointer;
    typedef vertex &reference;

    explicit preorder_iterator_generic(const vertex &vtx = vertex())
        : super(vtx)
    {
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
