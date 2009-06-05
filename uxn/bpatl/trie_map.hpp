#ifndef BPATL_TRIE_MAP_HPP
#define BPATL_TRIE_MAP_HPP

#include <stxxl/mng>
#include "impl/node.hpp"
#include "impl/node_block.hpp"
#include "../patl/impl/trivial.hpp"
#include "../patl/impl/algorithm.hpp"

namespace uxn
{
namespace bpatl
{

template <
    typename Key,
    typename Datum,
    typename BitComp = patl::bit_comparator<Key> >
class trie_map
{
public:
    typedef Key key_type;
    typedef std::pair<const Key, Datum> value_type;
    typedef BitComp bit_compare;
};

} // namespace bpatl
} // namespace uxn

#endif
