#ifndef PATL_IMPL_SUFFIX_SET_HPP
#define PATL_IMPL_SUFFIX_SET_HPP

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
#ifdef PATL_DEBUG
    node_gen_suffix<T>(word_t id)
        : id_(id)
    {
    }
#endif

    const typename T::value_type &get_value(const typename T::key_type &key) const
    {
        return key;
    }

    typename T::value_type &get_value(const typename T::key_type &key)
    {
        return key;
    }

private:
#ifdef PATL_DEBUG
    word_t id_;
#endif
};

template <
    typename Type,
    int Delta,
    typename BitComp,
    typename Allocator>
class suffix_set_traits
{
    typedef suffix_set_traits<Type, Delta, BitComp, Allocator> this_t;

public:
    static const word_t delta = Delta;
    typedef Type key_type;
    typedef Type value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;
    typedef node_gen_suffix<this_t> node_type;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
