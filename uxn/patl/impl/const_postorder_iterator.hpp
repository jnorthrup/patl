#ifndef PATL_IMPL_CONST_POSTORDER_ITERATOR_HPP
#define PATL_IMPL_CONST_POSTORDER_ITERATOR_HPP

#include "vertex.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Vertex>
class const_postorder_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        Vertex>
{
    typedef const_postorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef typename vertex::const_vertex const_vertex;
    typedef const const_vertex *const_pointer;
    typedef const const_vertex &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_postorder_iterator_generic(const const_vertex &vtx = vertex())
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
