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

/// ������� ����� ���������� � ��������� ��������� ����������
/**
    � ������ ����� ������������� ��������� P, ���������� �
    �. ����, "��������� ����������������", ��� 3.
*/
template <typename T, typename This>
class algorithm_generic
{
    typedef This this_t;
    typedef typename T::bit_compare bit_compare;
    typedef typename T::allocator_type allocator_type;

public:
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
    /// ����������� �� ��������� ��� ������������� ������
    /// (� ���������� �� ���������)
    algorithm_generic()
#ifdef PATL_ALIGNHACK
        : qq_(0)
#else
        : q_(0)
        , qid_(0)
#endif
    {
    }
    /// ����������� ��� ���������������� ������������� ����� \c Q � �������� \c Qid
    /// (� ���������� �� ���������)
    algorithm_generic(const node_type *q, word_t qid)
#ifdef PATL_ALIGNHACK
        : qq_(qid | reinterpret_cast<word_t>(q))
#else
        : q_(const_cast<node_type*>(q))
        , qid_(static_cast<unsigned char>(qid))
#endif
    {
    }
    /// ����������� ���������� ������������� ���������
    algorithm_generic(word_t qq)
#ifdef PATL_ALIGNHACK
        : qq_(qq)
#else
        : q_(reinterpret_cast<node_type*>(qq & ~word_t(1)))
        , qid_(static_cast<unsigned char>(qq & word_t(1)))
#endif
    {
    }

    word_t compact() const
    {
        return
#ifdef PATL_ALIGNHACK
            qq_;
#else
            qid_ | reinterpret_cast<word_t>(q_);
#endif
    }

    /// ��������� ���� �������� ���������
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

	this_t sibling() const
	{
		return CSELF->construct(get_q(), word_t(1) ^ get_qid());
	}

    /// ������������� ������
    void toggle()
    {
#ifdef PATL_ALIGNHACK
        qq_ ^= word_t(1);
#else
        qid_ ^= word_t(1);
#endif
    }

    word_t get_prefix_length() const
    {
        return get_qtag() ? ~word_t(0) : get_p()->get_skip();
    }

    template <bool Huge>
    class mismatch_suffix;

    template <>
    class mismatch_suffix<false>
    {
        mismatch_suffix &operator=(const mismatch_suffix&);

    public:
        mismatch_suffix(
            algorithm_generic &pal,
            const bit_compare &bit_comp)
            : pal_(pal)
            , bit_comp_(bit_comp)
        {
        }

        word_t operator()(
            const key_type &key,
            word_t skip)
        {
            pal_.run(bit_comp_, key);
            skip = align_down<bit_compare::bit_size>(skip) +
                bit_comp_.bit_mismatch(
                    key + skip / bit_compare::bit_size,
                    static_cast<this_t*>(&pal_)->get_key() +
                    skip / bit_compare::bit_size);
            pal_.ascend(skip);
            return skip;
        }

    private:
        algorithm_generic &pal_;
        const bit_compare &bit_comp_;
    };

    template <>
    class mismatch_suffix<true>
    {
        mismatch_suffix &operator=(const mismatch_suffix&);

    public:
        mismatch_suffix(
            algorithm_generic &pal,
            const bit_compare &bit_comp)
            : pal_(pal)
            , bit_comp_(bit_comp)
        {
        }

        word_t operator()(
            const key_type &key,
            word_t skip)
        {
            while (!pal_.get_qtag())
            {
                const word_t skip_cur = pal_.get_p()->get_skip();
                if (skip <= skip_cur)
                {
                    const key_type &exist_key = static_cast<this_t*>(&pal_)->get_key();
                    for (; skip != skip_cur; ++skip)
                    {
                        if (bit_comp_.get_bit(key, skip) != bit_comp_.get_bit(exist_key, skip))
                            return skip;
                    }
                    ++skip;
                }
                pal_.iterate_by_key(bit_comp_, key);
            }
            return align_down<bit_compare::bit_size>(skip) +
                bit_comp_.bit_mismatch(
                    key + skip / bit_compare::bit_size,
                    static_cast<this_t*>(&pal_)->get_key() +
                    skip / bit_compare::bit_size);
        }

