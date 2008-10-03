#ifndef PATL_IMPL_LEVELORDER_ITERATOR_HPP
#define PATL_IMPL_LEVELORDER_ITERATOR_HPP

#include "preorder_iterator.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class levelorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
    typedef levelorder_iterator_generic<Vertex> this_t;

protected:
    typedef Vertex vertex;
    typedef vertex *pointer;
    typedef vertex &reference;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;

public:
    explicit levelorder_iterator_generic(
        word_t limit,
        const vertex &vtx = vertex())
        : limit_(limit)
        , vtx_(vtx)
    {
    }

    bool operator==(const this_t &it) const
    {
        return vtx_ == it.vtx_;
    }
    bool operator!=(const this_t &it) const
    {
        return !(*this == it);
    }

    const_pointer operator->() const
    {
        return &**this;
    }
    const_reference operator*() const
    {
        return vtx_;
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
        vtx_.template move_subtree<1>();
        vtx_.template descend<0>(limit_);
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
        vtx_.template move_subtree<0>();
        vtx_.template descend<1>(limit_);
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

private:
    word_t limit_;

protected:
    vertex vtx_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
