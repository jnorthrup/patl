#ifndef BPATL_EXTPOSTORDER_ITERATOR_HPP
#define BPATL_EXTPOSTORDER_ITERATOR_HPP

#include "extvertex.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

template <typename ExtVertex, typename PostOrderIter>
class const_extpostorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        ExtVertex>
{
    typedef const_extpostorder_iterator_generic<ExtVertex, PostOrderIter> this_t;

public:
    typedef ExtVertex extvertex;
    typedef const extvertex *const_pointer;
    typedef const extvertex &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    typedef PostOrderIter exploit_t;
    typedef exploit_t const_exploit_t;

    // тут должны быть кторы

    std::auto_ptr<const_exploit_t> exploit() const
    {
        return std::auto_ptr<const_exploit_t>(
            new const_exploit_t(vtx_.exploit()));
    }

protected:
    extvertex vtx_;
};

template <typename ExtVertex, typename PostOrderIter>
class extpostorder_iterator_generic
    : public const_extpostorder_iterator_generic<ExtVertex, PostOrderIter>
{
public:
    std::auto_ptr<exploit_t> exploit()
    {
        return std::auto_ptr<exploit_t>(
            new exploit_t(this->vtx_.exploit()));
    }
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
