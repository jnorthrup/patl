/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PARTIMATOR_HPP
#define PATL_IMPL_PARTIMATOR_HPP

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
template <typename PreorderIter, typename Decision>
class const_partimator_generic
    : public std::iterator<
        std::forward_iterator_tag,
        typename PreorderIter::vertex::value_type>
{
    typedef const_partimator_generic<PreorderIter, Decision> this_t;

protected:
    typedef PreorderIter preorder_iterator;
    typedef typename preorder_iterator::vertex vertex;
    typedef typename vertex::const_vertex const_vertex;
    typedef typename vertex::cont_type cont_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename vertex::key_type key_type;

public:
    typedef typename vertex::value_type value_type;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_partimator_generic(
        const Decision &decis = Decision(),
        const vertex &vtx = vertex())
        : decis_(decis)
        , pit_(vtx)
    {
        decis_.init();
        if (!pit_->the_end())
            next_fit();
    }

    template <typename T>
    const_partimator_generic(const const_partimator_generic<T, Decision> &obj)
        : decis_(obj.decis())
        , pit_(obj)
    {
    }

    operator const const_vertex&() const
    {
        return *pit_;
    }

    const Decision &decis() const
    {
        return decis_;
    }

    bool operator==(const this_t &pt) const
    {
        return pit_ == pt.pit_;
    }
    bool operator!=(const this_t &pt) const
    {
        return !(*this == pt);
    }

    const_pointer operator->() const
    {
        return &**this;
    }
    const_reference operator*() const
    {
        return pit_->value();
    }

    this_t &operator++()
    {
        ++pit_;
        next_fit();
        return *this;
    }
    this_t operator++(int)
    {
        this_t pt(*this);
        ++*this;
        return pt;
    }

private:
    void next_fit()
    {
        for (; ; ++pit_)
            for (;;)
            {
                while (pit_->get_qtag()) // reach the leaf
                {
                    if (pit_->the_end() || prove_branch(true))
                        return;
                }
                if (prove_branch(false))
                    break;
            }
    }

    bool prove_branch(bool leaf)
    {
        const word_t skip0 = max0(static_cast<sword_t>(pit_->skip()));
        // if current bit is illegal in pattern - skip whole subtree
        if (!decis_.bit_level(skip0, pit_->get_qid()))
        {
            pit_.next_subtree();
            return false;
        }
        word_t i = skip0 / bit_compare::bit_size;
        const key_type &key = pit_->key();
        const word_t limit =
            (leaf ? pit_->bit_comp().bit_length(key) : pit_->next_skip()) /
            bit_compare::bit_size;
        for (; i != limit; ++i)
        {
            if (!decis_(i))
            {
// C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable : 4127)
                if (Decision::accept_subtree)
                    return true;
#pragma warning(pop)
                pit_.next_subtree();
                return false;
            }
            if (!decis_(i, key[i]))
            {
                pit_.next_subtree();
                return false;
            }
        }
        return true;
    }

    Decision decis_;

protected:
    preorder_iterator pit_;
};

template <typename PreorderIter, typename Decision>
class partimator_generic
    : public const_partimator_generic<PreorderIter, Decision>
{
    typedef const_partimator_generic<PreorderIter, Decision> super;
    typedef partimator_generic<PreorderIter, Decision> this_t;

protected:
    typedef PreorderIter preorder_iterator;
    typedef typename preorder_iterator::const_preorder_iterator const_preorder_iterator;
    typedef typename preorder_iterator::vertex vertex;
    typedef typename vertex::const_vertex const_vertex;

public:
    typedef typename super::value_type value_type;
    typedef const_partimator_generic<const_preorder_iterator, Decision> const_partimator;
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit partimator_generic(
        const Decision &decis = Decision(),
        const vertex &vtx = vertex())
        : super(decis, vtx)
    {
    }

    pointer operator->()
    {
        return &**this;
    }
    reference operator*()
    {
        return this->pit_->value();
    }

    this_t &operator++()
    {
        ++(*(super*)this);
        return *this;
    }
    this_t operator++(int)
    {
        this_t it(*this);
        ++*this;
        return it;
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
