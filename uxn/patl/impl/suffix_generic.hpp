/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_SUFFIX_GENERIC_HPP
#define PATL_IMPL_SUFFIX_GENERIC_HPP

#include <iterator>
#include "node.hpp"
#include "core_algorithm.hpp"
#include "ring_buffer.hpp"
#include "assoc_generic.hpp"
#include "core_prefix.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename T, typename Container>
class algorithm_gen_suffix
    : public core_algorithm_generic<T, algorithm_gen_suffix<T, Container>, Container>
{
    typedef algorithm_gen_suffix<T, Container> this_t;
    typedef core_algorithm_generic<T, this_t, Container> super;

public:
    typedef Container cont_type;
    typedef typename T::node_type node_type;
    typedef typename T::key_type key_type;
    typedef key_type const_key_reference;
    typedef typename T::value_type value_type;

    explicit algorithm_gen_suffix(const cont_type *cont = 0)
        : super(cont)
    {
    }

    algorithm_gen_suffix(const cont_type *cont, const node_type *q, word_t qid)
        : super(cont, q, qid)
    {
    }

    algorithm_gen_suffix(const cont_type *cont, word_t qq)
        : super(cont, qq)
    {
    }

    this_t construct(const node_type *q, word_t qid) const
    {
        return this_t(this->cont_, q, qid);
    }

    const value_type &get_value() const
    {
        const node_type *p = this->get_p();
        return p->get_value(this->cont_->get_key(p));
    }
    value_type &get_value()
    {
        node_type *p = this->get_p();
        return p->get_value(this->cont_->get_key(p));
    }

    const value_type &get_value(const node_type *p) const
    {
        return p->get_value(this->cont_->get_key(p));
    }
    value_type &get_value(node_type *p)
    {
        return p->get_value(this->cont_->get_key(p));
    }

    const_key_reference get_key() const
    {
        return this->cont_->get_key(this->get_p());
    }

    const_key_reference get_key(const node_type *p) const
    {
        return this->cont_->get_key(p);
    }
};

template <
    typename T,
    template <typename, typename> class Algorithm,
    template <typename, typename> class Prefix,
    typename Container>
struct suffix_generic_traits
    : public T
{
    typedef Algorithm<T, Container> algorithm;
    typedef Prefix<Container, typename T::node_type> prefix;
};

