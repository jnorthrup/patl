#ifndef PATL_SUFFIX_SET_HPP
#define PATL_SUFFIX_SET_HPP

#include "impl/suffix_set.hpp"
#include "impl/suffix_generic.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{

template <
    typename Type,
    sword_t Delta = 1, // расстояние в bit_size между соседними суффиксами
    typename BitComp = bit_comparator<Type>,
    typename Allocator = std::allocator<Type> >
class suffix_set
    : public impl::suffix_generic<
        impl::suffix_set_traits<Type, Delta, BitComp, Allocator> >
{
    typedef suffix_set<Type, Delta, BitComp, Allocator> this_t;
    typedef impl::suffix_generic<
        impl::suffix_set_traits<Type, Delta, BitComp, Allocator> > super;

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
        const key_type last = this->keys_ + this->size() * this->delta;
        word_t i = this->size() - 1;
        for (
            ; last + this->trie_[i]->get_skip() / this->bit_size >= from
            ; --i, last - this->delta) ;
        //printf("reindex(count=%d).(size=%d).", size() - 1 - i, size());
        return this->size() - 1 - i;
    }

    void pop_reindex(word_t i)
    {
        while (i--)
            this->pop_back();
        //printf("pop(size=%d).", size());
    }

    void push_reindex(word_t i)
    {
        while (i--)
            this->push_back();
        //printf("push(size=%d).done\n", size());
    }
};

} // namespace patl
} // namespace uxn

#endif
