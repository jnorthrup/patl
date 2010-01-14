/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
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
    word_t N,
    typename BitComp,
    typename Allocator>
class trie_set_traits
{
public:
    typedef Key key_type;
    typedef Key value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;

    static const word_t N_ = N;

    static const key_type &ref_key(const value_type &val)
    {
        return val;
    }
};

} // namespace impl

template <
    typename Key,
    word_t N = 0,
    typename BitComp = bit_comparator<Key, N>,
    typename Allocator = std::allocator<Key> >
class trie_set
    : public impl::trie_generic<
        impl::trie_set_traits<Key, N, BitComp, Allocator> >
{
    typedef impl::trie_generic<
        impl::trie_set_traits<Key, N, BitComp, Allocator> > super;

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

template <typename Key, word_t N, typename BitComp, typename Allocator>
inline void swap(trie_set<Key, N, BitComp, Allocator> &a,
                 trie_set<Key, N, BitComp, Allocator> &b)
{
    a.swap(b);
}

} // namespace patl
} // namespace uxn

#endif
