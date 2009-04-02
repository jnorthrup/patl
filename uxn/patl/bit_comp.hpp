#ifndef PATL_BIT_COMP_HPP
#define PATL_BIT_COMP_HPP

#include <string>
#include <vector>
#include "impl/trivial.hpp"

namespace uxn
{
namespace patl
{

/// restriction: NO key CAN BE a prefix of any other key
template <typename T>
class bit_comparator
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
    word_t bit_mismatch(T a, T b) const
    {
        return a == b ? ~word_t(0) : impl::bit_mismatch_scalar(a, b);
    }
};

template <>
class bit_comparator<bool>
{
public:
    static const word_t bit_size = 1;

    word_t bit_length(bool) const
    {
        return 1;
    }

    word_t get_bit(bool val, word_t) const
    {
        return val ? 1 : 0;
    }

    word_t bit_mismatch(bool a, bool b) const
    {
        return a == b ? ~word_t(0) : 0;
    }
};

/// simple infinite (T*) string
template <typename T>
class bit_comparator<T*>
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;

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

    word_t bit_mismatch(const T *a, const T *b) const
    {
        if (a == b)
            return ~word_t(0);
        for (word_t i = 0; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
    }
};

/// simple infinite (T**) string (treat (T*) as word_t)
template <typename T>
class bit_comparator<T**>
    : public bit_comparator<word_t>
{
    typedef bit_comparator<word_t> super;

public:
    typedef T *char_type;

    static const word_t bit_size = 8 * sizeof(word_t);

    word_t bit_length(T* const *) const
    {
        return ~word_t(0);
    }

    word_t get_bit(T * const *s, word_t id) const
    {
        return super::get_bit(
            reinterpret_cast<const word_t*>(s)[id / bit_size],
            id % bit_size);
    }

    word_t bit_mismatch(T * const *ap, T * const *bp) const
    {
        const word_t
            *a = reinterpret_cast<const word_t*>(ap),
            *b = reinterpret_cast<const word_t*>(bp);
        if (a == b)
            return ~word_t(0);
        for (word_t i = 0; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
    }
};

/// standard finite std::basic_string
template <typename T>
class bit_comparator<std::basic_string<T> >
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;
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

    word_t bit_mismatch(const value_type &a, const value_type &b) const
    {
        if (&a == &b)
            return ~word_t(0);
        return b.length() < a.length()
            ? mismatch_intern(b, a)
            : mismatch_intern(a, b);
    }

private:
    word_t mismatch_intern(const value_type &a, const value_type &b) const
    {
        word_t i = 0;
        for (; i <= a.length(); ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
        return ~word_t(0);
    }
};

/// standard finite std::vector
template <typename T>
class bit_comparator<std::vector<T> >
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;
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

    word_t bit_mismatch(const value_type &a, const value_type &b) const
    {
        if (&a == &b)
            return ~word_t(0);
        return b.size() < a.size()
            ? mismatch_intern(b, a)
            : mismatch_intern(a, b);
    }

private:
    word_t mismatch_intern(const value_type &a, const value_type &b) const
    {
        word_t i = 0;
        for (; i != a.size(); ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
        return ~word_t(0);
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

/// simple zero-terminated (T*) string
template <typename T>
class bit_comparator_0
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;

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

    word_t bit_mismatch(const T *a, const T *b) const
    {
        if (a == b)
            return ~word_t(0);
        for (word_t i = 0; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
            else if (!a[i])
                return ~word_t(0);
        }
    }
};

} // namespace patl
} // namespace uxn

#endif
