#ifndef PATL_SUFFIX_SET_HPP
#define PATL_SUFFIX_SET_HPP

#include "impl/suffix_set.hpp"
#include "bit_comp.hpp"
#include "lca_oracle.hpp"

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

    friend class lca_oracle<this_t>;

public:
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
#ifdef _DEBUG
                this->size()
#endif
                ));
            else // иначе - полностью заполнено
                this->reserve(2 * this->capacity());
        }
        return this->push_back_generic(
#ifdef _DEBUG
            typename super::node_type(this->size())
#else
            // неважно, каким узлом инициализировать
            *this->root_
#endif
            );
    }

    word_t count_reindex(const key_type from) const
    {
        const key_type last = keys_ + size() * delta;
        word_t i = size() - 1;
        for (; last + trie_[i]->get_skip() / bit_size >= from; --i, last - delta);
        //printf("reindex(count=%d).(size=%d).", size() - 1 - i, size());
        return size() - 1 - i;
    }

    void pop_reindex(word_t i)
    {
        while (i--)
            pop_back();
        //printf("pop(size=%d).", size());
    }

    void push_reindex(word_t i)
    {
        while (i--)
            push_back();
        //printf("push(size=%d).done\n", size());
    }
};

} // namespace patl
} // namespace uxn

#endif
