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
    typedef preorder_iterator_generic<Vertex> this_t;

public:
    typedef Vertex vertex;
    typedef vertex *pointer;
    typedef vertex &reference;
    typedef const vertex *const_pointer;
    typedef const vertex &const_reference;

    explicit preorder_iterator_generic(const vertex &vtx = vertex())
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
        if (vtx_.get_qtag())
            next_subtree();
        else
            vtx_.iterate(0);
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
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.template descend<1>();
        }
        else
            vtx_.ascend();
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

    void next_subtree()
    {
        vtx_.template move_subtree<1>();
    }
    template <typename Callback>
    void next_subtree(Callback &cb)
    {
        vtx_.template move_subtree<Callback, 1>(cb);
    }

    template <typename Callback>
    void increment(Callback &cb)
    {
        if (vtx_.get_qtag())
            next_subtree(cb);
        else
            vtx_.iterate(0);
    }

    template <typename Callback>
    void decrement(Callback &cb)
    {
        if (vtx_.get_qid())
        {
            vtx_.toggle();
            vtx_.template descend<Callback, 1>(cb);
        }
        else
            vtx_.ascend();
    }

protected:
    vertex vtx_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
