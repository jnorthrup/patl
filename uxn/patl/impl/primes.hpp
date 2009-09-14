/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PRIMES_HPP
#define PATL_IMPL_PRIMES_HPP

#include <memory>
#include <math.h>
#include "generator.hpp"
#include "trivial.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Primes>
struct gen_map
{
    typedef typename Primes::nat_type nat_type;

    gen_map(nat_type p0, nat_type p1)
        : lo_(p0 * p0)
        , hi_(p1 * p1)
        , bound_((hi_ - lo_) / 2 - 1)
        , arr_(new word_t [bound_ / bits_in_word + 1])
        , i_(~nat_type(0))
        , n_(lo_)
    {
        for (word_t i = 0
            ; i != bound_ / bits_in_word + 1
            ; ++i)
            arr_[i] = word_t(0);
        Primes ps(3);
        for (ps.next(); ps.value() != p1; ps.next())
        {
            const nat_type i = ps.value();
            for (nat_type j = correct(i - lo_ % i, i) / 2 - 1
                ; j < bound_
                ; j += i)
                arr_[j / bits_in_word] |= 1 << (j % bits_in_word);
        }
        next();
    }

    ~gen_map()
    { delete[] arr_; }

    bool avail() const
    { return i_ != bound_; }

    nat_type value() const
    { return n_; }

    void next()
    {
        do { ++i_; n_ += 2; }
        while (
            avail() &&
            (arr_[i_ / bits_in_word] & (1 << (i_ % bits_in_word))));
    }

private:
    static nat_type correct(nat_type n, nat_type i)
    { return n & 1 ? n + i : n; }

    const nat_type lo_, hi_, bound_;
    word_t *arr_;
    nat_type i_, n_;
};

template <typename Natural>
inline Natural sqrt_int(Natural x)
{ return static_cast<Natural>(sqrt(static_cast<double>(x))); }

template <typename Natural>
PATL_GENERATOR(primes, Natural, primes<Natural>)
{
public:
    typedef Natural nat_type;

    primes(nat_type n = 2)
        : n_(n)
        , sq_n_(sqrt_int(n))
    { }

    PATL_EMIT(nat_type)
    {
        if (n_ <= 2) PATL_YIELD(2);
        if (n_ <= 3) PATL_YIELD(3);
        if (n_ <= 5) PATL_YIELD(5);
        if (n_ <= 7) PATL_YIELD(7);
        ps_.reset(new primes(3));
        ps_->next();
        p1_ = ps_->value();
        for (;;)
        {
            p0_ = p1_;
            ps_->next();
            p1_ = ps_->value();
            if (p1_ <= sq_n_)
                continue;
            gmap_.reset(new gen_map<primes>(p0_, p1_));
            for (; gmap_->avail(); gmap_->next())
                if (gmap_->value() >= n_)
                    PATL_YIELD(gmap_->value());
        }
    }
    PATL_STOP_EMIT()

private:
    const nat_type n_, sq_n_;
    std::auto_ptr<gen_map<primes> > gmap_;
    nat_type p0_, p1_;
    std::auto_ptr<primes> ps_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
