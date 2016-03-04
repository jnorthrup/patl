/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_VERTEX_HPP
#define PATL_IMPL_VERTEX_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Algorithm>
class const_vertex_generic
    : public Algorithm
{
protected:
    using this_t = const_vertex_generic<Algorithm>;
    using algorithm = Algorithm;

public:
    using const_vertex = this_t;
    using cont_type = typename algorithm::cont_type;

protected:
    using node_type = typename algorithm::node_type;
    using bit_compare = typename cont_type::bit_compare;
    using prefix = typename cont_type::prefix;
    using const_preorder_iterator = typename cont_type::const_preorder_iterator;
    using const_postorder_iterator = typename cont_type::const_postorder_iterator;
    using preorder_iterator = typename cont_type::preorder_iterator;
    using postorder_iterator = typename cont_type::postorder_iterator;
    using const_iterator = typename cont_type::const_iterator;
    using iterator = typename cont_type::iterator;
    using const_reverse_iterator = typename cont_type::const_reverse_iterator;
    using reverse_iterator = typename cont_type::reverse_iterator;

public:
    using key_type = typename algorithm::key_type;
    using const_key_reference = typename algorithm::const_key_reference;
    using value_type = typename algorithm::value_type;

    explicit const_vertex_generic(const algorithm &pal = algorithm())
        : algorithm(pal)
    {
    }

    const_vertex_generic(const cont_type *cont, const node_type *q, word_t qid)
        : algorithm(cont, q, qid)
    {
    }

    const_vertex_generic(const cont_type *cont, word_t qq)
        : algorithm(cont, qq)
    {
    }

    bool operator<(const this_t &r) const
    {
        return algorithm::compact() < r.compact();
    }

    prefix get_prefix() const
    {
        return prefix(algorithm::cont(), algorithm::get_q());
    }

    word_t node_q_uid() const
    {
        return reinterpret_cast<word_t>(algorithm::get_q());
    }

    word_t node_p_uid() const
    {
        return reinterpret_cast<word_t>(algorithm::get_p());
    }

    const_iterator begin() const
    {
        return const_iterator(*postorder_begin());
    }

    const_iterator end() const
    {
        return const_iterator(*postorder_end());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }

    const_postorder_iterator postorder_begin() const
    {
        this_t vtx(*this);
        if (algorithm::compact())
            vtx.descend<0>();
        else
            vtx.toggle();
        return const_postorder_iterator(vtx);
    }

    const_postorder_iterator postorder_end() const
    {
        this_t vtx(*this);
        if (algorithm::get_q())
            vtx.move<1>();
        else
            vtx.toggle();
        return const_postorder_iterator(vtx);
    }

    const_preorder_iterator preorder_begin() const
    {
        const node_type *q = algorithm::get_q();
        return const_preorder_iterator(this_t(algorithm::cont(), q, q ? algorithm::get_qid() : 1));
    }
    const_preorder_iterator preorder_begin(word_t limit) const
    {
        this_t vtx(*this);
        if (algorithm::compact())
            vtx.template descend<0>(limit);
        else
            vtx.toggle();
        return const_preorder_iterator(vtx);
    }

    const_preorder_iterator preorder_end() const
    {
        if (algorithm::get_q())
        {
            const_preorder_iterator pit(*this);
            pit.next_subtree();
            return pit;
        }
        else
        {
            this_t vtx(*this);
            vtx.toggle();
            return const_preorder_iterator(vtx);
        }
    }

    const_key_reference parent_key() const
    {
        return algorithm::get_key(algorithm::get_q());
    }

    const value_type &parent_value() const
    {
        return get_value(get_q());
    }

    word_t skip() const
    {
        return algorithm::get_q()->get_skip();
    }

    /// only for !leaf()
    word_t next_skip() const
    {
        return algorithm::get_p()->get_skip();
    }

    bool limited(word_t limit) const
    {
        return algorithm::get_qtag() || next_skip() >= limit;
    }

    /// trie descend to the leaves
    template <word_t Side>
    void descend()
    {
        while (!algorithm::get_qtag())
            algorithm::iterate(Side);
    }
    template <word_t Side, typename Callback>
    void descend(Callback &cb)
    {
        while (!algorithm::get_qtag())
        {
            cb(this);
            algorithm::iterate(Side);
        }
    }

    /// trie descend to the leaves limited by
    template <word_t Side>
    void descend(word_t limit)
    {
        while (!limited(limit))
            algorithm::iterate(Side);
    }
    template <word_t Side, typename Callback>
    void descend(word_t limit, Callback &cb)
    {
        while (!limited(limit))
        {
            cb(this);
            iterate(Side);
        }
    }

    /// generic part of all move functions
    template <word_t Side>
    void move_subtree()
    {
        while (algorithm::get_qid() == Side)
            algorithm::ascend();
        algorithm::toggle();
    }
    template <word_t Side, typename Callback>
    void move_subtree(Callback &cb)
    {
        while (algorithm::get_qid() == Side)
        {
            algorithm::ascend();
            cb(this);
        }
        algorithm::toggle();
    }

    /// move to the next leaf
    template <word_t Side>
    void move()
    {
        move_subtree<Side>();
        descend<word_t(1) ^ Side>();
    }

    // low-level functions

    word_t get_parent_id() const
    {
        return algorithm::get_q()->get_parent_id();
    }
};

