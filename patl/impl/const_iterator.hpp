#ifndef PATL_IMPL_CONST_ITERATOR_HPP
#define PATL_IMPL_CONST_ITERATOR_HPP

#include "postorder_iterator.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class const_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        typename Vertex::value_type>
{
    typedef const_iterator_generic<Vertex> this_t;

protected:
    typedef Vertex vertex;
    typedef postorder_iterator_generic<vertex> postorder_iterator;

public:
    typedef typename Vertex::value_type value_type;
    typedef const value_type *pointer;
    typedef const value_type &reference;

    explicit const_iterator_generic(const vertex &vtx = vertex())
        : pit_(vtx)
    {
    }

    operator const vertex&() const
    {
        return *pit_;
    }

    bool operator==(const this_t &it) const
    {
        return pit_ == it.pit_;
    }
    bool operator!=(const this_t &it) const
    {
        return !(*this == it);
    }

    pointer operator->() const
    {
        return &**this;
    }
    reference operator*() const
    {
        return pit_->value();
    }

    this_t &operator++()
    {
        do ++pit_;
        while (!pit_->leaf());
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
        do --pit_;
        while (!pit_->leaf());
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

protected:
    postorder_iterator pit_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
