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
    typedef const_iterator_generic<Vertex> super;
    typedef iterator_generic<Vertex> this_t;
    typedef Vertex vertex;

public:
    typedef typename super::value_type value_type;
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
