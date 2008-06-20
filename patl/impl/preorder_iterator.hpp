#ifndef PATL_IMPL_PREORDER_ITERATOR_HPP
#define PATL_IMPL_PREORDER_ITERATOR_HPP

#include "vertex.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class preorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
protected:
    typedef Vertex vertex;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;

public:
    explicit preorder_iterator_generic(const vertex &vtx = vertex())
        : vtx_(vtx)
    {
    }

    bool operator==(const preorder_iterator_generic &it) const
    {
        return vtx_ == it.vtx_;
    }
    bool operator!=(const preorder_iterator_generic &it) const
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

    preorder_iterator_generic &operator++()
    {
        if (vtx_.get_qtag())
            next_subtree();
        else
            vtx_.iterate(0);
        return *this;
    }
    preorder_iterator_generic operator++(int)
    {
        const_iterator it(*this);
        ++*this;
        return it;
    }

    preorder_iterator_generic &operator--()
    {
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.descend(1);
        }
        else
            vtx_.ascend();
        return *this;
    }
    preorder_iterator_generic operator--(int)
    {
        const_iterator it(*this);
        --*this;
        return it;
    }

protected:
    void next_subtree()
    {
        while (vtx_.get_qid())
            vtx_.ascend();
        vtx_.toggle();
    }

    vertex vtx_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
