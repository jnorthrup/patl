/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_BIT_COMP_HPP
#define PATL_BIT_COMP_HPP

#include <string>
#include <vector>
#include "impl/trivial.hpp"

namespace uxn
{
namespace patl
{

template <typename T, word_t N = 0>
class bit_comparator
{
};

/// restriction: NO key CAN BE a prefix of any other key
template <typename T>
class bit_comparator<T, 0>
{
public:
    // optional; used in non-scalar bit_comparator's
    //typedef undefined char_type;

    /// NOTE must be power of 2 for proper work of mismatch_suffix
    static const word_t bit_size = 8 * sizeof(T);

    /// return key length in bits, ~word_t(0) if infinite
    word_t bit_length(T) const
    {
        return 8 * sizeof(T);
    }

    /// return bit by id in key (must be 0 or 1)
    word_t get_bit(T val, word_t id) const
    {
        return word_t(1) & (impl::unsigned_cast(val) >>
            (8 * sizeof(T) - 1 - id));
    }

    /// return id of first mismatch bit, ~word_t(0) if keys are equal
    word_t bit_mismatch(T a, T b, word_t = 0) const
    {
        return a == b ? ~word_t(0) : impl::bit_mismatch_scalar(a, b);
    }
};

template <typename T>
class bit_comparator<T, 8>
    : public bit_comparator<T, 0>
{
public:
    word_t get_bits(T val, word_t off) const
    {
        return 0xFF & impl::unsigned_cast(val) >> off % sizeof(T) * 8;
    }
};

template <>
class bit_comparator<char, 8>
    : public bit_comparator<char, 0>
{
public:
    word_t get_bits(char val, word_t) const
    {
        return val;
    }
};

template <>
class bit_comparator<unsigned char, 8>
    : public bit_comparator<unsigned char, 0>
{
public:
    word_t get_bits(unsigned char val, word_t) const
    {
        return val;
    }
};

template <>
class bit_comparator<word_t, 16>
    : public bit_comparator<word_t, 0>
{
    word_t get_bits(word_t val, word_t off) const
    {
        return 0xFFFF & val >> off % sizeof(word_t) * 8;
    }
};

/// simple infinite (T*) string
template <typename T>
class bit_comparator<T*, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;

public:
    typedef T char_type;

    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const T*) const
    {
        return ~word_t(0);
    }

    word_t get_bit(const T *s, word_t id) const
    {
        return super::get_bit(s[id / bit_size], id % bit_size);
    }

    word_t bit_mismatch(const T *a, const T *b, word_t skip = 0) const
    {
        if (a == b)
            return ~word_t(0);
        for (word_t i = skip / bit_size; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
    }

    word_t get_bits(const T *s, word_t off) const
    {
        return super::get_bits(s[off / sizeof(T)], off);
    }
};

template <typename T>
class bit_comparator<T*, 8>
    : public bit_comparator<T*, 0>
{
public:
    word_t get_bits(const T *s, word_t off) const
    {
        return bit_comparator<T, 8>::get_bits(s[off / sizeof(T)], off);
    }
};

/// simple infinite (T**) string (treat (T*) as word_t)
template <typename T>
class bit_comparator<T**, 0>
    : public bit_comparator<word_t, 0>
{
    typedef bit_comparator<word_t, 0> super;

public:
    typedef T *char_type;

    static const word_t bit_size = bits_in_word;

    word_t bit_length(T* const*) const
    {
        return ~word_t(0);
    }

    word_t get_bit(T * const *s, word_t id) const
    {
        return super::get_bit(
            reinterpret_cast<const word_t*>(s)[id / bit_size],
            id % bit_size);
    }

    word_t bit_mismatch(T * const *ap, T * const *bp, word_t skip = 0) const
    {
        const word_t
            *a = reinterpret_cast<const word_t*>(ap),
            *b = reinterpret_cast<const word_t*>(bp);
        if (a == b)
            return ~word_t(0);
        for (word_t i = skip / bit_size; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
    }
};

template <typename T>
class bit_comparator<T**, 8>
    : public bit_comparator<T**, 0>
{
public:
    word_t get_bits(T * const *s, word_t off) const
    {
        return bit_comparator<word_t, 8>::get_bits(reinterpret_cast<const word_t*>(s)[off / sizeof(word_t)], off);
    }
};

template <typename T>
class bit_comparator<T**, 16>
    : public bit_comparator<T**, 0>
{
public:
    word_t get_bits(T * const *s, word_t off) const
    {
        return bit_comparator<word_t, 16>::get_bits(reinterpret_cast<const word_t*>(s)[off / sizeof(word_t)], off);
    }
};

/// standard finite std::basic_string
template <typename T>
class bit_comparator<std::basic_string<T>, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;
    typedef std::basic_string<T> value_type;

public:
    typedef T char_type;

    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const value_type &s) const
    {
        return bit_size * (s.length() + 1);
    }

    word_t get_bit(const value_type &s, word_t id) const
    {
        return super::get_bit(s[id / bit_size], id % bit_size);
    }

