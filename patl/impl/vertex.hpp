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
    typedef typename algorithm::cont_type cont_type;
    typedef typename algorithm::node_type node_type;
    typedef typename cont_type::prefix prefix;
    typedef typename cont_type::levelorder_iterator levelorder_iterator;
    typedef typename cont_type::preorder_iterator preorder_iterator;
    typedef typename cont_type::postorder_iterator postorder_iterator;

public:
    typedef typename algorithm::key_type key_type;
    typedef typename algorithm::value_type value_type;

    explicit vertex_generic(const algorithm &pal = algorithm())
        : pal_(pal)
    {
    }

    explicit vertex_generic(const cont_type *cont)
        : pal_(cont, (const node_type*)cont->root(), 0)
    {
    }

    vertex_generic(const prefix &pref, word_t qid)
        : pal_(pref.cont(), (const node_type*)pref, qid)
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

    operator const algorithm&() const
    {
        return pal_;
    }
    operator algorithm&()
    {
        return pal_;
    }

    word_t compact() const
    {
        return pal_.compact();
    }

    postorder_iterator postorder_begin() const
    {
        this_t vtx(*this);
        if (compact())
            vtx.descend(0);
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    postorder_iterator postorder_end() const
    {
        this_t vtx(*this);
        if (compact() != 1)
            vtx.move(1);
        return postorder_iterator(vtx);
    }

    preorder_iterator preorder_begin() const
    {
        const node_type *q = pal_.get_q();
        return preorder_iterator(this_t(cont(), q, q ? get_qid() : 1));
    }

    preorder_iterator preorder_end() const
    {
        this_t vtx(*this);
        if (compact() != 1)
            vtx.move(1);
        return preorder_iterator(vtx);
    }

    levelorder_iterator levelorder_begin(word_t limit) const
    {
        this_t vtx(*this);
        if (compact())
            vtx.descend(0, limit);
        else
            vtx.toggle();
        return levelorder_iterator(limit, vtx);
    }

    levelorder_iterator levelorder_end(word_t limit) const
    {
        this_t vtx(*this);
        if (compact() != 1)
            vtx.move(1, limit);
        return levelorder_iterator(limit, vtx);
    }

    bool leaf() const
    {
        return pal_.get_qtag() != 0;
    }

    key_type key() const
    {
        return pal_.get_key();
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
    void descend(word_t side)
    {
        while (!get_qtag())
            iterate(side);
    }

    /// trie descend to the leaves limited by prefix length
    void descend(word_t side, word_t prefixLen)
    {
        while (!get_qtag() && next_skip() < prefixLen)
            iterate(side);
    }

    /// trie descend to the leaves driven by decision functor
    template <typename Decision>
    void descend_decision(word_t side, Decision &decis)
    {
        for (;;)
        {
            while (!get_qtag())
                iterate(decis(next_skip(), side)
                    ? side
                    : word_t(1) ^ side);
            if (!pal_.get_p())
                return;
            const word_t skip = decis(key());
            if (skip == ~word_t(0))
                return;
            pal_.ascend(skip);
            do move_generic(word_t(1) ^ side);
            while (
                pal_.get_q()->get_parent() &&
                //skip() != ~word_t(0) &&
                !decis(this->skip(), get_qid()));
            /*if (!pal_.get_q()->get_parent())
            return;*/
        }
    }

    /// generic part of all move functions
    void move_generic(word_t side)
    {
        if (get_qid() == side)
        {
            node_type *cur = pal_.get_q();
            for (; cur->get_parent_id() == side; cur = cur->get_parent()) ;
            pal_.init(cur->get_parent(), side);
        }
        else
            toggle();
    }

    /// move to the next leaf
    void move(word_t side)
    {
        move_generic(side);
        descend(word_t(1) ^ side);
    }

    /// move to the next leaf limited by prefix length
    void move(word_t side, word_t prefixLen)
    {
        move_generic(side);
        descend(word_t(1) ^ side, prefixLen);
    }

    /// move to the next node driven by decision functor
    template <typename Decision>
    void move_decision(word_t side, Decision &decis)
    {
        do move_generic(side);
        while (
            pal_.get_q()->get_parent() &&
            //skip() != ~word_t(0) &&
            !decis(skip(), get_qid()));
        descend_decision(word_t(1) ^ side, decis);
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

    const cont_type *cont() const
    {
        return pal_.get_cont();
    }

protected:
    algorithm pal_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
