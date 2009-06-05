#ifndef BPATL_IMPL_BLOCK_NODE_HPP
#define BPATL_IMPL_BLOCK_NODE_HPP

#include "const_block_node.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

/// объекты этого класса используются алгоритмом для итерации
/// этот класс должен быть node_type_ref в bpatl_cont's
/// node_type_ref и const_node_type_ref должны заменить (node_type*) и (const node_type*)
/// public-интерфейс этого класса должен совпадать с node_generic
/// root_ контейнера должен быть типа node_type_ref
template <typename Node, typename Container>
class block_node_generic
    : public const_block_node_generic<Node, Container>
{
public:
    /// return parent node
    node_type_ref get_parent(cont_type *cont)
    {
        return nod_->get_parent(cont, block_);
    }

    /// return child node by id
    node_type_ref get_xlink(cont_type *cont, word_t id)
    {
        return nod_->get_xlink(cont, block_, id);
    }

    /// set parent info
    void set_parentid(cont_type *cont, node_type_ref parent, word_t id)
    {
        nod_->set_parentid(cont, block_, parent, id);
    }

    /// set child info by id
    void set_xlinktag(cont_type *cont, word_t id, const_node_type_ref link, word_t tag)
    {
        nod_->set_xlinktag(cont, block_, id, link, tag);
    }

    /// set the number of distinction bit
    void set_skip(word_t skip)
    {
        nod_->set_skip(skip);
    }

    /// root special initialization
    void init_root(cont_type *cont)
    {
        nod_->init_root(cont, block_);
    }

    /// copy data members from right except value_
    void set_all_but_value(const_node_type_ref right)
    {
        nod_->set_all_but_value(right);
    }
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
