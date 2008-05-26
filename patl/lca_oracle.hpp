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
    typedef typename suffix_cont::algorithm algorithm;
    typedef typename suffix_cont::vertex vertex;
    typedef impl::lca_base<lca_oracle<suffix_cont> > lca_type;

    friend class lca_type;

    const lca_type *get_by(const algorithm &pal) const
    {
        return lca_ + (cont_->trie_.index_of(pal.get_q()) * 2 | pal.get_qid());
    }

    lca_type *get_by(const algorithm &pal)
    {
        return lca_ + (cont_->trie_.index_of(pal.get_q()) * 2 | pal.get_qid());
    }

    algorithm new_algo(const lca_type *cur) const
    {
        const word_t id = cur - lca_;
        return algorithm(
            cont_,
            cont_->trie_[id / 2],
            id & word_t(1));
    }

public:
    lca_oracle(const suffix_cont &cont)
        : cont_(&cont)
        , lca_alloc_(allocator_type())
        , lca_(0)
        , unsigned_alloc_(allocator_type())
    {
        init();
    }

    void init()
    {
        if (lca_)
        {
            lca_alloc_.deallocate(lca_, lca_size_);
            lca_ = 0;
            unsigned_alloc_.deallocate(lca_map_, lca_size_ + 1);
        }
        if (!cont_->root_)
            return;
        lca_ = lca_alloc_.allocate(lca_size_ = 2 * cont_->size());
        memset(lca_, 0, lca_size_ * sizeof(lca_type));
        lca_map_ = unsigned_alloc_.allocate(lca_size_ + 1);
        // numerate, set I- & L-nodes
        word_t num = 0;
        algorithm pal(cont_, cont_->root_, 0);
        const algorithm palEnd(cont_, cont_->root_, 1);
        while (pal != palEnd)
        {
            // descend to the leaves
            for (; !pal.get_qtag(); pal.iterate(0))
                get_by(pal)->numerate(++num);
            //
            lca_type *cur = get_by(pal);
            cur->numerate(++num);
            cur->set_inode();
            // move to the next
            if (pal.get_qid())
            {
                while (pal.get_q()->get_parent_id())
                {
                    pal.ascend();
                    get_by(pal)->setup_i(this);
                }
                //
                pal.init(pal.get_q()->get_parent(), 0);
                get_by(pal)->setup_i(this);
                pal.toggle();
            }
            else
                pal.toggle();
        }
        // num - last numerator
        lca_root_h_ = impl::get_highest_bit_id(num);
        // set A bits
        pal.init(cont_->root_, 0);
        lca_type *cur = get_by(pal);
        cur->set_root_a();
        if (!pal.get_qtag())
            pal.iterate(0);
        for (; pal != palEnd; pal.move_generic(1))
        {
            // descend to the leaves
            for (; !pal.get_qtag(); pal.iterate(0))
                get_by(pal)->setup_a(this);
            //
            get_by(pal)->setup_a(this);
        }
        // generate map: number -> index
        for (word_t i = 0; i != lca_size_; ++i)
            lca_map_[lca_[i].number()] = i;
    }

    vertex operator()(vertex vx, vertex vy) const
    {
        const algorithm x(static_cast<algorithm>(vx)), y(static_cast<algorithm>(vy));
        const lca_type
            *xPrime = get_by(x),
            *xI = xPrime->get_i(),
            *yPrime = get_by(y),
            *yI = yPrime->get_i();
        const word_t
            b = impl::get_binary_lca(lca_root_h_, xI->number(), yI->number()),
            i = impl::get_lowest_bit_id(b),
            xA = xPrime->get_a(),
            yA = yPrime->get_a(),
            j = impl::get_lowest_bit_id((xA & yA) >> i << i);
        //
        algorithm
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
        return vertex(
            get_by(xCap)->number() < get_by(yCap)->number() ? xCap : yCap);
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
