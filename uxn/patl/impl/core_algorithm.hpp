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
    typedef typename super::cont_type cont_type;
    typedef typename super::node_type node_type;

public:
    /// zero-init default ctor
    explicit core_algorithm_generic(const cont_type *cont = 0)
        : super(cont)
    {
    }

    /// simple init ctor
    core_algorithm_generic(const cont_type *cont, const node_type *q, word_t qid)
        : super(cont, q, qid)
    {
    }

    /// compact init ctor
    core_algorithm_generic(const cont_type *cont, word_t qq)
        : super(cont, qq)
    {
    }

    const node_type *get_parent(const node_type *nod) const
    {
        return nod->get_parent();
    }
    node_type *get_parent(node_type *nod) const
    {
        return nod->get_parent();
    }

    const node_type *get_xlink(const node_type *nod, word_t id) const
    {
        return nod->get_xlink(id);
    }
    node_type *get_xlink(node_type *nod, word_t id) const
    {
        return nod->get_xlink(id);
    }

    void set_parentid(node_type *nod, node_type *parent, word_t id) const
    {
        return nod->set_parentid(parent, id);
    }

    void set_xlinktag(node_type *nod, word_t id, const node_type *link, word_t tag) const
    {
        return nod->set_xlinktag(id, link, tag);
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
