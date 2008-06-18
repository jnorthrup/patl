#ifndef PATL_IMPL_POSTORDER_ITERATOR_HPP
#define PATL_IMPL_POSTORDER_ITERATOR_HPP

#include "vertex.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class postorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
protected:
    typedef Vertex vertex;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;

public:
    explicit postorder_iterator_generic(const vertex &vtx = vertex())
        : vtx_(vtx)
    {
    }

    bool operator==(const postorder_iterator_generic &it) const
    {
        return vtx_ == it.vtx_;
    }
    bool operator!=(const postorder_iterator_generic &it) const
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

    postorder_iterator_generic &operator++()
    {
        if (vtx_.get_qid())
            vtx_.ascend();
        else
        {
            vtx_.toggle();
            vtx_.descend(0);
        }
        return *this;
    }
    postorder_iterator_generic operator++(int)
    {
        const_iterator it(*this);
        ++*this;
        return it;
    }

    postorder_iterator_generic &operator--()
    {
        if (vtx_.get_qtag())
        {
            if (!vtx_.get_qid())
                vtx_.ascend();
            vtx_.toggle();
        }
        else
            vtx_.iterate(1);
        return *this;
    }
    postorder_iterator_generic operator--(int)
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
