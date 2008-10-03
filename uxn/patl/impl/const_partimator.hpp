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
/// NOTE ������ ���� ����� ��� bidirectional, �� �������� ������ � ����������
/// NOTE ������� ������ � �� ����� ������������� ������, ������ ��� ������
/// NOTE ����� decision-functor ��� ���-���������������, ������ ��
/// NOTE ������-������������, ��� �� ��� � partimator engine
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
            next_fit();
    }

    operator const vertex&() const
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
                while (pit_->leaf()) // reach the leaf
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

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
