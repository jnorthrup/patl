/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PREFIX_HPP
#define PATL_IMPL_PREFIX_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

#define CSELF static_cast<const this_t*>(this)

template <typename This, typename Container, typename Node>
class prefix_generic
{
    typedef This this_t;

protected:
    typedef Node node_type;

public:
    typedef Container cont_type;
    typedef typename cont_type::key_type key_type;
    typedef typename cont_type::const_key_reference const_key_reference;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename cont_type::vertex vertex;

    prefix_generic(const cont_type *cont, const node_type *q)
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
        q_ = CSELF->get_xlink(q_, id);
    }

    void go_parent()
    {
        q_ = CSELF->get_parent(q_);
    }

    vertex get_vertex() const
    {
        return vertex(cont_, CSELF->get_parent(q_), q_->get_parent_id());
    }

    const cont_type *cont() const
    {
        return cont_;
    }

    bool is_root() const
    {
        return CSELF->get_parent(q_) == 0;
    }

    word_t skip() const
    {
        return q_->get_skip();
    }
    word_t skip(word_t id) const
    {
        return CSELF->get_xlink(q_, id)->get_skip();
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
        return cont_->get_key(CSELF->get_xlink(q_, id));
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
            CSELF->get_parent(q_) &&
            (q_->get_skip() / bit_compare::bit_size ==
            CSELF->get_parent(q_)->get_skip() / bit_compare::bit_size ||
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
            CSELF->get_parent(q_) &&
            (q_->get_skip() / bit_compare::bit_size ==
            CSELF->get_parent(q_)->get_skip() / bit_compare::bit_size ||
            cb.cond()));
        if (q_->get_skip() / bit_compare::bit_size == 0)
            *this = cont_->root_prefix();
    }

private:
    template <typename Callback>
    void postorder_descend(Callback &cb)
    {
        for (; ; q_ = CSELF->get_xlink(q_, 1))
        {
            while (!q_->get_xtag(0))
                q_ = CSELF->get_xlink(q_, 0);
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
            q_ = CSELF->get_parent(q_);
            cb.update();
        }
        else
        {
            q_ = CSELF->get_parent(q_);
            if (q_->get_xtag(1))
                cb.incr();
            else
            {
                cb.push();
                q_ = CSELF->get_xlink(q_, 1);
                postorder_descend(cb);
            }
        }
    }

    const cont_type *cont_;
    const node_type *q_;
};

#undef CSELF

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
