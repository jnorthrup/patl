/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
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
    : public algorithm_generic<T, This, Container, T::N_>
{
    using super = algorithm_generic<T, This, Container, T::N_>;
    using cont_type = typename super::cont_type;
    using node_type = typename super::node_type;

public:
    explicit core_algorithm_generic(const cont_type *cont = 0)
        : super(cont)
    {
    }

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

    void set_parentid(node_type *nod, node_type *parent, word_t id) const
    {
        return nod->set_parentid(parent, id);
    }

    const node_type *get_xlink(const node_type *nod, word_t id) const
    {
        return nod->get_xlink(id);
    }
    node_type *get_xlink(node_type *nod, word_t id) const
    {
        return nod->get_xlink(id);
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
