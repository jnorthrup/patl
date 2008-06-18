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
class const_iterator
    : public std::iterator<
        std::bidirectional_iterator_tag,
        typename Vertex::value_type>
{
protected:
    typedef Vertex vertex;
    typedef postorder_iterator_generic<vertex> postorder_iterator;

public:
    typedef typename Vertex::value_type value_type;
    typedef const value_type *pointer;
    typedef const value_type &reference;

    explicit const_iterator(const vertex &vtx = vertex())
        : pit_(vtx)
    {
    }

    operator const vertex&() const
    {
        return *pit_;
    }

    bool operator==(const const_iterator &it) const
    {
        return pit_ == it.pit_;
    }
    bool operator!=(const const_iterator &it) const
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

    const_iterator &operator++()
    {
        do ++pit_;
        while (!pit_->leaf());
        return *this;
    }
    const_iterator operator++(int)
    {
        const_iterator it(*this);
        ++*this;
        return it;
    }

    const_iterator &operator--()
    {
        do --pit_;
        while (!pit_->leaf());
        return *this;
    }
    const_iterator operator--(int)
    {
        const_iterator it(*this);
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
