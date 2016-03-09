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
    : public std::iterator<std::bidirectional_iterator_tag, typename PostorderIter::vertex::value_type>
{
    using this_t = const_iterator_generic<PostorderIter>;

protected:
    using postorder_iterator = PostorderIter;
    using vertex = typename postorder_iterator::vertex;

public:
    using value_type = typename vertex::value_type;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using pointer = const_pointer;
    using reference = const_reference;

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
        return pit_->get_value();
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
    using super = const_iterator_generic<PostorderIter>;
    using this_t = iterator_generic<PostorderIter>;
    using postorder_iterator = PostorderIter;
    using const_postorder_iterator = typename postorder_iterator::const_postorder_iterator;
    using vertex = typename postorder_iterator::vertex;
    using const_vertex = typename vertex::const_vertex;

public:
    using value_type = typename super::value_type;
    using const_iterator = const_iterator_generic<const_postorder_iterator>;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using reference = value_type&;

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
        return this->pit_->get_value();
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
