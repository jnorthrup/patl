/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | trie_set, trie_map, suffix_set, suffix_map template containers, v0.x
 | Based on PATRICIA trie
 | Key features: STL-compatible, high performance with non-scalar keys
 | Compile with: MSVC 2003/2005/2008/2010, G++ 3.x
 |
 | algorithm P class template
-*/

#ifndef PATL_IMPL_ALGORITHM_HPP
#define PATL_IMPL_ALGORITHM_HPP

#include "../config.hpp"
#include "trivial.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

#define SELF static_cast<this_t*>(this)
#define CSELF static_cast<const this_t*>(this)

template <typename T, typename This, typename Container, word_t N = 0>
class algorithm_generic
    : public algorithm_generic<T, This, Container, 0>
{
    typedef This this_t;
    typedef algorithm_generic<T, This, Container, 0> super;

public:
    typedef Container cont_type;
    typedef typename T::key_type key_type;
    typedef typename T::value_type value_type;
    typedef typename T::node_type node_type;

    /// zero-init default ctor
    explicit algorithm_generic(const cont_type *cont = 0)
        : super(cont)
    {
    }

    /// simple init ctor
    algorithm_generic(const cont_type *cont, const node_type *q, word_t qid)
        : super(cont, q, qid)
    {
    }

    /// compact init ctor
    algorithm_generic(const cont_type *cont, word_t qq)
        : super(cont, qq)
    {
    }

    /// one run of the classical algorithm P
    void run(const key_type &key)
    {
        while (!CSELF->get_qtag())
        {
            // TODO
            /*if ((get_p()->get_skip() >> N) != (get_q()->get_skip() >> N) &&
                cont_->)
            else*/
            iterate_by_key(key);
        }
    }

    /// one run of the classical algorithm P limited by prefix length
    void run(const key_type &key, word_t prefixLen)
    {
        while (!CSELF->get_qtag() && CSELF->get_p()->get_skip() < prefixLen)
            iterate_by_key(key);
    }
};

/**
 * Base class of algorithms & iterator's state store
 * Algorithm P, described in Knuth's TAOCP, vol. 3, put in a basis
 */
template <typename T, typename This, typename Container>
class algorithm_generic<T, This, Container, 0>
{
    typedef This this_t;
    typedef typename T::bit_compare bit_compare;
    typedef typename T::allocator_type allocator_type;

public:
    typedef Container cont_type;
    typedef typename T::key_type key_type;
    typedef typename T::value_type value_type;
    typedef typename T::node_type node_type;

private:
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef word_t size_type;

public:
    /// zero-init default ctor
    explicit algorithm_generic(const cont_type *cont = 0)
        : cont_(cont)
#ifdef PATL_ALIGNHACK
        , qq_(0)
#else
        , q_(0)
        , qid_(0)
#endif
    {
    }

    /// simple init ctor
    algorithm_generic(const cont_type *cont, const node_type *q, word_t qid)
        : cont_(cont)
#ifdef PATL_ALIGNHACK
        , qq_(qid | reinterpret_cast<word_t>(q))
#else
        , q_(const_cast<node_type*>(q))
        , qid_(qid)
#endif
    {
    }

    /// compact init ctor
    algorithm_generic(const cont_type *cont, word_t qq)
        : cont_(cont)
#ifdef PATL_ALIGNHACK
        , qq_(qq)
#else
        , q_(reinterpret_cast<node_type*>(qq & ~word_t(1)))
        , qid_(qq & word_t(1))
#endif
    {
    }

    /// return compact representation of algorithm state
    word_t compact() const
    {
        return
#ifdef PATL_ALIGNHACK
            qq_;
#else
            qid_ | reinterpret_cast<word_t>(q_);
#endif
    }

    /// equality op
    bool operator==(const this_t &right) const
    {
        return
#ifdef PATL_ALIGNHACK
            qq_ == right.qq_;
#else
            q_ == right.q_ && qid_ == right.qid_;
#endif
    }

    bool operator!=(const this_t &pal) const
    {
        return !(*this == pal);
    }

    /// return sibling of algorithm
    this_t sibling() const
    {
        return CSELF->construct(get_q(), word_t(1) ^ get_qid());
    }

    /// toggle index i.e. convert algorithm to its sibling
    void toggle()
    {
#ifdef PATL_ALIGNHACK
        qq_ ^= word_t(1);
#else
        qid_ ^= word_t(1);
#endif
    }

