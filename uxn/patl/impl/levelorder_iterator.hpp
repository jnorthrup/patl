#ifndef PATL_IMPL_LEVELORDER_ITERATOR_HPP
#define PATL_IMPL_LEVELORDER_ITERATOR_HPP

#include "const_levelorder_iterator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class levelorder_iterator_generic
    : public const_levelorder_iterator_generic<Vertex>
{
    typedef const_levelorder_iterator_generic<Vertex> super;
    typedef levelorder_iterator_generic<Vertex> this_t;

protected:
    typedef Vertex vertex;
    typedef vertex *pointer;
    typedef vertex &reference;

public:
    explicit levelorder_iterator_generic(
        word_t limit,
        const vertex &vtx = vertex())
        : super(limit, vtx)
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