    private:
        algorithm_generic &pal_;
        const bit_compare &bit_comp_;
    };

    /// ����������� ���������� � ����� ����, �� ������� ����
    /// �������� ������ �� ���������, ������������ ����������; O(log N)
    const node_type *get_subtree_node(
        const bit_compare &bit_comp) const
    {
        const node_type
            *cur = get_q(),
            *parent = cur->get_parent();
        word_t id = get_qid();
        while (
            parent &&
            id != bit_comp.get_bit(CSELF->get_key(cur), cur->get_skip()))
        {
            id = cur->get_parent_id();
            cur = parent;
            parent = cur->get_parent();
        }
        return cur;
    }

    /// ��������� ����� ���� \c R � ������
    void add(node_type *r, word_t b, word_t prefixLen)
    {
        // add new node_type into trie
        r->set_parentid(get_q(), get_qid());
        if (!get_qtag())
            get_p()->set_parentid(r, word_t(1) ^ b);
        r->set_xlinktag(b, r, 1);
        r->set_xlinktag(word_t(1) ^ b, get_p(), get_qtag());
        r->set_skip(prefixLen);
        get_q()->set_xlinktag(get_qid(), r, 0);
    }

    /// ������� ���� \c P �� ������
    /// \return �������� ���� (��� ������������ ���������� �� ������)
    node_type *erase()
    {
        // �� ���� Q �� ���� P ���� �������� ����� (Qtag==1)
        // ���� Q � P ����� ������������ ���� � ��� �� ����
        node_type
            *q = get_q(),
            *p = get_p();
        // ���� pBrother - ���� ���� P
        const word_t
            pBrotherId = word_t(1) ^ get_qid(),
            pBrotherTag = q->get_xtag(pBrotherId);
        node_type *pBrother = q->get_xlink(pBrotherId);
        // ������ �������� ���� Q
        const word_t qParentId = q->get_parent_id();
        node_type *qParent = q->get_parent();
        // ���� �������� ���� Q ���������� (�.�. Q - �� ������)
        if (qParent)
        {
            // ������� ���� Q �� ���� pBrother
            qParent->set_xlinktag(qParentId, pBrother, pBrotherTag);
            if (!pBrotherTag)
                pBrother->set_parentid(qParent, qParentId);
        }
        // ���� ���� P � Q ������������ ������ ����
        if (q != p)
        {
            // ������� ���� P ����� Q
            q->set_all_but_value(p);
            // ������������� �������� ���� P, ���� �� ����������
            node_type *pParent = p->get_parent();
            if (pParent)
                pParent->set_xlinktag(p->get_parent_id(), q, 0);
            // ������������� ����� ���� P
            if (!p->get_xtag(0))
                p->get_xlink(0)->set_parentid(q, 0);
            if (!p->get_xtag(1))
                p->get_xlink(1)->set_parentid(q, 1);
        }
        // ����� ��������� ����
        return p;
    }

    // find node with nearest key and
    // return the number of first mismatching bit
    unsigned mismatch(
        const bit_compare &bit_comp,
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        const word_t len = get_min(prefixLen, bit_comp.bit_length(key));
        if (len == ~word_t(0))
            run(bit_comp, key);
        else
            run(bit_comp, key, len);
        const word_t l = bit_comp.bit_mismatch(key, SELF->get_key());
        if (l != ~word_t(0))
            ascend(l);
        return l;
    }

    /// ����� �� ������ � ��������� \c side �������
    void descend(word_t side)
    {
        while (!get_qtag())
            iterate(side);
    }

    /// ����� �� ������, ������������ ������ ��������
    void descend(word_t side, word_t prefixLen)
    {
        while (!get_qtag() && get_p()->get_skip() < prefixLen)
            iterate(side);
    }

    /// ����� �� ������, ����������� ��������� \c decis
    /// ������� ������ ���� � ������, ��������������� �������� \c Decision
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
            const unsigned skip = decis(SELF->get_key());
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

