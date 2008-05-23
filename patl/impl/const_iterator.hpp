#ifndef PATL_IMPL_CONST_ITERATOR_HPP
#define PATL_IMPL_CONST_ITERATOR_HPP

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

public:
    typedef typename Vertex::value_type value_type;
    typedef const value_type *pointer;
    typedef const value_type &reference;

    explicit const_iterator(const vertex &vtx = vertex())
        : vtx_(vtx)
    {
    }

    operator const vertex&() const
    {
        return vtx_;
    }
    operator vertex&()
    {
        return vtx_;
    }

    bool operator==(const const_iterator &it) const
    {
        return vtx_ == it.vtx_;
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
        return vtx_.value();
    }

    const_iterator &operator++()
    {
        ++vtx_;
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
        --vtx_;
        return *this;
    }
    const_iterator operator--(int)
    {
        const_iterator it(*this);
        --*this;
        return it;
    }

protected:
    vertex vtx_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
