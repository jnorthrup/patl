#ifndef PATL_IMPL_PARTIMATOR_HPP
#define PATL_IMPL_PARTIMATOR_HPP

#include "const_partimator.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Algorithm, typename Decision>
class partimator_generic
    : public const_partimator_generic<Algorithm, Decision>
{
    typedef const_partimator_generic<Algorithm, Decision> super;
    typedef partimator_generic<Algorithm, Decision> this_t;

public:
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit partimator_generic(
        const Decision &decis = Decision(),
        const algorithm &pal = algorithm())
        : super(decis, pal)
    {
    }

    iterator begin() const
    {
        algorithm pal(*this);
        pal.template descend<0>();
        return iterator(pal);
    }
    iterator end() const
    {
        algorithm pal(*this);
        pal.template move<1>();
        return iterator(pal);
    }

    template <typename Decis2>
    partimator_generic<algorithm, Decis2> begin(const Decis2 &decis) const
    {
        algorithm pal(*this);
        pal.descend_decision(0, decis);
        return partimator_generic<algorithm, Decis2>(decis, pal);
    }
    template <typename Decis2>
    partimator_generic<algorithm, Decis2> end(const Decis2 &decis) const
    {
        algorithm pal(*this);
        pal.move_decision(1, decis);
        return partimator_generic<algorithm, Decis2>(decis, pal);
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return this->pal_.get_value();
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
