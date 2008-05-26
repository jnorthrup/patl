#ifndef PATL_IMPL_SUFFIX_MAP_HPP
#define PATL_IMPL_SUFFIX_MAP_HPP

#include "impl/suffix_generic.hpp"

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
    explicit node_gen_suffix(const typename T::mapped_type &datum)
        : datum_(datum)
    {
    }

    const typename T::value_type &get_value(
        const typename T::key_type &key) const
    {
        // to avoid C4172: returning address of local variable or temporary
#pragma warning(push)
#pragma warning(disable:4172)
        return std::make_pair(key, datum_);
#pragma warning(pop)
    }
    typename T::value_type &get_value(
        const typename T::key_type &key)
    {
        // to avoid C4172: returning address of local variable or temporary
#pragma warning(push)
#pragma warning(disable:4172)
        return std::make_pair(key, datum_);
#pragma warning(pop)
    }

private:
    typename T::mapped_type datum_;
};

template <
    typename Type,
    typename Datum,
    int Delta,
    typename BitComp,
    typename Allocator>
class suffix_map_traits
{
    typedef suffix_map_traits<Type, Datum, Delta, BitComp, Allocator> this_t;

public:
    typedef Datum mapped_type;
    static const word_t delta = Delta;
    typedef Type key_type;
    typedef std::pair<Type, Datum> value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;
    typedef node_gen_suffix<this_t> node_type;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
