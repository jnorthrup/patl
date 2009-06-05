#ifndef BPATL_IMPL_NODE_HPP
#define BPATL_IMPL_NODE_HPP

#include "../../patl/config.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

/// external memory node_generic with presence in core memory
/// some functions require Container::get_node and Container::set_node
/// for in memory node access and modification
/// объекты этого класса физически представляют узлы
template <typename Node, typename Container>
class node_generic
{
    typedef Node node_type;
    typedef Container cont_type;
    typedef typename cont_type::node_type_ref node_type_ref;
    typedef typename cont_type::const_node_type_ref const_node_type_ref;
    typedef typename cont_type::node_block node_block;
    typedef typename cont_type::value_type value_type;
    typedef typename cont_type::key_type key_type;

public:
    /// return parent node
    const_node_type_ref get_parent(const cont_type *cont, const node_block *block) const
    {
        return cont->get_node(block, parentid_ & ~highest_bit);
    }
    node_type_ref get_parent(cont_type *cont, node_block *block)
    {
        return cont->get_node(block, parentid_ & ~highest_bit);
    }

    /// return node id relate to parent
    word_t get_parent_id() const
    {
        return parentid_ >> bits_in_word - 1;
    }

    /// return child node by id
    const_node_type_ref get_xlink(const cont_type *cont, const node_block *block, word_t id) const
    {
        return cont->get_node(block, linktag_[id] & ~highest_bit);
    }
    node_type_ref get_xlink(cont_type *cont, node_block *block, word_t id)
    {
        return cont->get_node(block, linktag_[id] & ~highest_bit);
    }

    /// return tag of child node by id
    word_t get_xtag(word_t id) const
    {
        return linktag_[id] >> bits_in_word - 1;
    }

    /// return the number of distinction bit
    word_t get_skip() const
    {
        return skip_;
    }

    /// set parent info
    void set_parentid(cont_type *cont, node_block *block, node_type_ref parent, word_t id)
    {
        parentid_ = cont->set_node(block, parent) | id << bits_in_word - 1;
    }

    /// set child info by id
    void set_xlinktag(cont_type *cont, node_block *block, word_t id, const_node_type_ref link, word_t tag)
    {
        linktag_[id] = cont->set_node(block, link) | tag << bits_in_word - 1;
    }

    /// set the number of distinction bit
    void set_skip(word_t skip)
    {
        skip_ = skip;
    }

    /// root special initialization
    void init_root(cont_type *cont, node_block *block)
    {
        set_parentid(cont, block, 0, 0);
        // skip = -1
        set_skip(~word_t(0));
        // left link self-pointed (root)
        set_xlinktag(cont, block, 0, static_cast<node_type*>(this), 1);
        // right link null-pointed (end)
        set_xlinktag(cont, block, 1, 0, 1);
    }

    /// copy data members from right except value_
    void set_all_but_value(const node_type *right)
    {
        skip_ = right->skip_;
        parentid_ = right->parentid_;
        linktag_[0] = right->linktag_[0];
        linktag_[1] = right->linktag_[1];
    }

    /// return value stored in node
    value_type &get_value()
    {
        return value_;
    }
    const value_type &get_value() const
    {
        return value_;
    }

    const key_type &get_key() const
    {
        // этот ref_key должен быть свой для каждого контейнера (set или map)
        return T::ref_key(value_);
    }

private:
    /// the number of distinction bit
    word_t skip_;
    /// offset of parent node with parent-id in msb
    word_t parentid_;
    /// offset of child nodes with their tags in msb
    word_t linktag_[2];
    /// value - key & datum or sole key
    value_type value_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
