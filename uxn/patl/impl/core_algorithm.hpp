#ifndef PATL_IMPL_CORE_ALGORITHM_GENERIC_HPP
#define PATL_IMPL_CORE_ALGORITHM_GENERIC_HPP

#include "algorithm.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

/// core memory algorithm template
template <typename T, typename This, typename Container>
class core_algorithm_generic
    : public algorithm_generic<T, This, Container>
{
    typedef algorithm_generic<T, This, Container> super;

public:
    typedef typename super::cont_type cont_type;
    typedef typename super::const_node_type_ref const_node_type_ref;
    typedef typename super::node_type_ref node_type_ref;

    /// zero-init default ctor
    explicit core_algorithm_generic(const cont_type *cont = 0)
        : super(cont)
    {
    }

    /// simple init ctor
    core_algorithm_generic(const cont_type *cont, const_node_type_ref q, word_t qid)
        : super(cont, q, qid)
    {
    }

    /// compact init ctor
    core_algorithm_generic(const cont_type *cont, word_t qq)
        : super(cont, qq)
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
