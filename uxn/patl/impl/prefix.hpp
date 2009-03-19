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
    typedef typename Container::const_key_reference const_key_reference;
    typedef typename Container::bit_compare bit_compare;
    typedef typename Container::vertex vertex;

public:
    prefix_generic(const Container *cont, const node_type *q)
        : cont_(cont)
        , q_(q)
    {
    }

    bool operator==(const this_t &b) const
    {
        return q_ == b.q_;
    }
    bool operator!=(const this_t &b) const
    {
        return !operator==(b);
    }

    operator const node_type*() const
    {
        return q_;
    }

    word_t node_uid() const
    {
        return reinterpret_cast<word_t>(q_);
    }

    word_t get_xtag(word_t id) const
    {
        return q_->get_xtag(id);
    }

    word_t get_parent_id() const
    {
        return q_->get_parent_id();
    }

    void go_xlink(word_t id)
    {
        q_ = q_->get_xlink(id);
    }

    void go_parent()
    {
        q_ = q_->get_parent();
    }

    vertex get_vertex() const
    {
        return vertex(cont_, q_->get_parent(), q_->get_parent_id());
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
    word_t skip(word_t id) const
    {
        return q_->get_xlink(id)->get_skip();
    }

    bool symbol_limit(word_t id) const
    {
        return skip() / bit_compare::bit_size != skip(id) / bit_compare::bit_size;
    }

    word_t length() const
    {
        return skip() / bit_compare::bit_size;
    }

    const_key_reference key() const
    {
        return cont_->get_key(q_);
    }

    const_key_reference key(word_t id) const
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
            (q_->get_skip() / bit_compare::bit_size ==
            q_->get_parent()->get_skip() / bit_compare::bit_size ||
            cb.cond()))
                postorder_ascend(cb);
        if (q_->get_skip() / bit_compare::bit_size == 0)
            *this = cont_->root_prefix();
    }

    template <typename Callback>
    void postorder_incr(Callback &cb)
    {
        do postorder_ascend(cb);
        while (
            q_->get_parent() &&
            (q_->get_skip() / bit_compare::bit_size ==
            q_->get_parent()->get_skip() / bit_compare::bit_size ||
            cb.cond()));
        if (q_->get_skip() / bit_compare::bit_size == 0)
            *this = cont_->root_prefix();
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
