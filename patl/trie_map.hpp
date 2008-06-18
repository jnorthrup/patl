#ifndef PATL_TRIE_MAP_HPP
#define PATL_TRIE_MAP_HPP

#include "impl/trie_map.hpp"
#include "impl/trie_generic.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{

template <
    typename Key,
    typename Datum,
    typename BitComp = bit_comparator<Key>,
    typename Allocator = std::allocator<std::pair<const Key, Datum> > >
class trie_map
    : public impl::trie_generic<
        impl::trie_map_traits<Key, Datum, BitComp, Allocator> >
{
    typedef impl::trie_generic<
        impl::trie_map_traits<Key, Datum, BitComp, Allocator> > super;

public:
    explicit trie_map(
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(bit_comp, alloc)
    {
    }

    template <typename Iter>
    trie_map(
        Iter first,
        Iter last,
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(bit_comp, alloc)
    {
        insert(first, last);
    }

    Datum &operator[](const Key &key)
    {
        if (this->root_)
        {
            typename super::algorithm pal(this, this->root_, 0);
            const word_t l = pal.mismatch(key);
            if (~word_t(0) == l)
                return pal.get_value().second;
            return add(std::make_pair(key, Datum()), pal, l).get_value().second;
        }
        return add_root(std::make_pair(key, Datum())).get_value().second;
    }
};

template <typename Key, typename Datum, typename BitComp, typename Allocator>
inline void swap(trie_map<Key, Datum, BitComp, Allocator> &a,
                 trie_map<Key, Datum, BitComp, Allocator> &b)
{
    a.swap(b);
}

} // namespace patl
} // namespace uxn

#endif
