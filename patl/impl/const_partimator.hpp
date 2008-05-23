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

template <typename Algorithm, typename Decision>
class const_partimator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        typename Algorithm::value_type>
{
    typedef const_partimator_generic<Algorithm, Decision> this_t;

protected:
    typedef Algorithm algorithm;

public:
    typedef const value_type *pointer;
    typedef const value_type &reference;

    explicit const_partimator_generic(
        const Decision &decis = Decision(),
        const algorithm &pal = algorithm())
        : decis_(decis)
        , pal_(pal)
    {
    }

    operator algorithm() const
    {
        return pal_;
    }

    bool leaf() const
    {
        return pal_.get_qtag() == 1ul;
    }

    const_iterator<algorithm> begin() const
    {
        algorithm pal(pal_);
        pal.template descend<0>();
        return const_iterator<algorithm, value_type>(pal);
    }
    const_iterator<algorithm> end() const
    {
        algorithm pal(pal_);
        pal.template move<1>();
        return const_iterator<algorithm, value_type>(pal);
    }

    template <typename Decis2>
    const_partimator_generic<algorithm, Decis2> begin(const Decis2 &decis) const
    {
        algorithm pal(pal_);
        pal.template descend_decision<0>(decis);
        return const_partimator_generic<algorithm, Decis2>(decis, pal);
    }
    template <typename Decis2>
    const_partimator_generic<algorithm, Decis2> end(const Decis2 &decis) const
    {
        algorithm pal(pal_);
        pal.template move_decision<1>(decis);
        return const_partimator_generic<algorithm, Decis2>(decis, pal);
    }

    bool operator==(const this_t &pt) const
    {
        return pal_ == pt.pal_;
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
        return pal_.get_value();
    }

    this_t &operator++()
    {
        pal_.move_decision(1, decis_);
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
        pal_.move_decision(0, decis_);
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
    algorithm pal_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
