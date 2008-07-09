#ifndef PATL_IMPL_CONST_PARTIMATOR_HPP
#define PATL_IMPL_CONST_PARTIMATOR_HPP

#include <iterator>
#include "const_iterator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex, typename Decision>
class const_partimator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        typename Vertex::value_type>
{
    typedef const_partimator_generic<Vertex, Decision> this_t;

protected:
    typedef Vertex vertex;

public:
    typedef const value_type *pointer;
    typedef const value_type &reference;

    explicit const_partimator_generic(
        const Decision &decis = Decision(),
        const vertex &vtx = vertex())
        : decis_(decis)
        , vtx_(vtx)
    {
    }

    operator const vertex&() const
    {
        return vtx_;
    }

    bool operator==(const this_t &pt) const
    {
        return vtx_ == pt.vtx_;
    }
    bool operator!=(const this_t &pt) const
    {
        return !(*this == pt);
    }

    pointer operator->() const
    {
        return &**this;
    }
    reference operator*() const
    {
        return this->vtx_.value();
    }

    this_t &operator++()
    {
        this->vtx_.move_decision(1, decis_);
        return *this;
    }
    this_t operator++(int)
    {
        this_t pt(*this);
        ++*this;
        return pt;
    }

    this_t &operator--()
    {
        this->vtx_.move_decision(0, decis_);
        return *this;
    }
    this_t operator--(int)
    {
        this_t pt(*this);
        --*this;
        return pt;
    }

private:
    Decision decis_;

protected:
    vertex vtx_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
