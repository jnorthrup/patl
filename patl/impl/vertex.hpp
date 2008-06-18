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

public:
    typedef typename algorithm::key_type key_type;
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

    this_t sibling() const
    {
        return this_t(pal_.sibling());
    }

    // low-level functions

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

    void descend(word_t side)
    {
        pal_.descend(side);
    }

    void ascend()
    {
        pal_.ascend();
    }

protected:
    algorithm pal_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
