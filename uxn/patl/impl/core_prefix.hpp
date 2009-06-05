#ifndef PATL_IMPL_CORE_PREFIX_GENERIC_HPP
#define PATL_IMPL_CORE_PREFIX_GENERIC_HPP

#include "prefix.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Container, typename ConstNodeRef, typename NodeRef>
class core_prefix_generic
    : public prefix_generic<core_prefix_generic<Container, ConstNodeRef, NodeRef>, Container, ConstNodeRef, NodeRef>
{
    typedef prefix_generic<core_prefix_generic<Container, ConstNodeRef, NodeRef>, Container, ConstNodeRef, NodeRef> super;

protected:
    typedef typename super::const_node_type_ref const_node_type_ref;
    typedef typename super::node_type_ref node_type_ref;

public:
    typedef typename super::cont_type cont_type;

    core_prefix_generic(const cont_type *cont, const_node_type_ref q)
        : super(cont, q)
    {
    }

    const_node_type_ref get_parent(const_node_type_ref nod) const
    {
        return nod->get_parent();
    }
    node_type_ref get_parent(node_type_ref nod) const
    {
        return nod->get_parent();
    }

    const_node_type_ref get_xlink(const_node_type_ref nod, word_t id) const
    {
        return nod->get_xlink(id);
    }
    node_type_ref get_xlink(node_type_ref nod, word_t id) const
    {
        return nod->get_xlink(id);
    }

    void set_parentid(node_type_ref nod, node_type_ref parent, word_t id) const
    {
        return nod->set_parentid(parent, id);
    }

    void set_xlinktag(node_type_ref nod, word_t id, const_node_type_ref link, word_t tag) const
    {
        return nod->set_xlinktag(id, link, tag);
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
