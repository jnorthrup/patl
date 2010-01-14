/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_SUFFIX_MAP_HPP
#define PATL_SUFFIX_MAP_HPP

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
    explicit node_gen_suffix(const typename T::mapped_type &datum
#ifdef PATL_DEBUG
        , word_t id
#endif
        )
        : datum_(datum)
#ifdef PATL_DEBUG
        , id_(id)
#endif
    {
    }

    const typename T::value_type &get_value() const
    {
        return datum_;
    }
    typename T::value_type &get_value()
    {
        return datum_;
    }

private:
    typename T::mapped_type datum_;
#ifdef PATL_DEBUG
    word_t id_;
#endif
};

template <
    typename Type,
    typename Datum,
    word_t N,
    word_t Delta,
    typename BitComp,
    typename Allocator>
class suffix_map_traits
{
    typedef suffix_map_traits<Type, Datum, Delta, BitComp, Allocator> this_t;

public:
    typedef Datum mapped_type;
    typedef Type key_type;
    typedef Datum value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;
    typedef node_gen_suffix<this_t> node_type;

    static const word_t N_ = N;
    static const word_t delta = Delta;
};

} // namespace impl

template <
    typename Type,
    typename Datum,
    word_t N = 0,
    word_t Delta = 1, // расстояние в bit_size между соседними суффиксами
    typename BitComp = bit_comparator<Type, N>,
    typename Allocator = std::allocator<Type> >
class suffix_map
    : public impl::suffix_generic<
    impl::suffix_map_traits<Type, Datum, N, Delta, BitComp, Allocator> >
{
    typedef suffix_map<Type, Datum, N, Delta, BitComp, Allocator> this_t;
    typedef impl::suffix_generic<
        impl::suffix_map_traits<Type, Datum, N, Delta, BitComp, Allocator> > super;

public:
    // constructor
    suffix_map(
        Type keys,
        typename super::size_type size = 0x100,
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(keys, size, bit_comp, alloc)
    {
    }

    // вставляем очередной суффикс
    // возвращает длину совпавшего префикса в битах
    // вставляем очередной суффикс
    const typename super::vertex *push_back(const Datum &datum)
    {
        if (this->endpoint()) // проверка на краевые точки
        {
            if (this->empty()) // если дерево пусто (нет корня)
                return this->push_back_root(typename super::node_type(
                    datum,
#ifdef PATL_DEBUG
                    this->size()
#endif
                    ));
            else // иначе - полностью заполнено
                this->reserve(2 * this->capacity());
        }
        return this->push_back_generic(typename super::node_type(
            datum,
#ifdef PATL_DEBUG
            this->size()
#endif
            ));
    }
};

} // namespace patl
} // namespace uxn

#endif