    /// ����� ����� ��� ���� ������� ���� \c move<side>
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

    /// ����������� � ����������� ��� ���������� ����, � ����������� ��
    /// \c side (����� ������ � �������)
    void move(word_t side)
    {
        move_generic(side);
        descend(word_t(1) ^ side);
    }

    /// ����������� � ����, ������������ ������ ��������
    void move(word_t side, word_t prefixLen)
    {
        move_generic(side);
        descend(word_t(1) ^ side, prefixLen);
    }

    /// ����������� � ����, ����������� ��������� \c decis
    /// ������� ��������� ���� � ������, ��������������� �������� \c Decision
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

    void ascend()
    {
        node_type *q = get_q();
        init(q->get_parent(), q->get_parent_id());
    }

    /// ������ �� ������ � ������� ����, \c skip �������� ������ ��� �����
    /// �������� ����� ��������
    void ascend(sword_t prefixLen)
    {
        for (; static_cast<sword_t>(get_q()->get_skip()) > prefixLen; ascend()) ;
    }

    /// ������ �� ������ � ������� ����, \c skip �������� ������ ������
    /// �������� ����� ��������
    void ascend_less(sword_t prefixLen)
    {
        for (; static_cast<sword_t>(get_q()->get_skip()) >= prefixLen; ascend()) ;
    }

    /// ������ ������������� ��������� P
    void run(
        const bit_compare &bit_comp,
        const key_type &key)
    {
        while (!get_qtag())
            iterate(bit_comp.get_bit(key, get_p()->get_skip()));
    }

    /// ������ ������������� ��������� P, ������������� ������ ��������
    void run(
        const bit_compare &bit_comp,
        const key_type &key,
        word_t prefixLen)
    {
        word_t skip;
        while (!get_qtag() && (skip = get_p()->get_skip()) < prefixLen)
            iterate(bit_comp.get_bit(key, skip));
    }

    /// ���� �������� ������������� ��������� P
    void iterate(word_t id)
    {
        init(get_p(), id);
    }

    void iterate_by_key(
        const bit_compare &bit_comp,
        const key_type &key)
    {
        iterate(bit_comp.get_bit(key, get_p()->get_skip()));
    }

    /// ���������������� ������������� ��������� ����� \c Q � �������� \c Qid
    void init(const node_type *q, word_t qid)
    {
#ifdef PATL_ALIGNHACK
        qq_ = qid | reinterpret_cast<word_t>(q);
#else
        q_ = const_cast<node_type*>(q);
        qid_ = static_cast<unsigned char>(qid);
#endif
    }

    /// ��������� ���� \c P
    const node_type *get_p() const
    {
        return get_q()->get_xlink(get_qid());
    }
    /// ��������� ���� \c P
    node_type *get_p()
    {
        return get_q()->get_xlink(get_qid());
    }

    /// ��������� ���, ��������������� ����� ���� \c Q � ���� \c P
    word_t get_qtag() const
    {
        return get_q()->get_xtag(get_qid());
    }

    /// ���������� ���� \c Q
    const node_type *get_q() const
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<node_type*>(qq_ & ~word_t(1));
#else
        return q_;
#endif
    }
    /// ���������� ���� \c Q
    node_type *get_q()
    {
#ifdef PATL_ALIGNHACK
        return reinterpret_cast<node_type*>(qq_ & ~word_t(1));
#else
        return q_;
#endif
    }

    /// ���������� ������ \c Qid
    word_t get_qid() const
    {
#ifdef PATL_ALIGNHACK
        return qq_ & word_t(1);
#else
        return qid_;
#endif
    }

private:
#ifdef PATL_ALIGNHACK
    /// ����������� ���� \c Q � �������� \c Qid � ������� ����
    word_t qq_;
#else
    node_type *q_; ///< ���� \c Q � ��������� P
    /// ����������, ����� �������� �������� ���� \c P
    /// �� ��������� � ���� \c Q: ����� ��� ������
    unsigned char qid_;
#endif
};

#undef SELF
#undef CSELF

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
