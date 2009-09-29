/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PRIMES_HPP
#define PATL_IMPL_PRIMES_HPP

#include <memory>
#include <cmath>
#include "generator.hpp"
#include "trivial.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Natural>
inline Natural sqrt_int(Natural x)
{ return static_cast<Natural>(sqrt(static_cast<double>(x))); }

template <typename Natural>
PATL_GENERATOR(primes, Natural, primes<Natural>)
{
    typedef primes<Natural> this_t;

public:
    typedef Natural nat_type;

    enum { B = 12 << 10 }; // to avoid g++ errors about variably modified type

    primes(const nat_type &n = 2, const nat_type &m = ~nat_type(0))
        : n_(n)
        , m_(patl::impl::get_max<nat_type>(9, m & 1 ? m : m + 1))
    { }

    PATL_EMIT(nat_type)
    {
        if (n_ <= 2) PATL_YIELD(2);
        if (n_ <= 3) PATL_YIELD(3);
        if (n_ <= 5) PATL_YIELD(5);
        if (n_ <= 7) PATL_YIELD(7);
        for (n_ = patl::impl::get_max<nat_type>(9, n_ & 1 ? n_ : n_ - 1); n_ < m_; )
        {
            bound_ = static_cast<word_t>(patl::impl::get_min<nat_type>((m_ - n_) / 2, B * bits_in_word));
            if (n_ < B)
                bound_ = static_cast<word_t>(patl::impl::get_min<nat_type>((n_ * n_ - n_) / 2, bound_));
            //
            {
                for (word_t i = 0; i != bound_ / bits_in_word + 1; ++i)
                    arr_[i] = 0;
                const nat_type sq_m(sqrt_int(n_ + bound_ * 2) + 1); // !!!
                std::auto_ptr<this_t> ps(new this_t(3, sq_m));
                if (ps->next())
                    do
                    {
                        const nat_type i = ps->value();
                        for (nat_type j = correct(corr0(n_ % i, i), i) / 2
                            ; j < bound_
                            ; j += i)
                            arr_[j / bits_in_word] |= 1 << (j % bits_in_word);
                    } while (ps->next());
            }
            //
            for (i_ = 0; ; ++i_, n_ += 2)
            {
                {
                    word_t
                        sh = i_ % bits_in_word,
                        a = ~arr_[i_ / bits_in_word] >> sh;
                    while (!a)
                    {
                        i_ += bits_in_word - sh;
                        n_ += 2 * (bits_in_word - sh);
                        sh = 0;
                        a = ~arr_[i_ / bits_in_word];
                    }
                    const word_t id = patl::impl::get_lowest_bit_id(a);
                    i_ += id;
                    n_ += 2 * id;
                }
                if (i_ == bound_)
                    break;
                PATL_YIELD(n_);
            }
        }
    }
    PATL_STOP_EMIT()

private:
    static nat_type corr0(const nat_type &n, const nat_type &i)
    { return n ? i - n : n; }

    static nat_type correct(const nat_type &n, const nat_type &i)
    { return n & 1 ? n + i : n; }

    nat_type n_, m_;
    word_t arr_[B + 1];
    word_t bound_, i_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