    word_t bit_mismatch(
        const value_type &a,
        const value_type &b,
        word_t skip = 0) const
    {
        if (&a == &b)
            return ~word_t(0);
        return b.length() < a.length()
            ? mismatch_intern(b, a, skip)
            : mismatch_intern(a, b, skip);
    }

private:
    word_t mismatch_intern(
        const value_type &a,
        const value_type &b,
        word_t skip) const
    {
        for (word_t i = skip / bit_size; i <= a.length(); ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
        return ~word_t(0);
    }
};

template <typename T>
class bit_comparator<std::basic_string<T>, 8>
    : public bit_comparator<std::basic_string<T>, 0>
{
    typedef std::basic_string<T> value_type;

public:
    word_t get_bits(const value_type &s, word_t off) const
    {
        return bit_comparator<T, 8>::get_bits(s[off / sizeof(T)], off);
    }
};

/// standard finite std::vector
template <typename T>
class bit_comparator<std::vector<T>, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;
    typedef std::vector<T> value_type;

public:
    typedef T char_type;

    static const word_t bit_size = super::bit_size;

    word_t bit_length(const value_type &v) const
    {
        return bit_size * v.size();
    }

    word_t get_bit(const value_type &v, word_t id) const
    {
        return super::get_bit(v[id / bit_size], id % bit_size);
    }

    word_t bit_mismatch(
        const value_type &a,
        const value_type &b,
        word_t skip = 0) const
    {
        if (&a == &b)
            return ~word_t(0);
        return b.size() < a.size()
            ? mismatch_intern(b, a, skip)
            : mismatch_intern(a, b, skip);
    }

private:
    word_t mismatch_intern(
        const value_type &a,
        const value_type &b,
        word_t skip) const
    {
        for (word_t i = skip / bit_size; i != a.size(); ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
        return ~word_t(0);
    }
};

template <typename T>
class bit_comparator<std::vector<T>, 8>
    : public bit_comparator<std::vector<T>, 0>
{
public:
    word_t get_bits(const std::vector<T> &s, word_t off) const
    {
        return bit_comparator<T, 8>::get_bits(s[off / sizeof(T)], off);
    }
};

/// keys dereferenced from pointers
template <typename T, word_t N = 0>
class ptr_bit_comparator
    : public bit_comparator<T, N>
{
    typedef bit_comparator<T, N> super;

public:
    word_t bit_length(const T *p) const
    {
        return super::bit_length(*p);
    }

    word_t get_bit(const T *s, word_t id) const
    {
        return super::get_bit(*s, id);
    }

    word_t bit_mismatch(const T *a, const T *b, word_t skip = 0) const
    {
        return super::bit_mismatch(*a, *b, skip);
    }

    word_t get_bits(const T *s, word_t off) const
    {
        return super::get_bits(*s, off);
    }
};

/// bit-comparator for reversed lexicographical ordering
template <typename T, word_t N = 0>
class reverse_bit_comparator
    : public bit_comparator<T, N>
{
    typedef bit_comparator<T, N> super;

public:
    word_t get_bit(const T &val, word_t id) const
    {
        return 1 ^ super::get_bit(val, id);
    }

    word_t get_bits(const T &s, word_t off) const
    {
        return ~word_t(0) >> bits_in_word - N ^ super::get_bits(s, off);
    }
};

/// simple zero-terminated (T*) string
template <typename T, word_t N = 0>
class bit_comparator_0
    : public bit_comparator_0<T, 0>
{
public:
    word_t get_bits(const T *s, word_t off) const
    {
        return bit_comparator<T, N>::get_bits(s[off / sizeof(T)], off);
    }
};

template <typename T>
class bit_comparator_0<T, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;

public:
    typedef T char_type;

    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const T*) const
    {
        return ~word_t(0);
    }

    word_t get_bit(const T *s, word_t id) const
    {
        return super::get_bit(s[id / bit_size], id % bit_size);
    }

    word_t bit_mismatch(const T *a, const T *b, word_t skip = 0) const
    {
        if (a == b)
            return ~word_t(0);
        for (word_t i = skip / bit_size; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
            else if (!a[i])
                return ~word_t(0);
        }
    }
};

/// statically Q-bounded bit-comparator
template <typename T, word_t Q, word_t N = 0, word_t Delta = 1>
class bounded_bit_comparator
     : public bounded_bit_comparator<T, Q, 0, Delta>
{
public:
    word_t get_bits(const T *s, word_t off) const
    {
        return bit_comparator<T, N>::get_bits(s[off / sizeof(T)], off);
    }
};

template <typename T, word_t Q, word_t Delta>
class bounded_bit_comparator<T, Q, 0, Delta>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;

public:
    typedef T char_type;

    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const T*) const
    {
        return bit_size * Q * Delta;
    }

    word_t get_bit(const T *s, word_t id) const
    {
        return super::get_bit(s[id / bit_size], id % bit_size);
    }

    word_t bit_mismatch(const T *a, const T *b, word_t skip = 0) const
    {
        if (a == b)
            return ~word_t(0);
        for (word_t i = skip / bit_size; i != Q * Delta; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
        return ~word_t(0);
    }
};

} // namespace patl
} // namespace uxn

#endif
