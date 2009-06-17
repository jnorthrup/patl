#ifndef PATL_TRIE_SET_HPP
#define PATL_TRIE_SET_HPP

#include "impl/trie_generic.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <
    typename Key,
    typename BitComp,
    typename Allocator>
class trie_set_traits
{
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;

    static const key_type &ref_key(const value_type &val)
    {
        return val;
    }
};

} // namespace impl

template <
    typename Key,
    typename BitComp = bit_comparator<Key>,
    typename Allocator = std::allocator<Key> >
class trie_set
    : public impl::trie_generic<
        impl::trie_set_traits<Key, BitComp, Allocator> >
{
    typedef impl::trie_generic<
        impl::trie_set_traits<Key, BitComp, Allocator> > super;

public:
    explicit trie_set(
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(bit_comp, alloc)
    {
    }
    template <typename Iter>
    trie_set(Iter first, Iter last,
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(bit_comp, alloc)
    {
        insert(first, last);
    }
};

template <typename Key, typename BitComp, typename Allocator>
inline void swap(trie_set<Key, BitComp, Allocator> &a,
                 trie_set<Key, BitComp, Allocator> &b)
{
    a.swap(b);
}

} // namespace patl
} // namespace uxn

#endif