template <typename Algorithm>
class vertex_generic
    : public const_vertex_generic<Algorithm>
{
protected:
    using super = const_vertex_generic<Algorithm>;
    using this_t = vertex_generic<Algorithm>;
    using algorithm = Algorithm;

public:
    using const_vertex = super;
    using cont_type = typename algorithm::cont_type;

protected:
    using node_type = typename algorithm::node_type;
    using bit_compare = typename cont_type::bit_compare;
    using prefix = typename cont_type::prefix;
    using preorder_iterator = typename cont_type::preorder_iterator;
    using postorder_iterator = typename cont_type::postorder_iterator;
    using iterator = typename cont_type::iterator;
    using reverse_iterator = typename cont_type::reverse_iterator;

public:
    using key_type = typename algorithm::key_type;
    using value_type = typename algorithm::value_type;

    explicit vertex_generic(const algorithm &pal = algorithm())
        : super(pal)
    {
    }

    vertex_generic(const cont_type *cont, const node_type *q, word_t qid)
        : super(cont, q, qid)
    {
    }

    vertex_generic(const cont_type *cont, word_t qq)
        : super(cont, qq)
    {
    }

    vertex_generic(const super &vtx)
        : super(vtx)
    {
    }

    operator const const_vertex&() const
    {
        return *this;
    }
    operator const_vertex&()
    {
        return *this;
    }

    iterator begin()
    {
        return iterator(*postorder_begin());
    }

    iterator end()
    {
        return iterator(*postorder_end());
    }

    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    postorder_iterator postorder_begin()
    {
        this_t vtx(*this);
        if (algorithm::compact())
            vtx.template descend<0>();
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    postorder_iterator postorder_end()
    {
        this_t vtx(*this);
        if (algorithm::get_q())
            vtx.template move<1>();
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    preorder_iterator preorder_begin()
    {
        const node_type *q = get_q();
        return preorder_iterator(this_t(cont(), q, q ? get_qid() : 1));
    }
    preorder_iterator preorder_begin(word_t limit)
    {
        this_t vtx(*this);
        if (compact())
            vtx.template descend<0>(limit);
        else
            vtx.toggle();
        return preorder_iterator(vtx);
    }

    preorder_iterator preorder_end()
    {
        if (get_q())
        {
            preorder_iterator pit(*this);
            pit.next_subtree();
            return pit;
        }
        else
        {
            this_t vtx(*this);
            vtx.toggle();
            return preorder_iterator(vtx);
        }
    }

    value_type &parent_value()
    {
        return this->get_value(get_q());
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
