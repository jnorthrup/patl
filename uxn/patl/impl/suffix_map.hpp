#ifndef PATL_IMPL_SUFFIX_MAP_HPP
#define PATL_IMPL_SUFFIX_MAP_HPP

#include "node.hpp"

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
    explicit node_gen_suffix(const typename T::mapped_type &datum
#ifdef PATL_DEBUG
        , word_t id
#endif
        )
        : datum_(datum)
#ifdef PATL_DEBUG
        , id_(id)
#endif
    {
    }

    const typename T::value_type &get_value() const
    {
        return datum_;
    }
    typename T::value_type &get_value()
    {
        return datum_;
    }

private:
    typename T::mapped_type datum_;
#ifdef PATL_DEBUG
    word_t id_;
#endif
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
    typedef Datum value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;
    typedef node_gen_suffix<this_t> node_type;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