    /// return real prefix length in bits
    /// NOTE maybe it must be in node_generic
    word_t get_prefix_length() const
    {
        return get_qtag() ? ~word_t(0) : get_p()->get_skip();
    }

    bit_compare bit_comp() const
    {
        return cont_->bit_comp();
    }

    /// this code guarantee amortized linear time for suffix indexing
    word_t mismatch_suffix(
        const key_type key,
        /// number of bits in key that certainly match with one exists in trie
        word_t skip)
    {
        const bit_compare bcmp = bit_comp();
        // until we achieved the leaf
        while (!get_qtag())
        {
            const word_t skip_cur = get_p()->get_skip();
            if (skip <= skip_cur)
            {
                const key_type exist_key = CSELF->get_key();
                for (; skip != skip_cur; ++skip)
                {
                    // if we discover mismatch in bits - immediately return its number
                    if (bcmp.get_bit(key, skip) != bcmp.get_bit(exist_key, skip))
                        return skip;
                }
                ++skip;
            }
            iterate_by_key(key);
        }
        return align_down<bit_compare::bit_size>(skip) +
            bcmp.bit_mismatch(
                key + skip / bit_compare::bit_size,
                CSELF->get_key() + skip / bit_compare::bit_size);
    }

#if 0 // obsolete
    /// analogue to mismatch, for suffix_cont
    template <bool Huge>
    class mismatch_suffix;

    /// classical algorithm P for search place for insert new node
    template <>
    class mismatch_suffix<false>
    {
        mismatch_suffix &operator=(const mismatch_suffix&);

    public:
        mismatch_suffix(this_t &pal)
            : pal_(pal)
        {
        }

        /**
         * this code faster than one in mismatch_suffix<true>
         * may used with small suffix_conts
         */ 
        word_t operator()(
            const key_type &key,
            /// number of bits in key that certainly match with available in trie
            word_t skip)
        {
            const bit_compare bit_comp = pal_.bit_comp();
            // trie traverse to the leaves
            pal_.run(key);
            // compute skip as the number of first mismatching bit
            skip = align_down<bit_compare::bit_size>(skip) +
                bit_comp.bit_mismatch(
                    key + skip / bit_compare::bit_size,
                    pal_.get_key() + skip / bit_compare::bit_size);
            // trie ascend to the inserting place
            pal_.ascend(skip);
            return skip; // new node mismatching bit number
        }

    private:
        this_t &pal_;
    };
#endif

    /// function determine highest node back-referenced from current subtree; O(log n)
    const node_type *get_subtree_node() const
    {
        const node_type
            *cur = get_q(),
            *parent = CSELF->get_parent(cur);
        word_t id = get_qid();
        while (
            parent &&
            id != bit_comp().get_bit(CSELF->get_key(cur), cur->get_skip()))
        {
            id = cur->get_parent_id();
            cur = parent;
            parent = CSELF->get_parent(cur);
        }
        return cur;
    }

    /// add new node into trie at current position; O(1)
    void add(node_type *r, word_t b, word_t prefixLen)
    {
        // add new node into trie
        CSELF->set_parentid(r, get_q(), get_qid());
        if (!get_qtag())
            CSELF->set_parentid(get_p(), r, word_t(1) ^ b);
        CSELF->set_xlinktag(r, b, r, 1);
        CSELF->set_xlinktag(r, word_t(1) ^ b, get_p(), get_qtag());
        r->set_skip(prefixLen);
        CSELF->set_xlinktag(get_q(), get_qid(), r, 0);
    }

    /**
     * erase current node from trie; O(1)
     * \return pointer to erased node for cleanup
     */
    node_type *erase()
    {
        // p-node back-referenced from q-node (q-tag == 1)
        // q- & p-node may be the same
        node_type
            *q = get_q(),
            *p = get_p();
        // p-brother is the sibling of p-node
        const word_t
            p_brother_id = word_t(1) ^ get_qid(),
            p_brother_tag = q->get_xtag(p_brother_id);
        node_type *p_brother = CSELF->get_xlink(q, p_brother_id);
        // take the parent of q-node
        const word_t q_parent_id = q->get_parent_id();
        node_type *q_parent = CSELF->get_parent(q);
        // if q-parent exist i.e. q-node isn't the root
        if (q_parent)
        {
            // replace q-node with p-brother
            CSELF->set_xlinktag(q_parent, q_parent_id, p_brother, p_brother_tag);
            if (!p_brother_tag)
                CSELF->set_parentid(p_brother, q_parent, q_parent_id);
        }
        // if p- & q-nodes is not the same
        if (q != p)
        {
            // replace p-node with q-node
            q->set_all_but_value(p);
            // correct p-parent, if one exist
            node_type *p_parent = CSELF->get_parent(p);
            if (p_parent)
                CSELF->set_xlinktag(p_parent, p->get_parent_id(), q, 0);
            // correct p-node children
            if (!p->get_xtag(0))
                CSELF->set_parentid(CSELF->get_xlink(p, 0), q, 0);
            if (!p->get_xtag(1))
                CSELF->set_parentid(CSELF->get_xlink(p, 1), q, 1);
        }
        //
        return p;
    }

