#ifndef PATL_IMPL_LEVELORDER_ITERATOR_HPP
#define PATL_IMPL_LEVELORDER_ITERATOR_HPP

#include "vertex.hpp"
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
protected:
    typedef Vertex vertex;
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

    bool operator==(const levelorder_iterator_generic &it) const
    {
        return vtx_ == it.vtx_;
    }
    bool operator!=(const levelorder_iterator_generic &it) const
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

    levelorder_iterator_generic &operator++()
    {
        vtx_.move(1, limit_);
        return *this;
    }
    levelorder_iterator_generic operator++(int)
    {
        const_iterator it(*this);
        ++*this;
        return it;
    }

    levelorder_iterator_generic &operator--()
    {
        vtx_.move(0, limit_);
        return *this;
    }
    levelorder_iterator_generic operator--(int)
    {
        const_iterator it(*this);
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
