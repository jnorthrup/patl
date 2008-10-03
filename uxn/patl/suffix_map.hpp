#ifndef PATL_SUFFIX_MAP_HPP
#define PATL_SUFFIX_MAP_HPP

#include "impl/suffix_map.hpp"
#include "impl/suffix_generic.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{

template <
    typename Type,
    typename Datum,
    sword_t Delta = 1, // расстояние в bit_size между соседними суффиксами
    typename BitComp = bit_comparator<Type>,
    typename Allocator = std::allocator<Type> >
class suffix_map
    : public impl::suffix_generic<
    impl::suffix_map_traits<Type, Datum, Delta, BitComp, Allocator> >
{
    typedef suffix_map<Type, Datum, Delta, BitComp, Allocator> this_t;
    typedef impl::suffix_generic<
        impl::suffix_map_traits<Type, Datum, Delta, BitComp, Allocator> > super;

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
