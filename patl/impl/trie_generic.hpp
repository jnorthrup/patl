#ifndef PATL_IMPL_TRIE_GENERIC_HPP
#define PATL_IMPL_TRIE_GENERIC_HPP

#include <algorithm>
#include "node.hpp"
#include "algorithm.hpp"
#include "assoc_generic.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename T>
class node_gen
    : public node_generic<node_gen<T> >
{
    // to avoid warning C4512
    node_gen &operator=(const node_gen&);

public:
    explicit node_gen(const typename T::value_type &value)
        : value_(value)
    {
    }

    // return stored value in node_type
    typename T::value_type &get_value()
    {
        return value_;
    }
    const typename T::value_type &get_value() const
    {
        return value_;
    }

    const typename T::key_type &get_key() const
    {
        return T::ref_key(value_);
    }

private:
    typename T::value_type value_;
};

template <typename T>
class algorithm_gen
    : public algorithm_generic<T, algorithm_gen<T> >
{
    typedef algorithm_generic<T, algorithm_gen<T> > super;

public:
    typedef typename super::node_type node_type;
    typedef typename super::value_type value_type;

    algorithm_gen()
        : super()
    {
    }
    template <typename Smth>
    explicit algorithm_gen(const Smth*)
        : super()
    {
    }
    template <typename Smth>
    algorithm_gen(const Smth*, const node_type *q, word_t qid)
        : super(q, qid)
    {
    }

	algorithm_gen construct(const node_type *q, word_t qid) const
	{
		return algorithm_gen(0, q, qid);
	}

    // return value stored in algorithm
    value_type &get_value()
    {
        return this->get_p()->get_value();
    }
    const value_type &get_value() const
    {
        return this->get_p()->get_value();
    }

    const key_type &get_key() const
    {
        return this->get_p()->get_key();
    }

    const key_type &get_key(const node_type *p) const
    {
        return p->get_key();
    }
};

template <typename T, template <typename> class Node>
struct algorithm_gen_traits
    : public T
{
    typedef Node<T> node_type;
};

template <
    typename T,
    template <typename> class Node,
    template <typename> class Algorithm>
struct trie_generic_traits
    : public algorithm_gen_traits<T, Node>
{
    typedef Algorithm<algorithm_gen_traits<T, Node> > algorithm;
};

