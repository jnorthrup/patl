#ifndef BPATL_IMPL_CONST_BLOCK_NODE_HPP
#define BPATL_IMPL_CONST_BLOCK_NODE_HPP

#include "node.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

/// объекты этого класса используются алгоритмом для итерации
/// этот класс должен быть const_node_type_ref в bpatl_cont's
/// node_type_ref и const_node_type_ref должны заменить (node_type*) и (const node_type*)
/// public-интерфейс этого класса должен совпадать с readonly-интерфейсом node_generic
template <typename Node, typename Container>
class const_block_node_generic
{
    typedef Container cont_type;
    typedef cont_type::node_type node_type;

public:
    /// return parent node
    const_node_type_ref get_parent(const cont_type *cont) const
    {
        return nod_->get_parent(cont, block_);
    }

    /// return node id relate to parent
    word_t get_parent_id() const
    {
        return nod_->get_parent_id();
    }

    /// return child node by id
    const_node_type_ref get_xlink(const cont_type *cont, word_t id) const
    {
        return nod_->get_xlink(cont, block_, id);
    }

    /// return tag of child node by id
    word_t get_xtag(word_t id) const
    {
        return nod_->get_xtag(id);
    }

    /// return the number of distinction bit
    word_t get_skip() const
    {
        return nod_->get_skip();
    }

protected:
    node_block *block_; // будет изменяться в algorithm_generic::init
    node_type *nod_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
