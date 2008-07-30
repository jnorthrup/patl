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
/// NOTE ранее decision-functor был бит-ориентированным, сейчас он
/// NOTE символ-ориентирован, так же как и partimator engine
template <typename Vertex, typename Decision>
class const_partimator_generic
    : public std::iterator<
        std::forward_iterator_tag,
        typename Vertex::value_type>
{
    typedef const_partimator_generic<Vertex, Decision> this_t;

protected:
    typedef Vertex vertex;
    typedef typename vertex::cont_type cont_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef preorder_iterator_generic<vertex> preorder_iterator;
    typedef typename vertex::key_type key_type;

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
        decis_.init();
        if (!pit_->the_end())
            prove_branch(false);
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
            ++pit_; // next node in preorder depth-first
            for (;;)
            {
                while (pit_->leaf()) // reach the leaf
                {
                    if (pit_->the_end() || prove_branch(true))
                        return *this;
                }
                if (prove_branch(false))
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

    bool prove_branch(bool leaf)
    {
        const key_type &key = pit_->key();
        word_t i =
            impl::max0(static_cast<sword_t>(pit_->skip())) /
            bit_compare::bit_size;
        const word_t limit =
            (leaf ? pit_->bit_comp().bit_length(key) : pit_->next_skip()) /
            bit_compare::bit_size;
        for (; i != limit; ++i)
        {
            if (!decis_(i, key[i]))
            {
                pit_.next_subtree();
                break;
            }
        }
        return i == limit;
    }

protected:
    preorder_iterator pit_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
