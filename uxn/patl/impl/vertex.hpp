#ifndef PATL_IMPL_VERTEX_HPP
#define PATL_IMPL_VERTEX_HPP

#include "const_vertex.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Algorithm>
class vertex_generic
    : public const_vertex_generic<Algorithm>
{
protected:
    typedef const_vertex_generic<Algorithm> super;
    typedef vertex_generic<Algorithm> this_t;
    typedef Algorithm algorithm;

public:
    typedef super const_vertex;
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

    operator algorithm&()
    {
        return this->pal_;
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
        if (this->compact())
            vtx.template descend<0>();
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    postorder_iterator postorder_end()
    {
        this_t vtx(*this);
        if (this->pal_.get_q())
            vtx.template move<1>();
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    preorder_iterator preorder_begin()
    {
        const node_type *q = this->pal_.get_q();
        return preorder_iterator(this_t(this->pal_.cont(), q, q ? this->get_qid() : 1));
    }

    preorder_iterator preorder_end()
    {
        if (this->pal_.get_q())
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

    levelorder_iterator levelorder_begin(word_t limit)
    {
        this_t vtx(*this);
        if (this->compact())
            vtx.template descend<0>(limit);
        else
            vtx.toggle();
        return levelorder_iterator(limit, vtx);
    }

    levelorder_iterator levelorder_end(word_t limit)
    {
        if (this->pal_.get_q())
            return ++levelorder_iterator(limit, *this);
        else
        {
            this_t vtx(*this);
            vtx.toggle();
            return levelorder_iterator(limit, vtx);
        }
    }

    value_type &value()
    {
        return this->pal_.get_value();
    }

    value_type &parent_value()
    {
        return this->pal_.get_value(this->pal_.get_q());
    }

    this_t sibling()
    {
        return this_t(this->pal_.sibling());
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
