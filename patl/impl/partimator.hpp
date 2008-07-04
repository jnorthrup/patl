#ifndef PATL_IMPL_PARTIMATOR_HPP
#define PATL_IMPL_PARTIMATOR_HPP

#include "const_partimator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex, typename Decision>
class partimator_generic
    : public const_partimator_generic<Vertex, Decision>
{
    typedef const_partimator_generic<Vertex, Decision> super;
    typedef partimator_generic<Vertex, Decision> this_t;

public:
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit partimator_generic(
        const Decision &decis = Decision(),
        const vertex &vtx = vertex())
        : super(decis, vtx)
    {
    }

    iterator begin() const
    {
        vertex vtx(*this);
        vtx.descend(0);
        return iterator(vtx);
    }

    iterator end() const
    {
        vertex vtx(*this);
        vtx.move(1);
        return iterator(vtx);
    }

    template <typename Decis2>
    partimator_generic<vertex, Decis2> begin(const Decis2 &decis) const
    {
        vertex vtx(*this);
        vtx.descend_decision(0, decis);
        return partimator_generic<vertex, Decis2>(decis, vtx);
    }

    template <typename Decis2>
    partimator_generic<vertex, Decis2> end(const Decis2 &decis) const
    {
        vertex vtx(*this);
        vtx.move_decision(1, decis);
        return partimator_generic<vertex, Decis2>(decis, vtx);
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return vtx_.value();
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
