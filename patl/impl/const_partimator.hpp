#ifndef PATL_IMPL_CONST_PARTIMATOR_HPP
#define PATL_IMPL_CONST_PARTIMATOR_HPP

#include "preorder_iterator.hpp"
#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

/// Partial match iterator class
/// NOTE раньше этот класс был bidirectional, но обратный проход с пропусками
/// NOTE слишком сложен и не имеет практического смысла, потому был удален
template <typename Vertex, typename Decision>
class const_partimator_generic
    : public std::iterator<
        std::forward_iterator_tag,
        typename Vertex::value_type>
{
    typedef const_partimator_generic<Vertex, Decision> this_t;

protected:
    typedef Vertex vertex;
    typedef preorder_iterator_generic<vertex> preorder_iterator;

public:
    typedef typename vertex::value_type value_type;
    typedef const value_type *pointer;
    typedef const value_type &reference;

    explicit const_partimator_generic(
        const Decision &decis = Decision(),
        const vertex &vtx = vertex())
        : decis_(decis)
        , pit_(vtx)
    {
    }

    operator const vertex&() const
    {
        return *pit_;
    }

    bool operator==(const this_t &pt) const
    {
        return pit_ == pt.pit_;
    }
    bool operator!=(const this_t &pt) const
    {
        return !(*this == pt);
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
        for (;;)
        {
            // next node in preorder depth-first
            ++pit_;
            for (;;)
            {
                // reach the leaf
                while (pit_->leaf())
                {
                    // reach the end or find the leaf with appropriate key
                    if (pit_->the_end() || decis_(pit_->key()))
                        return *this;
                    pit_.next_subtree();
                }
                // if current bit is illegal in pattern - skip whole subtree
                if (!decis_(pit_->skip(), pit_->get_qid()))
                    pit_.next_subtree();
                else
                    break;
            }
        }
    }
    this_t operator++(int)
    {
        this_t pt(*this);
        ++*this;
        return pt;
    }

private:
    Decision decis_;

protected:
    preorder_iterator pit_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
