/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_LCA_BASE_HPP
#define PATL_IMPL_LCA_BASE_HPP

#include "../config.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Oracle>
class lca_base
{
    typedef lca_base<Oracle> this_t;
    typedef typename Oracle::vertex vertex;

public:
    word_t number() const
    {
        return num_;
    }

    word_t height() const
    {
        return get_lowest_bit_id(num_);
    }

    bool is_inode() const
    {
        return il_ & word_t(1);
    }

    const this_t *get_i() const
    {
        return is_inode() ? this : reinterpret_cast<const this_t*>(il_);
    }

    this_t *get_i()
    {
        return is_inode() ? this : reinterpret_cast<this_t*>(il_);
    }

    const this_t *get_l_i() const
    {
        return is_inode()
            ? reinterpret_cast<const this_t*>(il_ & ~word_t(1))
            : reinterpret_cast<const this_t*>(
                reinterpret_cast<const this_t*>(il_)->il_ & ~word_t(1));
    }

    this_t *get_l_i()
    {
        return is_inode()
            ? reinterpret_cast<this_t*>(il_ & ~word_t(1))
            : reinterpret_cast<this_t*>(
                reinterpret_cast<this_t*>(il_)->il_ & ~word_t(1));
    }

    word_t get_a() const
    {
        return a_;
    }

    void numerate(word_t num)
    {
        num_ = num;
    }

    void set_inode()
    {
        il_ = reinterpret_cast<word_t>(this) | word_t(1);
    }

    void setup_i(Oracle *oracl)
    {
        vertex vtx(oracl->new_algo(this));
        vtx.iterate(0);
        this_t
            *ltI = oracl->get_by(vtx)->get_i(),
            *rtI = (oracl->get_by(vtx) + 1)->get_i();
        const word_t
            idH = height(),
            ltH = ltI->height(),
            rtH = rtI->height();
        if (idH > ltH && idH > rtH)
            set_inode();
        else if (ltH < rtH)
        {
            set_i(rtI);
            rtI->set_l(this);
        }
        else
        {
            set_i(ltI);
            ltI->set_l(this);
        }
    }

    void set_root_a()
    {
        a_ = word_t(1) << get_i()->height();
    }

    void setup_a(const Oracle *oracl)
    {
        vertex vtx(oracl->new_algo(this));
        vtx.ascend();
        a_ = oracl->get_by(vtx)->a_ |
            (word_t(1) << get_i()->height());
    }

private:
    void set_i(const this_t *pI)
    {
        il_ = reinterpret_cast<word_t>(pI);
    }

    void set_l(const this_t *pL)
    {
        il_ = reinterpret_cast<word_t>(pL) | word_t(1);
    }

    word_t num_, il_, a_; // il_ - pointer to I- or L-node
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