template <typename T>
class suffix_generic
    : public assoc_generic<
        suffix_generic<T>,
        suffix_generic_traits<T, algorithm_gen_suffix, core_prefix_generic, suffix_generic<T> >,
        T::N_>
{
    typedef suffix_generic<T> this_t;
    typedef suffix_generic_traits<T, algorithm_gen_suffix, core_prefix_generic, this_t> traits;
    typedef assoc_generic<this_t, traits, T::N_> super;

protected:
    typedef typename traits::node_type node_type;
    typedef typename traits::algorithm algorithm;

public:
    typedef typename super::key_type key_type;
    typedef typename super::value_type value_type;
    typedef typename super::bit_compare bit_compare;
    typedef typename super::allocator_type allocator_type;
    typedef typename super::size_type size_type;
    typedef typename super::const_iterator const_iterator;
    typedef typename super::iterator iterator;
    typedef typename super::prefix prefix;
    typedef typename super::const_vertex const_vertex;
    typedef typename super::vertex vertex;
    typedef typename super::const_preorder_iterator const_preorder_iterator;
    typedef typename super::const_postorder_iterator const_postorder_iterator;
    typedef typename super::preorder_iterator preorder_iterator;
    typedef typename super::postorder_iterator postorder_iterator;

    typedef typename algorithm::const_key_reference const_key_reference;

    static const word_t N_ = T::N_;
    static const word_t delta = T::delta;

    static const word_t bit_size = bit_compare::bit_size;

    const_key_reference get_key(const node_type *node) const
    {
        return keys_ + trie_.index_of(node) * delta;
    }

    // constructor
    suffix_generic(
        key_type keys,
        size_type size,
        const bit_compare &bit_comp,
        const allocator_type &alloc)
        : super(bit_comp)
        , keys_(keys)
        , trie_(size, alloc)
    {
    }

    size_type size() const
    {
        return trie_.size();
    }

    size_type capacity() const
    {
        return trie_.capacity();
    }

    const key_type keys() const
    {
        return keys_;
    }

    void rebind(const key_type keys)
    {
        keys_ = keys;
    }

    word_t vertex_index_of(const const_vertex &vtx) const
    {
        return
            trie_.index_of(static_cast<const algorithm&>(vtx).get_q()) * 2 |
            vtx.get_qid();
    }

    word_t index_of(const const_vertex &vtx) const
    {
        return trie_.index_of(static_cast<const algorithm&>(vtx).get_p());
    }

    word_t index_of(const prefix &pref) const
    {
        return trie_.index_of(static_cast<const node_type*>(pref));
    }

    prefix prefix_by(word_t id) const
    {
        return prefix(this, trie_[id]);
    }

    const_vertex vertex_by(word_t id, word_t qid) const
    {
        return const_vertex(this, trie_[id], qid);
    }
    vertex vertex_by(word_t id, word_t qid)
    {
        return vertex(this, trie_[id], qid);
    }

    /// return true if suffix_cont is empty OR full
    bool endpoint() const
    {
        return trie_.endpoint();
    }

    /// erase front suffix
    void pop_front()
    {
        node_type *front = trie_.front();
        algorithm pal(this, front, front != this->root_
            ? this->bit_comp_.get_bit(keys_, front->get_skip())
            : 0);
        pal.run(keys_);
        erase_node(pal);
        trie_.pop_front();
        keys_ += delta;
    }

    /// erase back suffix
    void pop_back()
    {
        node_type *back = trie_.back();
        algorithm pal(this, back, back != this->root_
            ? this->bit_comp_.get_bit(get_key(back), back->get_skip())
            : 0);
        pal.run(get_key(back));
        erase_node(pal);
        trie_.pop_back();
    }

    void clear()
    {
        trie_.clear();
        this->root_ = 0;
    }

    void reserve(size_type new_size)
    {
        word_t root_pos = trie_.index_of(this->root_);
        trie_.reserve(new_size);
        this->root_ = this->root_ ? trie_[root_pos] : 0;
    }

    class match_iterator
        : public std::iterator<std::forward_iterator_tag, vertex>
    {
    public:
        typedef const vertex *const_pointer;
        typedef const vertex &const_reference;
        typedef vertex *pointer;
        typedef vertex &reference;

        match_iterator(this_t *cont, const key_type &begin)
            : key_(begin)
            , skip_(0)
            , vtx_(cont, cont->root_, 0)
        {
            skip_ = static_cast<algorithm&>(vtx_).mismatch_suffix(key_, skip_);
        }

        bool operator==(const match_iterator &it) const
        {
            return vtx_ == it.vtx_;
        }
        bool operator!=(const match_iterator &it) const
        {
            return !(*this == it);
        }

        const_pointer operator->() const
        {
            return &**this;
        }
        const_reference operator*() const
        {
            return vtx_;
        }

        pointer operator->()
        {
            return &**this;
        }
        reference operator*()
        {
            return vtx_;
        }

        word_t skip() const
        {
            return skip_;
        }

        word_t length() const
        {
            return skip_ / bit_size;
        }

        const key_type key() const
        {
            return key_;
        }

        match_iterator &operator++()
        {
            skip_ = max0(static_cast<sword_t>(skip_) - delta * bit_size);
            key_ += delta;
            const this_t *cont = vtx_.cont();
            algorithm &pal = static_cast<algorithm&>(vtx_);
            if (skip_)
            {
                const node_type
                    *palP = pal.get_p(),
                    *nextQ = palP == cont->trie_.back()
                        ? cont->root_
                        : cont->trie_.following(palP);
                pal.init(nextQ, 0);
                pal.ascend(skip_);
                const node_type *pretender = pal.get_q();
                if (pretender != cont->root_)
                    pal.init(
                        pretender,
                        cont->bit_comp_.get_bit(key_, pretender->get_skip()));
            }
            else
                pal.init(cont->root_, 0);
            //
            skip_ = pal.mismatch_suffix(key_, skip_);
            return *this;
        }

        match_iterator operator++(int)
        {
            match_iterator it(*this);
            ++*this;
            return it;
        }

    private:
        key_type key_;
        word_t skip_;
        vertex vtx_;
    };

protected:
    vertex push_back_generic(const node_type &init_node)
    {
        const key_type key = keys_ + trie_.size() * delta;
        node_type *back = trie_.back();
        word_t skip = max0(static_cast<sword_t>(back->get_skip()) - delta * bit_size);
        algorithm pal(this, this->root_, 0);
        if (skip)
        {
            node_type *back_p = back->get_xlink(back->get_xlink(1) == back ? 0 : 1);
            pal.init(trie_.following(back_p), 0);
            pal.ascend(skip);
            node_type *pretender = pal.get_q();
            if (pretender != this->root_)
                pal.init(pretender, this->bit_comp_.get_bit(key, pretender->get_skip()));
        }
        //
        skip = pal.mismatch_suffix(key, skip);
        // brave new node
        trie_.push_back(init_node);
        node_type *r = trie_.back();
        // add new node into trie
        const word_t bit = this->bit_comp_.get_bit(key, skip);
        pal.add(r, bit, skip);
        return vertex(this, r, bit);
    }

    vertex push_back_root(const node_type &init_node)
    {
        trie_.push_back(init_node);
        this->root_ = trie_.back();
        this->root_->init_root();
        return vertex(this, this->root_, 0);
    }

    void erase_node(algorithm &pal)
    {
        node_type *p = pal.erase();
        // special case: del root
        if (p == this->root_)
        {
            node_type *q = pal.get_q();
            this->root_ = q == p ? 0 : q;
        }
    }

    // data members
    key_type keys_;
    ring_buffer<
        node_type,
        typename allocator_type::template rebind<node_type>::other> trie_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
