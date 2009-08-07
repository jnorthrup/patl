/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_CORE_PREFIX_GENERIC_HPP
#define PATL_IMPL_CORE_PREFIX_GENERIC_HPP

#include "prefix.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Container, typename Node>
class core_prefix_generic
    : public prefix_generic<core_prefix_generic<Container, Node>, Container, Node>
{
    typedef prefix_generic<core_prefix_generic<Container, Node>, Container, Node> super;

protected:
    typedef typename super::node_type node_type;

public:
    typedef typename super::cont_type cont_type;

    core_prefix_generic(const cont_type *cont, const node_type *q)
        : super(cont, q)
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
