/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Not finished yet...
-*/
#ifndef PATL_NGRAM_STAT_HPP
#define PATL_NGRAM_STAT_HPP

#include "impl/assoc_generic.hpp"
#include "impl/ring_buffer.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename T>
class node_gen_ngram
    : public node_generic<node_gen_ngram<T> >
{
public:
    const typename T::value_type &get_value(const typename T::key_type &key) const
    {
        return key;
    }

    typename T::value_type &get_value(const typename T::key_type &key)
    {
        return key;
    }
};

template <typename T, typename Container>
class algorithm_gen_ngram
    : public core_algorithm_generic<T, algorithm_gen_ngram<T, Container>, Container>
{
    typedef algorithm_gen_ngram<T, Container> this_t;
    typedef core_algorithm_generic<T, this_t, Container> super;

public:
    typedef Container cont_type;
    typedef typename T::node_type node_type;
    typedef typename T::key_type key_type;
    typedef key_type const_key_reference;
    typedef typename T::value_type value_type;

    explicit algorithm_gen_ngram(const cont_type *cont = 0)
        : super(cont)
    {
    }

    algorithm_gen_ngram(const cont_type *cont, const node_type *q, word_t qid)
        : super(cont, q, qid)
    {
    }

    algorithm_gen_ngram(const cont_type *cont, word_t qq)
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
    typename Char,
    word_t Q,
    word_t N,
    word_t Delta,
    typename BitComp,
    typename Allocator>
class ngram_stat_traits
{
    typedef ngram_stat_traits<Char, Q, N, Delta, BitComp, Allocator> this_t;

public:
    static const word_t Q_ = Q;
    static const word_t N_ = N;
    static const word_t delta = Delta;
    typedef Char *key_type;
    typedef key_type value_type;
    typedef BitComp bit_compare;
    typedef Allocator allocator_type;
    typedef node_gen_ngram<this_t> node_type;
    typedef Algorithm<this_t, Container> algorithm;
};

} // namespace impl

template <
    typename Char,
    word_t Q,
    word_t N = 0,
    word_t Delta = 1,
    typename BitComp = bounded_bit_comparator<Char, Q, N, Delta>,
    typename Allocator = std::allocator<Char> >
class ngram_stat
    : public impl::assoc_generic<
        ngram_stat<Char, Q, N, Delta, BitComp, Allocator>,
        impl::ngram_stat_traits<Char, Q, N, Delta, BitComp, Allocator>,
        impl::core_prefix_generic>
{
    typedef ngram_stat<Char, Q, N, Delta, BitComp, Allocator> this_t;
    typedef impl::ngram_stat_traits<Char, Q, N, Delta, BitComp, Allocator> traits;
    typedef impl::assoc_generic<this_t, traits, core_prefix_generic> super;

    typedef typename traits::node_type node_type;
    typedef typename traits::algorithm algorithm;

    typedef Char char_type;

public:
    typedef typename super::key_type key_type;
    typedef typename super::value_type value_type;
    typedef typename super::bit_compare bit_compare;
    typedef typename super::allocator_type allocator_type;
    typedef typename super::size_type size_type;
    typedef typename super::const_iterator const_iterator;
    typedef typename super::iterator iterator;
    typedef typename super::prefix prefix;
    typedef typename super::vertex vertex;
    typedef typename super::preorder_iterator preorder_iterator;
    typedef typename super::postorder_iterator postorder_iterator;

    typedef typename algorithm::const_key_reference const_key_reference;

    static const word_t n_const = T::n_const;
    static const word_t delta = T::delta;

    static const word_t bit_size = bit_compare::bit_size;

    const_key_reference get_key(const node_type *node) const
    {
        return &keys_[0] + trie_.index_of(node) * delta;
    }

    // constructor
    ngram_stat(
        size_type size,
        const bit_compare & bit_comp,
        const allocator_type &alloc)
        : super(bit_comp)
        , size_(0)
        , trie_(size, alloc)
        , last_(n_const, alloc)
    {
    }

    size_type size() const
    {
        return size_;
    }

    size_type capacity() const
    {
        return trie_.capacity();
    }

    const key_type keys() const
    {
        return &keys_[0];
    }

    bool empty() const
    {
        return !size_;
    }

    void clear()
    {
        size_ = 0;
        trie_.clear();
        keys_.clear();
        freq_.clear();
        this->root_ = 0;
    }

    vertex push_back(char_type ch)
    {
        keys_.push_back(char_type());
        keys_[trie_.size()] = ch;
        if (!this->root_)
        {
            for (word_t i = 1; i != n_const * delta; ++i)
                keys_.push_back(char_type());
            trie_.push_back(node_type());
            this->root_ = trie_.back();
            this->root_->init_root();
            return vertex(this, this->root_, 0);
        }
        const key_type key = &keys_[0] + trie_.size() * delta;
        /*node_type *back = trie_.back();
        word_t skip = max0(static_cast<sword_t>(back->get_skip()) - delta * bit_size);*/
        // reindex
        {
            key_type last = &keys_[0] + trie_.size() * delta;
            word_t i = trie_->size() - 1;
            for (
                ; trie_[i]->get_xlink(0) && last + trie_[i]->get_skip() / bit_size >= from
                ; --i, last -= delta) ;
            i = trie_->size() - 1 - i;
            while (i--)
        }
    }

private:
    word_t size_;
    impl::ring_buffer< // trie structure
        node_type,
        typename allocator_type::template rebind<node_type>::other> trie_;
    impl::ring_buffer< // Q last added trie::vertices
        vertex,
        typename allocator_type::template rebind<vertex>::other> last_;
    std::vector< // keys text data
        char_type,
        typename allocator_type::template rebind<char_type>::other> keys_;
    std::vector< // trie::vertex frequences
        word_t,
        typename allocator_type::template rebind<word_t>::other> freq_;
};

} // namespace patl
} // namespace uxn

#endif
