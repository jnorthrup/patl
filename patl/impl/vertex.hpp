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
    typedef typename algorithm::node_type node_type;

public:
    typedef typename algorithm::key_type key_type;
    typedef typename algorithm::value_type value_type;

    explicit vertex_generic(const algorithm &pal = algorithm())
        : pal_(pal)
    {
    }

    template <typename Container>
    vertex_generic(const Container *cont, word_t qq)
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

    bool leaf() const
    {
        return pal_.get_qtag() != 0;
    }

    const key_type &key() const
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

    this_t sibling() const
    {
        return vertex(pal_.sibling());
    }

    this_t &operator++()
    {
        do postorder_incr(); // may be preorder_incr();
        while (!leaf());
        return *this;
    }

    this_t operator++(int)
    {
        this_t vtx(*this);
        ++*this;
        return vtx;
    }

    this_t &operator--()
    {
        do postorder_decr(); // may be preorder_decr();
        while (!leaf());
        return *this;
    }

    this_t operator--(int)
    {
        this_t vtx(*this);
        --*this;
        return vtx;
    }

    void postorder_init()
    {
        if (pal_.get_q())
            pal_.descend(0);
        else
            pal_.toggle();
    }

    void postorder_incr()
    {
        if (pal_.get_qid())
            pal_.ascend();
        else
        {
            pal_.toggle();
            pal_.descend(0);
        }
    }

    void postorder_decr()
    {
        if (pal_.get_qtag())
        {
            if (!pal_.get_qid())
                pal_.ascend();
            pal_.toggle();
        }
        else
            pal_.iterate(1);
    }

    template <typename Decision>
    void partial_match_init(const Decision &decis)
    {
    }

    template <typename Decision>
    void partial_match_incr(const Decision &decis)
    {
    }

    void preorder_init()
    {
        if (!pal_.get_q())
            pal_.toggle();
    }

    void preorder_next_subtree()
    {
        while (pal_.get_qid())
            pal_.ascend();
        pal_.toggle();
    }

    void preorder_incr()
    {
        if (pal_.get_qtag())
            preorder_next_subtree();
        else
            pal_.iterate(0);
    }

    void preorder_decr()
    {
        if (pal_.get_qid())
        {
            pal_.toggle();
            pal_.descend(1);
        }
        else
            pal_.ascend();
    }

protected:
    algorithm pal_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
