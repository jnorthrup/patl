/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_ITERATOR_HPP
#define PATL_IMPL_ITERATOR_HPP

#include <iterator>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename PostorderIter>
class const_iterator_generic
    : public std::iterator<
        std::bidirectional_iterator_tag,
        typename PostorderIter::vertex::value_type>
{
    typedef const_iterator_generic<PostorderIter> this_t;

protected:
    typedef PostorderIter postorder_iterator;
    typedef typename postorder_iterator::vertex vertex;

public:
    typedef typename vertex::value_type value_type;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef const_pointer pointer;
    typedef const_reference reference;

    explicit const_iterator_generic(const vertex &vtx = vertex())
        : pit_(vtx)
    {
    }

    template <typename T>
    const_iterator_generic(const const_iterator_generic<T> &obj)
        : pit_(obj)
    {
    }

    operator const vertex&() const
    {
        return *pit_;
    }

    bool operator==(const this_t &it) const
    {
        return pit_ == it.pit_;
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
        return pit_->value();
    }

    this_t &operator++()
    {
        do ++pit_;
        while (!pit_->get_qtag());
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
        do --pit_;
        while (!pit_->get_qtag());
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }

protected:
    postorder_iterator pit_;
};

template <typename PostorderIter>
class iterator_generic
    : public const_iterator_generic<PostorderIter>
{
    typedef const_iterator_generic<PostorderIter> super;
    typedef iterator_generic<PostorderIter> this_t;
    typedef PostorderIter postorder_iterator;
    typedef typename postorder_iterator::const_postorder_iterator const_postorder_iterator;
    typedef typename postorder_iterator::vertex vertex;
    typedef typename vertex::const_vertex const_vertex;

public:
    typedef typename super::value_type value_type;
    typedef const_iterator_generic<const_postorder_iterator> const_iterator;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef value_type *pointer;
    typedef value_type &reference;

    explicit iterator_generic(const vertex &vtx = vertex())
        : super(vtx)
    {
    }

    operator vertex&()
    {
        return *this->pit_;
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

    this_t &operator--()
    {
        --(*(super*)this);
        return *this;
    }
    this_t operator--(int)
    {
        this_t it(*this);
        --*this;
        return it;
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
