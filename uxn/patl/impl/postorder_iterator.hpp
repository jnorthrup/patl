#ifndef PATL_IMPL_POSTORDER_ITERATOR_HPP
#define PATL_IMPL_POSTORDER_ITERATOR_HPP

#include "const_postorder_iterator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class postorder_iterator_generic
    : public const_postorder_iterator_generic<Vertex>
{
    typedef const_postorder_iterator_generic<Vertex> super;
    typedef postorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef vertex *pointer;
    typedef vertex &reference;

    explicit postorder_iterator_generic(const vertex &vtx = vertex())
        : super(vtx)
    {
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return vtx_;
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
