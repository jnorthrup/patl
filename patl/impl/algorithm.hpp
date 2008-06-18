/*-
 | trie_map, trie_set, suffix_map, suffix_set template containers, v0.31
 | Based on PATRICIA trie
 | Key features: STL-compatible, high performance with non-scalar keys
 | Compile with: MSVC 2005, MSVC 2008, GCC 3.4.4 (?)
 |
 | (c) 2005, 2007..2008 by Roman S. Klyujkov, rsk_comp at mail.ru
 |
 | algorithm P class template
-*/

#ifndef PATL_IMPL_ALGORITHM_HPP
#define PATL_IMPL_ALGORITHM_HPP

#include "../config.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

#define SELF static_cast<this_t*>(this)
#define CSELF static_cast<const this_t*>(this)

/**
 * Base class of algorithms & iterator's state store
 * Algorithm P, described in Knuth's TAOCP, vol. 3, put in a basis
 */
template <typename T, typename This, typename Container>
class algorithm_generic
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
        , qid_(static_cast<unsigned char>(qid))
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
        , qid_(static_cast<unsigned char>(qq & word_t(1)))
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

    /// analogue to mismatch, for suffix_cont
    /// TODO rewrite as templated member functions
    template <bool Huge>
    class mismatch_suffix;

    bit_compare bit_comp()
    {
        return cont_->bit_comp();
    }

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
            // trie traverse to the leafs
            pal_.run(key);
            // compute skip as the number of first mismatching bit
            skip = align_down<bit_compare::bit_size>(skip) +
                bit_comp.bit_mismatch(
                    key + skip / bit_compare::bit_size,
                    pal_.get_key() +
                    skip / bit_compare::bit_size);
            // trie ascend to the inserting place
            pal_.ascend(skip);
            return skip; // new node mismatching bit number
        }

    private:
        this_t &pal_;
    };

    template <>
    class mismatch_suffix<true>
    {
        mismatch_suffix &operator=(const mismatch_suffix&);

    public:
        mismatch_suffix(this_t &pal)
            : pal_(pal)
        {
        }

        /// this code guarantee amortized linear time for suffix indexing
        word_t operator()(
            const key_type &key,
            /// number of bits in key that certainly match with available in trie
            word_t skip)
        {
            const bit_compare bit_comp = pal_.bit_comp();
            // until we achieved the leaf
            while (!pal_.get_qtag())
            {
                const word_t skip_cur = pal_.get_p()->get_skip();
                if (skip <= skip_cur)
                {
                    const key_type &exist_key = pal_.get_key();
                    for (; skip != skip_cur; ++skip)
                    {
                        // if we discover mismatch in bits - immediately return its number
                        if (bit_comp.get_bit(key, skip) != bit_comp.get_bit(exist_key, skip))
                            return skip;
                    }
                    ++skip;
                }
                pal_.iterate_by_key(key);
            }
            return align_down<bit_compare::bit_size>(skip) +
                bit_comp.bit_mismatch(
                    key + skip / bit_compare::bit_size,
                    pal_.get_key() +
                    skip / bit_compare::bit_size);
        }

    private:
        this_t &pal_;
    };

    /// function determine highest node back-referenced by current subtree; O(log n)
    const node_type *get_subtree_node() const
    {
        const node_type
            *cur = get_q(),
            *parent = cur->get_parent();
        word_t id = get_qid();
        while (
            parent &&
            id != cont_->bit_comp().get_bit(CSELF->get_key(cur), cur->get_skip()))
        {
            id = cur->get_parent_id();
            cur = parent;
            parent = cur->get_parent();
        }
        return cur;
    }

    /// add new node into trie at current position; O(1)
    void add(node_type *r, word_t b, word_t prefixLen)
    {
        // add new node into trie
        r->set_parentid(get_q(), get_qid());
        if (!get_qtag())
            get_p()->set_parentid(r, word_t(1) ^ b);
        r->set_xlinktag(b, r, 1);
        r->set_xlinktag(word_t(1) ^ b, get_p(), get_qtag());
        r->set_skip(prefixLen);
        get_q()->set_xlinktag(get_qid(), r, 0);
    }

    /**
     * erase current node from trie; O(1)
     * \return pointer to erased node for cleanup
     */
    node_type *erase()
    {
        // p-node back-referenced from q-node (q-tag == 1)
        // q- & p-nodes may be the same
        node_type
            *q = get_q(),
            *p = get_p();
        // p-brother is the sibling of p-node
        const word_t
            pBrotherId = word_t(1) ^ get_qid(),
            pBrotherTag = q->get_xtag(pBrotherId);
        node_type *pBrother = q->get_xlink(pBrotherId);
        // take the parent of q-node
        const word_t qParentId = q->get_parent_id();
        node_type *qParent = q->get_parent();
        // if q-parent exist i.e. q-node isn't the root
        if (qParent)
        {
            // replace q-node with p-brother
            qParent->set_xlinktag(qParentId, pBrother, pBrotherTag);
            if (!pBrotherTag)
                pBrother->set_parentid(qParent, qParentId);
        }
        // if p- & q-nodes is not the same
        if (q != p)
        {
            // replace p-node with q-node
            q->set_all_but_value(p);
            // correct p-parent, if one exist
            node_type *pParent = p->get_parent();
            if (pParent)
                pParent->set_xlinktag(p->get_parent_id(), q, 0);
            // correct p-node children
            if (!p->get_xtag(0))
                p->get_xlink(0)->set_parentid(q, 0);
            if (!p->get_xtag(1))
                p->get_xlink(1)->set_parentid(q, 1);
        }
        //
        return p;
    }

    /// find node with nearest key and return the number of first mismatching bit
    word_t mismatch(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        const bit_compare bit_comp(cont_->bit_comp());
        const word_t len = get_min(prefixLen, bit_comp.bit_length(key));
        if (len == ~word_t(0))
            run(key);
        else
            run(key, len);
        const word_t l = bit_comp.bit_mismatch(key, SELF->get_key());
        if (l != ~word_t(0))
            ascend(l);
        return l;
    }

    /// trie descend to the leaves
    void descend(word_t side)
    {
        while (!get_qtag())
            iterate(side);
    }

    /// trie descend to the leaves limited by prefix length
    void descend(word_t side, word_t prefixLen)
    {
        while (!get_qtag() && get_p()->get_skip() < prefixLen)
            iterate(side);
    }

    /// trie descend to the leaves driven by decision functor
    template <typename Decision>
    void descend_decision(word_t side, Decision &decis)
    {
        for (;;)
        {
            while (!get_qtag() && decis(get_p()->get_skip()))
                iterate(decis(get_p()->get_skip(), side)
                    ? side
                    : word_t(1) ^ side);
            if (!get_p())
                return;
            const word_t skip = decis(SELF->get_key());
            if (skip == ~word_t(0))
                return;
            ascend(skip);
            do move_generic(word_t(1) ^ side);
            while (
                get_q()->get_parent() &&
                //get_q()->get_skip() != ~word_t(0) &&
                !decis(get_q()->get_skip(), get_qid()));
            /*if (!get_q()->get_parent())
            return;*/
        }
    }

    /// generic part of all move functions
    void move_generic(word_t side)
    {
        if (get_qid() == side)
        {
            node_type *cur = get_q();
            for (; cur->get_parent_id() == side; cur = cur->get_parent()) ;
            init(cur->get_parent(), side);
        }
        else
            toggle();
    }

    /// move to the next node
    void move(word_t side)
    {
        move_generic(side);
        descend(word_t(1) ^ side);
    }

    /// move to the next node limited by prefix length
    void move(word_t side, word_t prefixLen)
    {
        move_generic(side);
        descend(word_t(1) ^ side, prefixLen);
    }

    /// move to the next node driven by decision functor
    template <typename Decision>
    void move_decision(word_t side, Decision &decis)
    {
        do move_generic(side);
        while (
            get_q()->get_parent() &&
            //get_q()->get_skip() != ~word_t(0) &&
            !decis(get_q()->get_skip(), get_qid()));
        descend_decision(word_t(1) ^ side, decis);
    }

    /// ascend to parent node
    void ascend()
    {
        node_type *q = get_q();
        init(q->get_parent(), q->get_parent_id());
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
        const bit_compare bit_comp(cont_->bit_comp());
        while (!get_qtag())
            iterate(bit_comp.get_bit(key, get_p()->get_skip()));
    }

    /// one run of the classical algorithm P limited by prefix length
    void run(const key_type &key, word_t prefixLen)
    {
        word_t skip;
        const bit_compare bit_comp(cont_->bit_comp());
        while (!get_qtag() && (skip = get_p()->get_skip()) < prefixLen)
            iterate(bit_comp.get_bit(key, skip));
    }

    /// one iteration of the classical algorithm P
    void iterate(word_t side)
    {
        init(get_p(), side);
    }

    /// one iteration of the classical algorithm P driven by given key
    void iterate_by_key(const key_type &key)
    {
        iterate(cont_->bit_comp().get_bit(key, get_p()->get_skip()));
    }

    /// init function
    void init(const node_type *q, word_t qid)
    {
#ifdef PATL_ALIGNHACK
        qq_ = qid | reinterpret_cast<word_t>(q);
#else
        q_ = const_cast<node_type*>(q);
        qid_ = static_cast<unsigned char>(qid);
#endif
    }

    /// return p-node
    const node_type *get_p() const
    {
        return get_q()->get_xlink(get_qid());
    }
    node_type *get_p()
    {
        return get_q()->get_xlink(get_qid());
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