    /// find node with nearest key and return the number of first mismatching bit
    word_t mismatch(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        const bit_compare bcmp(bit_comp());
        const word_t len = get_min(prefixLen, bcmp.bit_length(key));
        if (len == ~word_t(0))
            run(key);
        else
            run(key, len);
        const word_t l = bcmp.bit_mismatch(key, SELF->get_key());
        if (l != ~word_t(0))
            ascend(l);
        return l;
    }

    /// ascend to parent node
    void ascend()
    {
        node_type *q = get_q();
        init(CSELF->get_parent(q), q->get_parent_id());
    }

    /// trie ascend to the lowest node whose prefix less or equal than prefix length
    void ascend(sword_t prefixLen)
    {
        for (; static_cast<sword_t>(get_q()->get_skip()) > prefixLen; ascend()) ;
    }

    /// trie ascend to the lowest node whose prefix less than prefix length
    void ascend_less(sword_t prefixLen)
    {
        for (; static_cast<sword_t>(get_q()->get_skip()) >= prefixLen; ascend()) ;
    }

    /// one run of the classical algorithm P
    void run(const key_type &key)
    {
        while (!get_qtag())
            iterate_by_key(key);
    }

    /// one run of the classical algorithm P limited by prefix length
    void run(const key_type &key, word_t prefixLen)
    {
        while (!get_qtag() && get_p()->get_skip() < prefixLen)
            iterate_by_key(key);
    }

    /// one iteration of the classical algorithm P
    void iterate(word_t side)
    {
        init(get_p(), side);
    }

    /// one iteration of the classical algorithm P driven by given key
    void iterate_by_key(const key_type &key)
    {
        iterate(bit_comp().get_bit(key, get_p()->get_skip()));
    }

    /// init function
    void init(const node_type *q, word_t qid)
    {
#ifdef PATL_ALIGNHACK
        qq_ = qid | reinterpret_cast<word_t>(q);
#else
        q_ = const_cast<node_type*>(q);
        qid_ = qid;
#endif
    }

    /// compact init function
    void init(word_t qq)
    {
#ifdef PATL_ALIGNHACK
        qq_ = qq;
#else
        q_ = reinterpret_cast<node_type*>(qq & ~word_t(1));
        qid_ = qq & word_t(1);
#endif
    }

    /// return p-node
    const node_type *get_p() const
    {
        return CSELF->get_xlink(get_q(), get_qid());
    }
    node_type *get_p()
    {
        return CSELF->get_xlink(get_q(), get_qid());
    }

    /// return q-tag
    word_t get_qtag() const
    {
        return get_q()->get_xtag(get_qid());
    }

    /// return q-node
    const node_type *get_q() const
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<node_type*>(qq_ & ~word_t(1));
#else
        return q_;
#endif
    }
    node_type *get_q()
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<node_type*>(qq_ & ~word_t(1));
#else
        return q_;
#endif
    }

    /// return distiction bit
    word_t get_qid() const
    {
#ifdef PATL_ALIGNHACK
        return qq_ & word_t(1);
#else
        return qid_;
#endif
    }

    bool the_end() const
    {
        const node_type *q = get_q();
        return get_qid() && !(q && CSELF->get_parent(q));
    }

    const cont_type *cont() const
    {
        return cont_;
    }

protected:
    const cont_type *cont_;

private:
#ifdef PATL_ALIGNHACK
    /// compact representation of algorithm state with distinction bit in lsb
    word_t qq_;
#else
    /// q-node
    node_type *q_;
    /// distinction bit
    word_t qid_;
#endif
};

#undef SELF
#undef CSELF

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