template <typename T>
class trie_generic
    : public assoc_generic<
        trie_generic<T>,
        trie_generic_traits<T, node_gen, algorithm_gen> >
{
    typedef trie_generic<T> this_t;
    typedef trie_generic_traits<T, node_gen, algorithm_gen> traits;
    typedef assoc_generic<trie_generic<T>, traits> super;

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

    trie_generic(const bit_compare &bit_comp, const allocator_type &alloc)
        : super(bit_comp)
        , alloc_(alloc)
        , size_(0)
    {
    }

    trie_generic(const this_t &b)
        : super(b.bit_comp_)
        , alloc_(b.alloc_)
        , size_(0)
    {
        insert(b.begin(), b.end());
    }

    trie_generic(
        const value_type *first,
        const value_type *last,
        const bit_compare &bit_comp,
        const allocator_type &alloc)
        : super(bit_comp)
        , alloc_(alloc)
        , size_(0)
    {
        insert(first, last);
    }

    ~trie_generic()
    {
        // recursively release the memory for all nodes in tree
        del_tree(root_);
    }

    this_t &operator=(const this_t &b)
    {
        del_tree(root_);
        bit_comp_ = b.bit_comp_;
        alloc_ = b.alloc_;
        root_ = 0;
        size_ = 0;
        insert(b.begin(), b.end());
        return *this;
    }

    void swap(this_t &b)
    {
        std::swap(bit_comp_, b.bit_comp_);
        std::swap(alloc_, b.alloc_);
        std::swap(root_, b.root_);
        std::swap(size_, b.size_);
    }

    void clear()
    {
        del_tree(root_);
        root_ = 0;
    }

    size_type size() const
    {
        return size_;
    }

    size_type max_size() const
    {
        return alloc_.max_size();
    }

    allocator_type get_allocator() const
    {
        return alloc_;
    }

private:
    typedef std::pair<iterator, bool> iter_bool_pair;

public:
    iter_bool_pair insert(const value_type &val)
    {
        if (this->root_)
        {
            algorithm pal(this, this->root_, 0);
            // find a number of first mismatching bit
            const word_t l = pal.mismatch(this->bit_comp_, T::ref_key(val));
            // if this number end at infinity
            if (~word_t(0) == l)
                // then this key already in trie
                return iter_bool_pair(iterator(vertex(pal)), false);
            // add new node_type for value with unique key
            return iter_bool_pair(iterator(vertex(add(val, pal, l))), true);
        }
        // if trie is empty then add root
        return iter_bool_pair(iterator(vertex(add_root(val))), true);
    }

    // just for backward compatibility with std assoc containers
    iterator insert(iterator, const value_type &val)
    {
        return insert(val).first;
    }

    // template of insert range
    template <typename Iter>
    void insert(Iter first, Iter last)
    {
        for (; first != last; ++first)
            insert(*first);
    }

    struct void_merge_handler
    {
        void operator()(iterator, const_iterator)
        {
        }
    };

    // merge two identical tries (instances of bit_compare must be equal)
    template <typename Handler>
    void merge(const_iterator first, const_iterator last, Handler handler)
    {
        // ���� ������ �����, ���� �������� ���� �������
        if (!root_ && first != last)
        {
            insert(*first);
            ++first;
        }
        algorithm
            palCur(this, root_, 0),
            pal(static_cast<algorithm&>(static_cast<vertex&>(first)));
        const algorithm &palEnd(static_cast<algorithm&>(static_cast<vertex&>(last)));
        word_t skip = 0;
        while (pal != palEnd)
        {
            palCur.ascend_less(skip);
            const word_t l = palCur.mismatch(bit_comp_, pal.get_p()->get_key());
            if (~word_t(0) == l)
                // ����� ���� ��� ����, �� ���� ����� ��������� �������
                handler(iterator(vertex(palCur)), const_iterator(vertex(pal)));
            else
                add(pal.get_p()->get_value(), palCur, l);
            // move to the next
            if (pal.get_qid())
            {
                node_type *cur = pal.get_q();
                for (; cur->get_parent_id(); cur = cur->get_parent());
                pal.init(cur->get_parent(), 1);
            }
            else
                pal.toggle();
            skip = pal.get_q()->get_skip();
            pal.descend(0);
        }
    }

    void erase(iterator delIt)
    {
        // retrieve algorithm structure from iterator
        erase_node(static_cast<algorithm&>(static_cast<vertex&>(delIt)));
    }

    // erase all values with specified prefix
    size_type erase(const key_type &key, word_t prefixLen = ~word_t(0))
    {
        if (root_)
        {
            algorithm pal(this, root_, 0);
            if (pal.mismatch(bit_comp_, key, prefixLen) >= prefixLen)
            {
                const size_type pastSize = size();
                // if erase entire tree
                if (pal.get_q() == root_)
                {
                    clear();
                    return pastSize;
                }
                erase_subtree(pal);
                return pastSize - size();
            }
        }
        return 0;
    }

private:
    void del_tree(node_type *node)
    {
        if (node)
        {
            vertex
                vtx(algorithm(this, node, 0)),
                vtxEnd(algorithm(this, node, 1));
            for (vtx.postorder_init(); vtx != vtxEnd; vtx.postorder_incr())
            {
                if (!vtx.leaf())
                    del_node(static_cast<algorithm&>(vtx).get_p());
            }
        }
    }

    void del_node(node_type *node)
    {
        alloc_.destroy(node);
        alloc_.deallocate(node, 1);
        --size_;
    }

    void erase_subtree(algorithm pal)
    {
        // if erase subtree
        if (!pal.get_qtag())
        {
            // deallocate subtree
            del_tree(pal.get_p());
            // we must find the node_type with key whose prefix
            // we erase among nodes from pal.get_q() up to root_
            pal.get_q()->set_xlinktag(pal.get_qid(),
                pal.get_subtree_node(bit_comp_), 1);
        }
        erase_node(pal);
    }

    void erase_node(algorithm pal)
    {
        node_type *p = pal.erase();
        // special case: del root
        if (p == root_)
            root_ = pal.get_q() == p ? 0 : pal.get_q();
        // sic transit gloria mundi
        del_node(p);
    }

protected:
    // create root in empty trie
    algorithm add_root(const value_type &val)
    {
        root_ = alloc_.allocate(1);
        alloc_.construct(root_, node_type(val));
        root_->init_root();
        size_ = 1;
        return algorithm(this, root_, 0);
    }

    // add new node_type with unique key
    algorithm add(const value_type &val, algorithm &pal, word_t prefixLen)
    {
        // brave new node_type
        node_type *r = alloc_.allocate(1);
        alloc_.construct(r, node_type(val));
        // add new node_type into trie
        const word_t b = bit_comp_.get_bit(T::ref_key(val), prefixLen);
        pal.add(r, b, prefixLen);
        ++size_;
        return algorithm(this, r, b);
    }

    // data members
    typename allocator_type::template rebind<node_type>::other alloc_;
    size_type size_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
