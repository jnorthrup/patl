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
class primes
    : public generator_base<primes<Natural>, Natural>
{
public:
    typedef Natural nat_type;

    enum { B = 12 << 10 };

    primes(const nat_type &n = 2, const nat_type &m = ~nat_type(0))
        : n_(n)
        , m_(get_max<nat_type>(9, m & 1 ? m : m + 1))
    { }

    PATL_EMIT
    {
        if (n_ <= 2) PATL_YIELD(2);
        if (n_ <= 3) PATL_YIELD(3);
        if (n_ <= 5) PATL_YIELD(5);
        if (n_ <= 7) PATL_YIELD(7);
        for (n_ = get_max<nat_type>(9, n_ & 1 ? n_ : n_ - 1); n_ < m_; )
        {
            bound_ = get_min(static_cast<word_t>((m_ - n_) / 2), B * bits_in_word);
            if (n_ < B)
                bound_ = get_min(static_cast<word_t>((n_ * n_ - n_) / 2), bound_);
            //
            {
                {
                    const word_t j = static_cast<word_t>((n_ - 3) / 2 % 105);
                    for (word_t i = 0, bmi = offs_[j % bits_in_word] + j / bits_in_word
                        ; i != bound_ / bits_in_word + 1
                        ; ++i, bmi = (bmi + 1) % 105)
                        arr_[i] = bm357_[bmi];
                    arr_[bound_ / bits_in_word] &=
                        ~word_t(0) >> (bits_in_word - bound_ % bits_in_word);
                }
                const nat_type sq_m(sqrt_int(n_ + bound_ * 2) + 1);
                if (sq_m >= 11)
                {
                    std::auto_ptr<primes> ps(new primes(9, sq_m));
                    if (ps->next())
                    {
                        do
                        {
                            const word_t i = static_cast<word_t>(ps->value());
                            for (word_t j = correct(corr0(static_cast<word_t>(n_ % i), i), i) / 2
                                ; j < bound_
                                ; j += i)
                                arr_[j / bits_in_word] |= 1 << (j % bits_in_word);
                        } while (ps->next());
                    }
                }
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
                    const word_t id = get_lowest_bit_id(a);
                    i_ += id;
                    n_ += 2 * id;
                }
                if (i_ >= bound_)
                    break;
                PATL_YIELD(n_);
            }
        }
    }
    PATL_STOP_EMIT

private:
    static word_t corr0(word_t n, word_t i)
    { return n ? i - n : n; }

    static word_t correct(word_t n, word_t i)
    { return n & 1 ? n + i : n; }

    nat_type n_, m_;
    word_t arr_[B + 1];
    word_t bound_, i_;

    static const word_t offs_[bits_in_word];
    static const word_t bm357_[105];
};

