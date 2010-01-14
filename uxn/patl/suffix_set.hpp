/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_SUFFIX_SET_HPP
#define PATL_SUFFIX_SET_HPP

#include "impl/suffix_generic.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename T>
class node_gen_suffix
    : public node_generic<node_gen_suffix<T> >
{
public:
#ifdef PATL_DEBUG
    node_gen_suffix<T>(word_t id)
        : id_(id)
    {
    }
#endif

    const typename T::value_type &get_value(const typename T::key_type &key) const
    {
        return key;
    }

    typename T::value_type &get_value(const typename T::key_type &key)
    {
        return key;
    }

private:
#ifdef PATL_DEBUG
    word_t id_;
#endif
};

template <
    typename Type,
    word_t N,
    word_t Delta,
    typename BitComp,
    typename Allocator>
class suffix_set_traits
{
    typedef suffix_set_traits<Type, N, Delta, BitComp, Allocator> this_t;

public:
    static const word_t N_ = N;
    static const word_t delta = Delta;

    typedef Type key_type;
    typedef Type value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;
    typedef node_gen_suffix<this_t> node_type;
};

} // namespace impl

template <
    typename Type,
    word_t N = 0,
    word_t Delta = 1, // расстояние в bit_size между соседними суффиксами
    typename BitComp = bit_comparator<Type, N>,
    typename Allocator = std::allocator<Type> >
class suffix_set
    : public impl::suffix_generic<
        impl::suffix_set_traits<Type, N, Delta, BitComp, Allocator> >
{
    typedef suffix_set<Type, N, Delta, BitComp, Allocator> this_t;
    typedef impl::suffix_generic<
        impl::suffix_set_traits<Type, N, Delta, BitComp, Allocator> > super;

public:
    typedef typename super::key_type key_type;

    // constructor
    suffix_set(
        Type keys,
        typename super::size_type size = 0x100,
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(keys, size, bit_comp, alloc)
    {
    }

    typename super::vertex push_back()
    {
        if (this->endpoint()) // проверка на краевые точки
        {
            if (this->empty()) // если дерево пусто (нет корня)
                return this->push_back_root(typename super::node_type(
#ifdef PATL_DEBUG
                this->size()
#endif
                ));
            else // иначе - полностью заполнено
                this->reserve(2 * this->capacity());
        }
        return this->push_back_generic(
#ifdef PATL_DEBUG
            typename super::node_type(this->size())
#else
            // неважно, каким узлом инициализировать
            *this->root_
#endif
            );
    }

    word_t count_reindex(const key_type from) const
    {
        key_type last = this->keys_ + this->size() * this->delta;
        word_t i = this->size() - 1;
        for (
            ; last + this->trie_[i]->get_skip() / this->bit_size >= from
            ; --i, last -= this->delta) ;
        return this->size() - 1 - i;
    }

    void pop_reindex(word_t i)
    {
        while (i--)
            this->pop_back();
    }

    void push_reindex(word_t i)
    {
        while (i--)
            this->push_back();
    }
};

} // namespace patl
} // namespace uxn

#endif
