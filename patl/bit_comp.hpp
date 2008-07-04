#ifndef PATL_BIT_COMP_HPP
#define PATL_BIT_COMP_HPP

#include <string>
#include "impl/trivial.hpp"

namespace uxn
{
namespace patl
{

// interface of bit comparator
// restriction: NO key CAN BE a prefix of any other key
template <typename T>
class bit_comparator
{
public:
    // optional; used in non-scalar bit_comparator's
    // NOTE must be power of 2 for proper work of mismatch_suffix
    static const word_t bit_size;

    // return key length in bits, ~word_t(0) if infinite
    word_t bit_length(const T&) const;

    // return bit by id in key (must be 0 or 1)
    word_t get_bit(const T&, word_t) const;

    // return id of first mismatch bit, ~word_t(0) if keys are equal
    word_t bit_mismatch(const T&, const T&) const;
};

template <typename T>
class reverse_bit_comparator
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;

public:
    word_t get_bit(const T &val, word_t id) const
    {
        return word_t(1) ^ super::get_bit(val, id);
    }
};

// standard finite std::basic_string
template <typename T>
class bit_comparator<std::basic_string<T> >
{
    typedef std::basic_string<T> value_type;

public:
    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const value_type &s) const
    {
        return bit_size * (static_cast<word_t>(s.length()) + 1);
    }

    word_t get_bit(const value_type &s, word_t id) const
    {
        return (s[id / bit_size] >>
            ((bit_size - 1) - id % bit_size)) & word_t(1);
    }

    word_t bit_mismatch(const value_type &a, const value_type &b) const
    {
        word_t i;
        if (a.length() == b.length())
        {
            typename value_type::const_iterator
                it1 = a.begin(),
                it2 = b.begin();
            for (; it1 != a.end() && *it1 == *it2; ++it1, ++it2) ;
            if (it1 == a.end())
                return ~word_t(0);
            i = static_cast<word_t>(it1 - a.begin());
        }
        else
            for (i = 0; a[i] == b[i]; ++i) ;
        return i * bit_size + impl::bit_mismatch_scalar(a[i], b[i]);
    }
};

// simple infinite (T*) string
template <typename T>
class bit_comparator<T*>
{
public:
    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const T*) const
    {
        return ~word_t(0);
    }

    word_t get_bit(const T *s, word_t id) const
    {
        return word_t(1) & (impl::unsigned_cast(s[id / bit_size]) >>
            ((bit_size - 1) - id % bit_size));
    }

    word_t bit_mismatch(const T *a, const T *b) const
    {
        if (a == b)
            return ~word_t(0);
        word_t i = 0;
        for (; a[i] == b[i]; ++i) ;
        return i * bit_size + impl::bit_mismatch_scalar(a[i], b[i]);
    }
};

template <typename T>
class ptr_bit_comparator
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;

public:
    word_t bit_length(const T *p) const
    {
        return super::bit_length(*p);
    }

    word_t get_bit(const T *s, word_t id) const
    {
        return super::get_bit(*s, id);
    }

    word_t bit_mismatch(const T *a, const T *b) const
    {
        return super::bit_mismatch(*a, *b);
    }
};

// finite (char*) string with ASCIIZ semantics
class asciiz_bit_comparator
{
public:
    static const word_t bit_size = 8;

    word_t bit_length(const char *s) const
    {
        const char *beg = s;
        while (*s++) ;
        return (s - beg + 1) * 8;
    }

    word_t get_bit(const char *s, word_t id) const
    {
        return (s[id / 8] >> (word_t(7) - id % 8)) & word_t(1);
    }

    word_t bit_mismatch(const char *a, const char *b) const
    {
        if (a == b)
            return ~word_t(0);
        word_t i = 0;
        for (; a[i] && b[i] && a[i] == b[i]; ++i);
        if (!a[i] && !b[i])
            return ~word_t(0);
        word_t m = a[i] ^ b[i];
        for (i *= 8; (m & 0x80) != 0x80; ++i, m <<= 1) ;
        return i;
    }
};

// char type
template<>
class bit_comparator<char>
{
public:
    word_t bit_length(char) const
    {
        return 8 * sizeof(char);
    }

    word_t get_bit(char val, word_t id) const
    {
        return word_t(1) & (impl::unsigned_cast(val) >>
            (8 * sizeof(char) - 1 - id));
    }

    word_t bit_mismatch(char a, char b) const
    {
        if (a == b)
            return ~word_t(0);
        return impl::bit_mismatch_scalar(a, b);
    }
};

// unsigned char type
template<>
class bit_comparator<unsigned char>
{
public:
    word_t bit_length(unsigned char) const
    {
        return 8 * sizeof(unsigned char);
    }

    word_t get_bit(unsigned char val, word_t id) const
    {
        return word_t(1) & (impl::unsigned_cast(val) >>
            (8 * sizeof(unsigned char) - 1 - id));
    }

    word_t bit_mismatch(unsigned char a, unsigned char b) const
    {
        if (a == b)
            return ~word_t(0);
        return impl::bit_mismatch_scalar(a, b);
    }
};

// wchar_t type
template<>
class bit_comparator<wchar_t>
{
public:
    word_t bit_length(wchar_t) const
    {
        return 8 * sizeof(wchar_t);
    }

    word_t get_bit(wchar_t val, word_t id) const
    {
        return word_t(1) & (impl::unsigned_cast(val) >>
            (8 * sizeof(wchar_t) - 1 - id));
    }

    word_t bit_mismatch(wchar_t a, wchar_t b) const
    {
        if (a == b)
            return ~word_t(0);
        return impl::bit_mismatch_scalar(a, b);
    }
};

// unsigned type
template<>
class bit_comparator<word_t>
{
public:
    word_t bit_length(word_t) const
    {
        return 8 * sizeof(word_t);
    }

    word_t get_bit(word_t val, word_t id) const
    {
        return word_t(1) & (val >> (8 * sizeof(word_t) - 1 - id));
    }

    word_t bit_mismatch(word_t a, word_t b) const
    {
        if (a == b)
            return ~word_t(0);
        return impl::bit_mismatch_scalar(a, b);
    }
};

// signed type
template<>
class bit_comparator<sword_t>
{
public:
    word_t bit_length(sword_t) const
    {
        return 8 * sizeof(sword_t);
    }

    word_t get_bit(sword_t val, word_t id) const
    {
        const word_t uval =
            (word_t(1) << (8 * sizeof(sword_t) - 1)) ^ impl::unsigned_cast(val);
        return (uval >> (8 * sizeof(sword_t) - 1 - id)) & word_t(1);
    }

    word_t bit_mismatch(sword_t a, sword_t b) const
    {
        if (a == b)
            return ~word_t(0);
        return impl::bit_mismatch_scalar(a, b);
    }
};

} // namespace patl
} // namespace uxn

#endif
