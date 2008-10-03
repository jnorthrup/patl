#ifndef PATL_IMPL_TRIE_SET_HPP
#define PATL_IMPL_TRIE_SET_HPP

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
} // namespace patl
} // namespace uxn

#endif
