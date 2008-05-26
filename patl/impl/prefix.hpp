#ifndef PATL_IMPL_PREFIX_HPP
#define PATL_IMPL_PREFIX_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Container, typename Node>
class prefix_generic
{
    typedef prefix_generic<Container, Node> this_t;
    typedef Node node_type;
    typedef typename Container::key_type key_type;

    static const word_t bit_size = Container::bit_size;

public:
    explicit prefix_generic(const Container *cont, const node_type *q)
        : cont_(cont)
        , q_(q)
    {
    }

    bool operator==(const node_type *b) const
    {
        return q_ == b;
    }
    bool operator!=(const node_type *b) const
    {
        return !operator==(b);
    }

    bool operator==(const this_t &b) const
    {
        return q_ == b.q_;
    }
    bool operator!=(const this_t &b) const
    {
        return !operator==(b);
    }

    const Container *cont() const
    {
        return cont_;
    }

    bool is_root() const
    {
        return q_->get_parent() == 0;
    }

    word_t skip() const
    {
        return q_->get_skip();
    }

    word_t length() const
    {
        return skip() / bit_size;
    }

    key_type key() const
    {
        return cont_->get_key(q_);
    }

    const key_type key(word_t id) const
    {
        return cont_->get_key(q_->get_xlink(id));
    }

    bool leaf(word_t id) const
    {
        return q_->get_xtag(id) != 0;
    }

    template <typename Callback>
    void postorder_init(Callback &cb)
    {
        postorder_descend(cb);
        while (
            q_->get_parent() &&
            (q_->get_skip() / bit_size == q_->get_parent()->get_skip() / bit_size ||
            cb.cond()))
                postorder_ascend(cb);
        if (q_->get_skip() / bit_size == 0)
            q_ = cont_->root().q_;
    }

    template <typename Callback>
    void postorder_incr(Callback &cb)
    {
        do postorder_ascend(cb);
        while (
            q_->get_parent() &&
            (q_->get_skip() / bit_size == q_->get_parent()->get_skip() / bit_size ||
            cb.cond()));
        if (q_->get_skip() / bit_size == 0)
            q_ = cont_->root().q_;
    }

private:
    template <typename Callback>
    void postorder_descend(Callback &cb)
    {
        for (; ; q_ = q_->get_xlink(1))
        {
            while (!q_->get_xtag(0))
                q_ = q_->get_xlink(0);
            if (q_->get_xtag(1))
            {
                cb.incr();
                break;
            }
            else
                cb.push();
        }
    }

    template <typename Callback>
    void postorder_ascend(Callback &cb)
    {
        if (q_->get_parent_id())
        {
            cb.merge();
            q_ = q_->get_parent();
            cb.update();
        }
        else
        {
            q_ = q_->get_parent();
            if (q_->get_xtag(1))
                cb.incr();
            else
            {
                cb.push();
                q_ = q_->get_xlink(1);
                postorder_descend(cb);
            }
        }
    }

    const Container *cont_;
    const node_type *q_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
