#ifndef PATL_IMPL_SUFFIX_GENERIC_HPP
#define PATL_IMPL_SUFFIX_GENERIC_HPP

#include <iterator>
#include <vector>
#include <set>
#include "node.hpp"
#include "algorithm.hpp"
#include "ring_buffer.hpp"
#include "assoc_generic.hpp"
#include "bit_comp.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename T, typename Container>
class algorithm_gen_suffix
    : public algorithm_generic<T, algorithm_gen_suffix<T, Container>, Container>
{
    typedef algorithm_gen_suffix<T, Container> this_t;
    typedef algorithm_generic<T, this_t, Container> super;

public:
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
		return this_t(cont_, q, qid);
	}

    const value_type &get_value() const
    {
        const node_type *p = this->get_p();
        return p->get_value(cont_->get_key(p));
    }

    value_type &get_value()
    {
        const node_type *p = this->get_p();
        return p->get_value(cont_->get_key(p));
    }

    const key_type get_key() const
    {
        return cont_->get_key(this->get_p());
    }

    const key_type get_key(const node_type *p) const
    {
        return cont_->get_key(p);
    }
};

template <
    typename T,
    template <typename, typename> class Algorithm,
    typename Container>
struct suffix_generic_traits
    : public T
{
    typedef Algorithm<T, Container> algorithm;
};

template <typename T>
class suffix_generic
    : public assoc_generic<
        suffix_generic<T>,
        suffix_generic_traits<T, algorithm_gen_suffix, suffix_generic<T> > >
{
    typedef suffix_generic<T> this_t;
    typedef suffix_generic_traits<T, algorithm_gen_suffix, this_t > traits;
    typedef assoc_generic<this_t, traits> super;

protected:
    typedef typename traits::node_type node_type;
    typedef typename traits::algorithm algorithm;

public:
    typedef typename super::key_type key_type;
    typedef typename super::value_type value_type;
    typedef typename super::bit_compare bit_compare;
    typedef typename super::allocator_type allocator_type;
    typedef typename super::size_type size_type;
    typedef typename super::iterator iterator;

    static const word_t delta = T::delta;

    static const word_t bit_size = bit_compare::bit_size;

    const key_type get_key(const node_type *node) const
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

    word_t index_of(const vertex &vtx) const
    {
        return trie_.index_of(static_cast<algorithm>(vtx).get_p());
    }

    prefix prefix_by(word_t id) const
    {
        return prefix(this, trie_[id]);
    }

    // возвращает true, если контейнер пуст либо полностью заполнен
    bool endpoint() const
    {
        return trie_.endpoint();
    }

    // удаляем наиболее ранний суффикс
    void pop_front()
    {
        node_type *front = trie_.front();
        algorithm pal(this, front, front != root_
            ? bit_comp_.get_bit(keys_, front->get_skip())
            : 0);
        pal.run(keys_);
        erase_node(pal);
        trie_.pop_front();
        keys_ += delta;
    }

    // удаляем последний добавленный суффикс
    void pop_back()
    {
        node_type *back = trie_.back();
        algorithm pal(this, back, back != root_
            ? bit_comp_.get_bit(get_key(back), back->get_skip())
            : 0);
        pal.run(get_key(back));
        erase_node(pal);
        trie_.pop_back();
    }

    void clear()
    {
        trie_.clear();
        root_ = 0;
    }

    void reserve(size_type newSize)
    {
        word_t root_pos = trie_.index_of(root_);
        trie_.reserve(newSize);
        root_ = root_ ? trie_[root_pos] : 0;
    }

    // check integrity
    word_t integrity()
    {
        for (word_t i = 0; i != trie_.size(); ++i)
        {
            if (!trie_[i]->integrity())
                return i;
        }
        return ~word_t(0);
    }

    template <bool Huge = true>
    class match_iterator
        : public std::iterator<std::forward_iterator_tag, vertex>
    {
        typedef const vertex *const_pointer;
        typedef const vertex &const_reference;

    public:
        match_iterator(this_t *cont, const key_type &begin)
            : cont_(cont)
            , key_(begin)
            , skip_(0)
            , vtx_(cont, cont->root_,0)
        {
            skip_ = ((algorithm&)vtx_).mismatch_suffix(key_, skip_);
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
            algorithm &pal = (algorithm&)vtx_;
            if (skip_)
            {
                const node_type
                    *palP = pal.get_p(),
                    *nextQ = palP == cont_->trie_.back()
                        ? cont_->root_
                        : cont_->trie_.following(palP);
                pal.init(nextQ, 0);
                pal.ascend(skip_);
                const node_type *pretender = pal.get_q();
                if (pretender != cont_->root_)
                    pal.init(
                        pretender,
                        cont_->bit_comp_.get_bit(key_, pretender->get_skip()));
            }
            else
                pal.init(cont_->root_, 0);
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
        this_t *cont_;
        key_type key_;
        word_t skip_;
        vertex vtx_;
    };

protected:
    vertex push_back_generic(const node_type &initNode)
    {
        const key_type key = keys_ + trie_.size() * delta;
        node_type *back = trie_.back();
        word_t skip = max0(static_cast<sword_t>(back->get_skip()) - delta * bit_size);
        algorithm pal(this, root_, 0);
        if (skip)
        {
            node_type *backP = back->get_xlink(word_t(1) ^ back->get_self_id());
            pal.init(trie_.following(backP), 0);
            pal.ascend(skip);
            node_type *pretender = pal.get_q();
            if (pretender != root_)
                pal.init(pretender, bit_comp_.get_bit(key, pretender->get_skip()));
        }
        //
        skip = pal.mismatch_suffix(key, skip);
        // brave new node
        trie_.push_back(initNode);
        node_type *r = trie_.back();
        // add new node into trie
        const word_t bit = bit_comp_.get_bit(key, skip);
        pal.add(r, bit, skip);
        return vertex(algorithm(this, r, bit));
    }

    vertex push_back_root(const node_type &initNode)
    {
        trie_.push_back(initNode);
        root_ = trie_.back();
        root_->init_root();
        return vertex(algorithm(this, root_, 0));
    }

    void erase_node(algorithm &pal)
    {
        node_type *p = pal.erase();
        // special case: del root
        if (p == root_)
        {
            node_type *q = pal.get_q();
            root_ = q == p ? 0 : q;
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
