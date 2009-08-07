/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_NODE_HPP
#define PATL_IMPL_NODE_HPP

#include <algorithm>
#include "../config.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

/// Base class of PATRICIA node
template <typename Node>
class node_generic
{
    typedef Node node_type;

public:
    /// return parent node
    const node_type *get_parent() const
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<const node_type*>(parentid_ & ~word_t(1));
#else
        return parent_;
#endif
    }
    node_type *get_parent()
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<node_type*>(parentid_ & ~word_t(1));
#else
        return parent_;
#endif
    }

    /// return node id relate to parent
    word_t get_parent_id() const
    {
#ifdef PATL_ALIGNHACK
        return parentid_ & word_t(1);
#else
        return tagsid_ >> 2 & word_t(1);
#endif
    }

    /// return child node by id
    const node_type *get_xlink(word_t id) const
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<const node_type*>(linktag_[id] & ~word_t(1));
#else
        return link_[id];
#endif
    }
    node_type *get_xlink(word_t id)
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<node_type*>(linktag_[id] & ~word_t(1));
#else
        return link_[id];
#endif
    }

    /// return tag of child node by id
    word_t get_xtag(word_t id) const
    {
#ifdef PATL_ALIGNHACK
        return linktag_[id] & word_t(1);
#else
        return tagsid_ >> id & word_t(1);
#endif
    }

    /// return the number of distinction bit
    word_t get_skip() const
    {
        return skip_;
    }

    /// set parent info
    void set_parentid(node_type *parent, word_t id)
    {
#ifdef PATL_ALIGNHACK
        parentid_ = reinterpret_cast<word_t>(parent) | id;
#else
        parent_ = parent;
        tagsid_ &= 3;
        tagsid_ |= id << 2;
#endif
    }

    /// set child info by id
    void set_xlinktag(word_t id, const node_type *link, word_t tag)
    {
#ifdef PATL_ALIGNHACK
        linktag_[id] = reinterpret_cast<word_t>(link) | tag;
#else
        link_[id] = const_cast<node_type*>(link);
        tagsid_ &= ~(1 << id);
        tagsid_ |= tag << id;
#endif
    }

    /// set the number of distinction bit
    void set_skip(word_t skip)
    {
        skip_ = skip;
    }

    /// root special initialization
    void init_root()
    {
        set_parentid(0, 0);
        // skip = -1
        set_skip(~word_t(0));
        // left link self-pointed (root)
        set_xlinktag(0, static_cast<node_type*>(this), 1);
        // right link null-pointed (end)
        set_xlinktag(1, 0, 1);
    }

    /// for debug only
    bool integrity() const
    {
        const word_t
            skip = get_skip(),
            tag0 = get_xtag(0),
            tag1 = get_xtag(1);
        const node_type
            *p0 = get_xlink(0),
            *p1 = get_xlink(1);
        return
            skip == ~word_t(0) && p1 == 0 && get_parent() == 0 && get_parent_id() == 0 ||
            !tag0 && p0->get_parent() == this && !p0->get_parent_id() && skip < p0->get_skip() ||
            !tag1 && p1->get_parent() == this && p1->get_parent_id() && skip < p1->get_skip() ||
            tag0 && tag1;
    }

    /// for debug only
    const char *visualize(const node_type *id0) const
    {
        static char buf[256];
        sprintf(buf, "%d skip: %d pt: %d ptId: %d lnk0: %d tag0: %d lnk1: %d tag1: %d\n",
            this - id0,
            get_skip(),
            get_parent() - id0,
            get_parent_id(),
            get_xlink(0) - id0,
            get_xtag(0),
            get_xlink(1) - id0,
            get_xtag(1));
        return buf;
    }

    /// change the root
    void make_root(node_type *old_root)
    {
        std::swap(skip_, old_root->skip_);
#ifdef PATL_ALIGNHACK
        std::swap(parentid_, old_root->parentid_);
        std::swap(linktag_[0], old_root->linktag_[0]);
        std::swap(linktag_[1], old_root->linktag_[1]);
#else
        std::swap(parent_, old_root->parent_);
        std::swap(link_[0], old_root->link_[0]);
        std::swap(link_[1], old_root->link_[1]);
        std::swap(tagsid_, old_root->tagsid_);
#endif
        old_root->get_parent()->set_xlinktag(old_root->get_parent_id(), old_root, 0);
        if (!old_root->get_xtag(0))
            old_root->get_xlink(0)->set_parentid(old_root, 0);
        if (!old_root->get_xtag(1))
            old_root->get_xlink(1)->set_parentid(old_root, 1);
        get_xlink(0)->set_parentid(reinterpret_cast<node_type*>(this), 0);
    }

    /// copy data members from right except value_
    void set_all_but_value(const node_type *right)
    {
        skip_ = right->skip_;
#ifdef PATL_ALIGNHACK
        parentid_ = right->parentid_;
        linktag_[0] = right->linktag_[0];
        linktag_[1] = right->linktag_[1];
#else
        parent_ = right->parent_;
        link_[0] = right->link_[0];
        link_[1] = right->link_[1];
        tagsid_ = right->tagsid_;
#endif
    }

    template <typename Functor>
    void realloc(const Functor &new_ptr)
    {
        if (get_parent())
        {
            set_parentid(new_ptr(get_parent()), get_parent_id());
            set_xlinktag(0, new_ptr(get_xlink(0)), get_xtag(0));
            set_xlinktag(1, new_ptr(get_xlink(1)), get_xtag(1));
        }
        else // if root
            set_xlinktag(0, new_ptr(get_xlink(0)), get_xtag(0));
    }

private:
    /// the number of distinction bit
    word_t skip_;
#ifdef PATL_ALIGNHACK
    /// compact representation of parent node with parent-id
    word_t parentid_;
    /// compact representation of child nodes with their tags
    word_t linktag_[2];
#else
    node_type
        *parent_, /// parent node
        *link_[2]; /// child nodes
    word_t tagsid_; /// parent-id with child tags
#endif
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
