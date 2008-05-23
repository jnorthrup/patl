#ifndef PATL_IMPL_TRIE_MAP_HPP
#define PATL_IMPL_TRIE_MAP_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

template <
    typename Key,
    typename Datum,
    typename BitComp,
    typename Allocator>
class trie_map_traits
{
public:
    typedef Key key_type;
    typedef std::pair<const Key, Datum> value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;

    static const key_type &ref_key(const value_type &val)
    {
        return val.first;
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