template <typename Nat>
const word_t primes<Nat>::offs_[bits_in_word] =
#ifdef PATL_32
{
    0, 23, 46, 69, 92, 10, 33, 56, 79, 102, 20, 43, 66, 89, 7, 30,
    53, 76, 99, 17, 40, 63, 86, 4, 27, 50, 73, 96, 14, 37, 60, 83
};
#elif PATL_64
{
    0, 64, 23, 87, 46, 5, 69, 28, 92, 51, 10, 74, 33, 97, 56, 15,
    79, 38, 102, 61, 20, 84, 43, 2, 66, 25, 89, 48, 7, 71, 30, 94,
    53, 12, 76, 35, 99, 58, 17, 81, 40, 104, 63, 22, 86, 45, 4, 68,
    27, 91, 50, 9, 73, 32, 96, 55, 14, 78, 37, 101, 60, 19, 83, 42
};
#endif
template <typename Nat>
const word_t primes<Nat>::bm357_[105] =
#ifdef PATL_32
{
    0xCDA59A4F, 0xB74976B2, 0x2CD2D9A6, 0x4B349E79, 0x92ED659B,
    0xA5B34D6E, 0x693CF259, 0xDACB3696, 0x669ADD25, 0x79E4B34B,
    0x966D2CD2, 0x35BA4BB5, 0xC96696CD, 0xDA59A4F3, 0x74976B2C,
    0xCD2D9A6B, 0xB349E792, 0x2ED659B4, 0x5B34D6E9, 0x93CF259A,
    0xACB36966, 0x69ADD25D, 0x9E4B34B6, 0x66D2CD27, 0x5BA4BB59,
    0x96696CD3, 0xA59A4F3C, 0x4976B2CD, 0xD2D9A6B7, 0x349E792C,
    0xED659B4B, 0xB34D6E92, 0x3CF259A5, 0xCB369669, 0x9ADD25DA,
    0xE4B34B66, 0x6D2CD279, 0xBA4BB596, 0x6696CD35, 0x59A4F3C9,
    0x976B2CDA, 0x2D9A6B74, 0x49E792CD, 0xD659B4B3, 0x34D6E92E,
    0xCF259A5B, 0xB3696693, 0xADD25DAC, 0x4B34B669, 0xD2CD279E,
    0xA4BB5966, 0x696CD35B, 0x9A4F3C96, 0x76B2CDA5, 0xD9A6B749,
    0x9E792CD2, 0x659B4B34, 0x4D6E92ED, 0xF259A5B3, 0x3696693C,
    0xDD25DACB, 0xB34B669A, 0x2CD279E4, 0x4BB5966D, 0x96CD35BA,
    0xA4F3C966, 0x6B2CDA59, 0x9A6B7497, 0xE792CD2D, 0x59B4B349,
    0xD6E92ED6, 0x259A5B34, 0x696693CF, 0xD25DACB3, 0x34B669AD,
    0xCD279E4B, 0xBB5966D2, 0x6CD35BA4, 0x4F3C9669, 0xB2CDA59A,
    0xA6B74976, 0x792CD2D9, 0x9B4B349E, 0x6E92ED65, 0x59A5B34D,
    0x96693CF2, 0x25DACB36, 0x4B669ADD, 0xD279E4B3, 0xB5966D2C,
    0xCD35BA4B, 0xF3C96696, 0x2CDA59A4, 0x6B74976B, 0x92CD2D9A,
    0xB4B349E7, 0xE92ED659, 0x9A5B34D6, 0x6693CF25, 0x5DACB369,
    0xB669ADD2, 0x279E4B34, 0x5966D2CD, 0xD35BA4BB, 0x3C96696C,
};
#elif PATL_64
{
    0xB74976B2CDA59A4Full, 0x4B349E792CD2D9A6ull, 0xA5B34D6E92ED659Bull,
    0xDACB3696693CF259ull, 0x79E4B34B669ADD25ull, 0x35BA4BB5966D2CD2ull,
    0xDA59A4F3C96696CDull, 0xCD2D9A6B74976B2Cull, 0x2ED659B4B349E792ull,
    0x93CF259A5B34D6E9ull, 0x69ADD25DACB36966ull, 0x66D2CD279E4B34B6ull,
    0x96696CD35BA4BB59ull, 0x4976B2CDA59A4F3Cull, 0x349E792CD2D9A6B7ull,
    0xB34D6E92ED659B4Bull, 0xCB3696693CF259A5ull, 0xE4B34B669ADD25DAull,
    0xBA4BB5966D2CD279ull, 0x59A4F3C96696CD35ull, 0x2D9A6B74976B2CDAull,
    0xD659B4B349E792CDull, 0xCF259A5B34D6E92Eull, 0xADD25DACB3696693ull,
    0xD2CD279E4B34B669ull, 0x696CD35BA4BB5966ull, 0x76B2CDA59A4F3C96ull,
    0x9E792CD2D9A6B749ull, 0x4D6E92ED659B4B34ull, 0x3696693CF259A5B3ull,
    0xB34B669ADD25DACBull, 0x4BB5966D2CD279E4ull, 0xA4F3C96696CD35BAull,
    0x9A6B74976B2CDA59ull, 0x59B4B349E792CD2Dull, 0x259A5B34D6E92ED6ull,
    0xD25DACB3696693CFull, 0xCD279E4B34B669ADull, 0x6CD35BA4BB5966D2ull,
    0xB2CDA59A4F3C9669ull, 0x792CD2D9A6B74976ull, 0x6E92ED659B4B349Eull,
    0x96693CF259A5B34Dull, 0x4B669ADD25DACB36ull, 0xB5966D2CD279E4B3ull,
    0xF3C96696CD35BA4Bull, 0x6B74976B2CDA59A4ull, 0xB4B349E792CD2D9Aull,
    0x9A5B34D6E92ED659ull, 0x5DACB3696693CF25ull, 0x279E4B34B669ADD2ull,
    0xD35BA4BB5966D2CDull, 0xCDA59A4F3C96696Cull, 0x2CD2D9A6B74976B2ull,
    0x92ED659B4B349E79ull, 0x693CF259A5B34D6Eull, 0x669ADD25DACB3696ull,
    0x966D2CD279E4B34Bull, 0xC96696CD35BA4BB5ull, 0x74976B2CDA59A4F3ull,
    0xB349E792CD2D9A6Bull, 0x5B34D6E92ED659B4ull, 0xACB3696693CF259Aull,
    0x9E4B34B669ADD25Dull, 0x5BA4BB5966D2CD27ull, 0xA59A4F3C96696CD3ull,
    0xD2D9A6B74976B2CDull, 0xED659B4B349E792Cull, 0x3CF259A5B34D6E92ull,
    0x9ADD25DACB369669ull, 0x6D2CD279E4B34B66ull, 0x6696CD35BA4BB596ull,
    0x976B2CDA59A4F3C9ull, 0x49E792CD2D9A6B74ull, 0x34D6E92ED659B4B3ull,
    0xB3696693CF259A5Bull, 0x4B34B669ADD25DACull, 0xA4BB5966D2CD279Eull,
    0x9A4F3C96696CD35Bull, 0xD9A6B74976B2CDA5ull, 0x659B4B349E792CD2ull,
    0xF259A5B34D6E92EDull, 0xDD25DACB3696693Cull, 0x2CD279E4B34B669Aull,
    0x96CD35BA4BB5966Dull, 0x6B2CDA59A4F3C966ull, 0xE792CD2D9A6B7497ull,
    0xD6E92ED659B4B349ull, 0x696693CF259A5B34ull, 0x34B669ADD25DACB3ull,
    0xBB5966D2CD279E4Bull, 0x4F3C96696CD35BA4ull, 0xA6B74976B2CDA59Aull,
    0x9B4B349E792CD2D9ull, 0x59A5B34D6E92ED65ull, 0x25DACB3696693CF2ull,
    0xD279E4B34B669ADDull, 0xCD35BA4BB5966D2Cull, 0x2CDA59A4F3C96696ull,
    0x92CD2D9A6B74976Bull, 0xE92ED659B4B349E7ull, 0x6693CF259A5B34D6ull,
    0xB669ADD25DACB369ull, 0x5966D2CD279E4B34ull, 0x3C96696CD35BA4BBull,
    0xB74976B2CDA59A4Full, 0x4B349E792CD2D9A6ull, 0xA5B34D6E92ED659Bull,
};
#endif

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
