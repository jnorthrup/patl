#ifndef PATL_IMPL_VERTEX_HPP
#define PATL_IMPL_VERTEX_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Algorithm>
class vertex_generic
{
protected:
    typedef vertex_generic<Algorithm> this_t;
    typedef Algorithm algorithm;

public:
    typedef typename algorithm::cont_type cont_type;

protected:
    typedef typename algorithm::node_type node_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename cont_type::prefix prefix;
    typedef typename cont_type::levelorder_iterator levelorder_iterator;
    typedef typename cont_type::preorder_iterator preorder_iterator;
    typedef typename cont_type::postorder_iterator postorder_iterator;
    typedef typename cont_type::const_iterator const_iterator;
    typedef typename cont_type::iterator iterator;
    typedef typename cont_type::const_reverse_iterator const_reverse_iterator;
    typedef typename cont_type::reverse_iterator reverse_iterator;

public:
    typedef typename algorithm::key_type key_type;
    typedef typename algorithm::const_key_reference const_key_reference;
    typedef typename algorithm::value_type value_type;

    explicit vertex_generic(const algorithm &pal = algorithm())
        : pal_(pal)
    {
    }

    vertex_generic(const cont_type *cont, const node_type *q, word_t qid)
        : pal_(cont, q, qid)
    {
    }

    vertex_generic(const cont_type *cont, word_t qq)
        : pal_(cont, qq)
    {
    }

    bool operator==(const this_t &r) const
    {
        return pal_ == r.pal_;
    }
    bool operator!=(const this_t &r) const
    {
        return !(*this == r);
    }

    bool operator<(const this_t &r) const
    {
        return compact() < r.compact();
    }

    operator const algorithm&() const
    {
        return pal_;
    }
    operator algorithm&()
    {
        return pal_;
    }

    prefix get_prefix() const
    {
        return prefix(pal_.cont(), pal_.get_q());
    }

    word_t compact() const
    {
        return pal_.compact();
    }

    word_t node_q_uid() const
    {
        return reinterpret_cast<word_t>(pal_.get_q());
    }

    word_t node_p_uid() const
    {
        return reinterpret_cast<word_t>(pal_.get_p());
    }

    const_iterator begin() const
    {
        return const_iterator(*postorder_begin());
    }
    iterator begin()
    {
        return iterator(*postorder_begin());
    }

    const_iterator end() const
    {
        return const_iterator(*postorder_end());
    }
    iterator end()
    {
        return iterator(*postorder_end());
    }

    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    postorder_iterator postorder_begin() const
    {
        this_t vtx(*this);
        if (compact())
            vtx.descend<0>();
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    postorder_iterator postorder_end() const
    {
        this_t vtx(*this);
        if (pal_.get_q())
            vtx.move<1>();
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    preorder_iterator preorder_begin() const
    {
        const node_type *q = pal_.get_q();
        return preorder_iterator(this_t(cont(), q, q ? get_qid() : 1));
    }

    preorder_iterator preorder_end() const
    {
        if (pal_.get_q())
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

    levelorder_iterator levelorder_begin(word_t limit) const
    {
        this_t vtx(*this);
        if (compact())
            vtx.descend<0>(limit);
        else
            vtx.toggle();
        return levelorder_iterator(limit, vtx);
    }

    levelorder_iterator levelorder_end(word_t limit) const
    {
        if (pal_.get_q())
            return ++levelorder_iterator(limit, *this);
        else
        {
            this_t vtx(*this);
            vtx.toggle();
            return levelorder_iterator(limit, vtx);
        }
    }

    bool the_end() const
    {
        return pal_.the_end();
    }

    bool leaf() const
    {
        return pal_.get_qtag() != 0;
    }

    const_key_reference key() const
    {
        return pal_.get_key();
    }

    const_key_reference parent_key() const
    {
        return pal_.get_key(pal_.get_q());
    }

    word_t prefix_length() const
    {
        return pal_.get_prefix_length();
    }

    const value_type &value() const
    {
        return pal_.get_value();
    }
    value_type &value()
    {
        return pal_.get_value();
    }

    const value_type &parent_value() const
    {
        return pal_.get_value(pal_.get_q());
    }
    value_type &parent_value()
    {
        return pal_.get_value(pal_.get_q());
    }

    word_t skip() const
    {
        return pal_.get_q()->get_skip();
    }

    /// only for !leaf()
    word_t next_skip() const
    {
        return pal_.get_p()->get_skip();
    }

    this_t sibling() const
    {
        return this_t(pal_.sibling());
    }

    /// trie descend to the leaves
    template <word_t Side>
    void descend()
    {
        while (!get_qtag())
            iterate(Side);
    }
    template <typename Callback, word_t Side>
    void descend(Callback &cb)
    {
        while (!get_qtag())
        {
            cb(this);
            iterate(Side);
        }
    }

    /// trie descend to the leaves limited by
    template <word_t Side>
    void descend(word_t limit)
    {
        while (!get_qtag() && next_skip() < limit)
            iterate(Side);
    }

    /// generic part of all move functions
    template <word_t Side>
    void move_subtree()
    {
        while (get_qid() == Side)
            ascend();
        toggle();
    }
    template <typename Callback, word_t Side>
    void move_subtree(Callback &cb)
    {
        while (get_qid() == Side)
        {
            ascend();
            cb(this);
        }
        toggle();
    }

    /// move to the next leaf
    template <word_t Side>
    void move()
    {
        move_subtree<Side>();
        descend<word_t(1) ^ Side>();
    }

    // low-level functions

    word_t mismatch(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        return pal_.mismatch(key, prefixLen);
    }

    word_t get_qid() const
    {
        return pal_.get_qid();
    }

    word_t get_qtag() const
    {
        return pal_.get_qtag();
    }

    word_t get_parent_id() const
    {
        return pal_.get_q()->get_parent_id();
    }

    void toggle()
    {
        pal_.toggle();
    }

    void iterate(word_t id)
    {
        pal_.iterate(id);
    }

    void ascend()
    {
        pal_.ascend();
    }

    void ascend(sword_t prefixLen)
    {
        pal_.ascend(prefixLen);
    }

    void ascend_less(sword_t prefixLen)
    {
        pal_.ascend_less(prefixLen);
    }

    const cont_type *cont() const
    {
        return pal_.cont();
    }

    bit_compare bit_comp() const
    {
        return pal_.bit_comp();
    }

protected:
    algorithm pal_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
