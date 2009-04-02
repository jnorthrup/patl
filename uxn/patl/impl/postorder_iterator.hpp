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
    typedef postorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef vertex *pointer;
    typedef vertex &reference;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;

    explicit postorder_iterator_generic(const vertex &vtx = vertex())
        : vtx_(vtx)
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
        if (vtx_.get_qid())
            vtx_.ascend();
        else
        {
            vtx_.toggle();
            vtx_.template descend<0>();
        }
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
    this_t operator--(int)
    {
        this_t it(*this);
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
