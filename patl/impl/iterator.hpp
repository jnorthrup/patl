#ifndef PATL_IMPL_ITERATOR_HPP
#define PATL_IMPL_ITERATOR_HPP

#include "const_iterator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class iterator_generic
    : public const_iterator_generic<Vertex>
{
    typedef iterator_generic<vertex> this_t;

public:
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit iterator_generic(const vertex &vtx = vertex())
        : const_iterator_generic(vtx)
    {
    }

    operator vertex&()
    {
        return *pit_;
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return pit_->value();
    }

    this_t &operator++()
    {
        ++(*(const_iterator_generic*)this);
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
        --(*(const_iterator_generic*)this);
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
