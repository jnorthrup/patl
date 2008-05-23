#ifndef PATL_SUFFIX_MAP_HPP
#define PATL_SUFFIX_MAP_HPP

#include "impl/suffix_map.hpp"
#include "bit_comp.hpp"
#include "lca_oracle.hpp"

namespace uxn
{
namespace patl
{

template <
    typename Type,
    typename Datum,
    sword_t Delta = 1, // ���������� � bit_size ����� ��������� ����������
    typename BitComp = bit_comparator<Type>,
    typename Allocator = std::allocator<Type> >
class suffix_map
    : public impl::suffix_generic<
    impl::suffix_map_traits<Type, Datum, Delta, BitComp, Allocator> >
{
    typedef suffix_map<Type, Datum, Delta, BitComp, Allocator> this_t;
    typedef impl::suffix_generic<
        impl::suffix_map_traits<Type, Datum, Delta, BitComp, Allocator> > super;

    friend class lca_oracle<this_t>;

public:
    // constructor
    suffix_map(
        Type keys,
        typename super::size_type size = 0x100,
        const BitComp &bit_comp = BitComp(),
        const Allocator &alloc = Allocator())
        : super(keys, size, bit_comp, alloc)
    {
    }

    // ��������� ��������� �������
    // ���������� ����� ���������� �������� � �����
    // ��������� ��������� �������
    const typename super::node_type *push_back(const Datum &datum)
    {
        if (this->endpoint()) // �������� �� ������� �����
        {
            if (this->empty()) // ���� ������ ����� (��� �����)
                return this->push_back_root(typename super::node_type(datum));
            else // ����� - ��������� ���������
                this->reserve(2 * this->capacity());
        }
        return this->push_back_generic(typename super::node_type(datum));
    }
};

} // namespace patl
} // namespace uxn

#endif
