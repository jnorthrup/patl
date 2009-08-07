/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_LCA_ORACLE_HPP
#define PATL_LCA_ORACLE_HPP

#include "impl/lca_base.hpp"

namespace uxn
{
namespace patl
{

template <typename SuffixCont>
class lca_oracle
{
    typedef SuffixCont suffix_cont;
    typedef typename suffix_cont::allocator_type allocator_type;
    typedef impl::lca_base<lca_oracle<suffix_cont> > lca_type;

public:
    typedef typename suffix_cont::vertex vertex;
    typedef typename suffix_cont::prefix prefix;

    /// utility func
    const lca_type *get_by(const vertex &vtx) const
    {
        return lca_ + cont_->vertex_index_of(vtx);
    }

    /// utility func
    lca_type *get_by(const vertex &vtx)
    {
        return lca_ + cont_->vertex_index_of(vtx);
    }

    /// utility func
    vertex new_algo(const lca_type *cur) const
    {
        const word_t id = cur - lca_;
        return cont_->vertex_by(id / 2, id & word_t(1));
    }

    /// ctor
    lca_oracle(const suffix_cont &cont)
        : cont_(&cont)
        , lca_alloc_(allocator_type())
        , lca_(0)
        , unsigned_alloc_(allocator_type())
        , lca_map_(0)
    {
        init();
    }

    ~lca_oracle()
    {
        lca_alloc_.deallocate(lca_, lca_size_);
        unsigned_alloc_.deallocate(lca_map_, lca_size_ + 1);
    }

    void init()
    {
        // deallocate old
        lca_alloc_.deallocate(lca_, lca_size_);
        lca_ = 0;
        unsigned_alloc_.deallocate(lca_map_, lca_size_ + 1);
        lca_map_ = 0;
        //
        if (cont_->size() == 0)
            return;
        lca_ = lca_alloc_.allocate(lca_size_ = 2 * cont_->size());
        memset(lca_, 0, lca_size_ * sizeof(lca_type));
        lca_map_ = unsigned_alloc_.allocate(lca_size_ + 1);
        // numerate, set I- & L-nodes
        word_t num = 0;
        vertex vtx(cont_->root());
        const vertex vtxEnd(vtx.sibling());
        while (vtx != vtxEnd)
        {
            // descend to the leaves
            for (; !vtx.get_qtag(); vtx.iterate(0))
                get_by(vtx)->numerate(++num);
            //
            lca_type *cur = get_by(vtx);
            cur->numerate(++num);
            cur->set_inode();
            // move to the next
            if (vtx.get_qid())
            {
                while (vtx.get_parent_id())
                {
                    vtx.ascend();
                    get_by(vtx)->setup_i(this);
                }
                //
                vtx.ascend();
                get_by(vtx)->setup_i(this);
            }
            vtx.toggle();
        }
        // num - last numerator
        lca_root_h_ = impl::get_highest_bit_id(num);
        // set A bits
        vtx = cont_->root();
        lca_type *cur = get_by(vtx);
        cur->set_root_a();
        if (!vtx.get_qtag())
            vtx.iterate(0);
        for (; vtx != vtxEnd; vtx.template move_subtree<1>())
        {
            // descend to the leaves
            for (; !vtx.get_qtag(); vtx.iterate(0))
                get_by(vtx)->setup_a(this);
            //
            get_by(vtx)->setup_a(this);
        }
        // generate map: number -> index
        for (word_t i = 0; i != lca_size_; ++i)
            lca_map_[lca_[i].number()] = i;
    }

    /// return LCA(vx, vy)
    vertex operator()(vertex vx, vertex vy) const
    {
        const lca_type
            *xPrime = get_by(vx),
            *xI = xPrime->get_i(),
            *yPrime = get_by(vy),
            *yI = yPrime->get_i();
        const word_t
            b = impl::get_binary_lca(lca_root_h_, xI->number(), yI->number()),
            i = impl::get_lowest_bit_id(b),
            xA = xPrime->get_a(),
            yA = yPrime->get_a(),
            j = impl::get_lowest_bit_id((xA & yA) >> i << i);
        //
        vertex
            xCap = new_algo(xPrime),
            yCap = new_algo(yPrime);
        if (impl::get_lowest_bit_id(xA) < j)
        {
            const word_t
                k = impl::get_highest_bit_id(xA & (word_t(1) << j) - 1),
                wINum = (xI->number() >> k | word_t(1)) << k;
            const lca_type
                *wI = lca_ + lca_map_[wINum],
                *wLI = wI->get_l_i();
            xCap = new_algo(wLI);
            xCap.ascend();
        }
        if (impl::get_lowest_bit_id(yA) < j)
        {
            const word_t
                k = impl::get_highest_bit_id(yA & (word_t(1) << j) - 1),
                wINum = (yI->number() >> k | word_t(1)) << k;
            const lca_type
                *wI = lca_ + lca_map_[wINum],
                *wLI = wI->get_l_i();
            yCap = new_algo(wLI);
            yCap.ascend();
        }
        return get_by(xCap)->number() < get_by(yCap)->number() ? xCap : yCap;
    }

private:
    const suffix_cont *cont_;
    typename allocator_type::template rebind<lca_type>::other lca_alloc_;
    lca_type *lca_;
    word_t lca_size_, lca_root_h_;
    word_t *lca_map_;
    typename allocator_type::template rebind<word_t>::other unsigned_alloc_;
};

} // namespace patl
} // namespace uxn

#endif
