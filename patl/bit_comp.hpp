#ifndef PATL_BIT_COMP_HPP
#define PATL_BIT_COMP_HPP

#include <string>
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
    // NOTE must be power of 2 for proper work of mismatch_suffix
    //static const word_t bit_size;

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

/// simple infinite (T*) string
template <typename T>
class bit_comparator<T*>
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;

public:
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
        word_t
            i = 0,
            m;
        for (; (m = super::bit_mismatch(a[i], b[i])) == ~word_t(0); ++i) ;
        return i * bit_size + m;
    }
};

/// simple infinite (T**) string (treat (T*) as word_t)
template <typename T>
class bit_comparator<T**>
    : public bit_comparator<word_t>
{
    typedef bit_comparator<word_t> super;

public:
    static const word_t bit_size = 8 * sizeof(word_t);

    word_t bit_length(T* const *) const
    {
        return ~word_t(0);
    }

    word_t get_bit(T * const *s, word_t id) const
    {
        return super::get_bit(
            impl::unsigned_cast(s[id / bit_size]),
            id % bit_size);
    }

    word_t bit_mismatch(T * const *a, T * const *b) const
    {
        if (a == b)
            return ~word_t(0);
        word_t
            i = 0,
            m;
        for (
            ; (m = super::bit_mismatch(
                impl::unsigned_cast(a[i]),
                impl::unsigned_cast(b[i]))) == ~word_t(0)
            ; ++i) ;
        return i * bit_size + m;
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
    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const value_type &s) const
    {
        return bit_size * (s.length() + 1);
    }

    word_t get_bit(const value_type &s, word_t id) const
    {
        const word_t i = id / bit_size;
        return i == s.length() ? 0 : super::get_bit(s[i], id % bit_size);
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
        typename value_type::const_iterator
            ita = a.begin(),
            itb = b.begin();
        word_t m = 0xBADC0DE; // to avoid warning C4701
        for (
            ; ita != a.end() && (m = super::bit_mismatch(*ita, *itb)) == ~word_t(0)
            ; ++ita, ++itb) ;
        const word_t len = std::distance(a.begin(), ita) * bit_size;
        return ita == a.end()
            ? (itb == b.end() ? ~word_t(0) : len + super::bit_mismatch(T(), *itb))
            : len + m;
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

} // namespace patl
} // namespace uxn

#endif
