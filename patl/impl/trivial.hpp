#ifndef PATL_IMPL_TRIVIAL_HPP
#define PATL_IMPL_TRIVIAL_HPP

#include "../config.hpp"

#ifdef PATL_INTRINSIC

#include <intrin.h>

#endif

namespace uxn
{
namespace patl
{
namespace impl
{

inline word_t highest_bit(word_t x)
{
    return x >> (bits_in_word - 1);
}

inline word_t bits_but_highest(word_t x)
{
    return x & ~(word_t(1) << (bits_in_word - 1));
}

inline word_t get_lowest_bit_id(word_t x)
{
#ifdef PATL_INTRINSIC
    unsigned long r;
#ifdef PATL_64
    _BitScanForward64(&r, x);
#else
    _BitScanForward(&r, x);
#endif
#else
    unsigned r = 0;
    x &= word_t(0) - x; // isolate lowest bit
#ifdef PATL_64
    if (x & 0xFFFFFFFF00000000) r += 32; // smth. wrong!
#endif
    if (x & 0xFFFF0000) r += 16;
    if (x & 0xFF00FF00) r += 8;
    if (x & 0xF0F0F0F0) r += 4;
    if (x & 0xCCCCCCCC) r += 2;
    if (x & 0xAAAAAAAA) ++r;
#endif
    return r;
}

inline word_t get_highest_bit_id(word_t x)
{
#ifdef PATL_INTRINSIC
    unsigned long r;
#ifdef PATL_64
    _BitScanReverse64(&r, x);
#else
    _BitScanReverse(&r, x);
#endif
#else
    unsigned r = 0;
#ifdef PATL_64
    if (x & 0xFFFFFFFF00000000) { x >>= 32; r += 32; }
#endif
    if (x & 0xFFFF0000) { x >>= 16; r += 16; }
    if (x & 0x0000FF00) { x >>= 8; r += 8; }
    if (x & 0x000000F0) { x >>= 4; r += 4; }
    if (x & 0x0000000C) { x >>= 2; r += 2; }
    if (x & 0x00000002) ++r;
#endif
    return r;
}

template <typename T>
inline const T &get_min(const T &a, const T &b)
{
    return a < b ? a : b;
}

template <typename T>
inline const T &get_max(const T &a, const T &b)
{
    return a < b ? b : a;
}

inline word_t get_binary_lca(word_t rootH, word_t i, word_t j)
{
    if (i == j)
        return i;
    //sort_2(i, j); // а нужно ли?
    const word_t
        iDeep = rootH - get_lowest_bit_id(i),
        jDeep = rootH - get_lowest_bit_id(j),
        xDeep = rootH - get_highest_bit_id(i ^ j);
    if (xDeep > get_min(iDeep, jDeep))
        return iDeep < jDeep ? i : j;
    const word_t sh = rootH - xDeep;
    return (i >> sh | word_t(1)) << sh;
}

template <word_t Align>
inline word_t align_down(word_t value)
{
    return value & (word_t(0) - Align);
}

template <word_t Align>
inline word_t align_up(word_t value)
{
    return value + Align - word_t(1) & (word_t(0) - Align);
}

// Return max(0,x), i.e. return zero for negative input (from J. Arndt)
inline word_t max0(sword_t x)
{
    return static_cast<word_t>(x & ~(x >> (bits_in_word - 1)));
}

template <typename Source>
inline word_t unsigned_cast(Source val)
{
    return *reinterpret_cast<word_t*>(&val) &
        ~word_t(1) >> (sizeof(word_t) - sizeof(Source)) * 8;
}

template <typename T>
inline word_t bit_mismatch_scalar(T a, T b)
{
    return 8 * sizeof(T) - 1 -
        get_highest_bit_id(unsigned_cast(a) ^ unsigned_cast(b));
}

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
