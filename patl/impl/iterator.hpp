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
class iterator
    : public const_iterator<Vertex>
{
public:
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit iterator(const vertex &vtx = vertex())
        : const_iterator(vtx)
    {
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return vtx_.value();
    }

    iterator &operator++()
    {
        ++(*(const_iterator*)this);
        return *this;
    }
    iterator operator++(int)
    {
        iterator it(*this);
        ++*this;
        return it;
    }

    iterator &operator--()
    {
        --(*(const_iterator*)this);
        return *this;
    }
    iterator operator--(int)
    {
        iterator it(*this);
        --*this;
        return it;
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
