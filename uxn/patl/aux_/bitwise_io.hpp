/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_BITWISE_IO_HPP
#define PATL_AUX_BITWISE_IO_HPP

#include "../config.hpp"
#include "../impl/trivial.hpp"
#include <cstring>

namespace uxn
{
namespace patl
{
namespace aux
{

const word_t fibs[] =
{ 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987 // n <= 1024
, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368         // n <= 65536
};

template <typename Output>
class bit_output
{
    bit_output &operator=(const bit_output&);

public:
    bit_output(Output &dst, word_t size = 1 << 16)
        : dst_(dst)
        , beg_(new word_t [size])
        , end_(beg_ + size)
        , cur_(beg_)
        , num_(0)
    {
        ::memset(beg_, 0, size * sizeof(word_t));
    }

    ~bit_output()
    {
        flush();
        delete[] beg_;
    }

    void put_bit(word_t bit)
    {
        *cur_ |= (bit & 1) << num_;
        if (++num_ & bits_in_word)
        {
            num_ = 0;
            flush_full();
        }
    }

    void put_bits(word_t bits, word_t n)
    {
        const word_t bits0 = bits & (1 << n) - 1;
        *cur_ |= bits0 << num_;
        if ((num_ += n) & bits_in_word)
        {
            num_ &= bits_in_word - 1;
            flush_full();
            *cur_ = bits0 >> (n - num_);
        }
    }

    void put_fib_code(word_t n)
    {
        word_t i = n < 1597 ? 14 : sizeof(fibs) / sizeof(*fibs) - 1;
        for (; fibs[i] > n; --i) ;
        const word_t highest = i + 1;
        word_t code = 1 << highest;
        for (; n; --i)
        {
            if (fibs[i] <= n)
            {
                code |= 1 << i;
                n -= fibs[i--];
            }
        }
        put_bits(code, highest + 1);
    }

    void put_word(word_t w)
    {
        if (num_)
        {
            *cur_ |= w << num_;
            flush_full();
            *cur_ = w >> (bits_in_word - num_);
        }
        else
        {
            *cur_ = w;
            flush_full();
        }
    }

    void put_bytes(const char *bytes, word_t n)
    {
        const word_t
            *cur = reinterpret_cast<const word_t*>(bytes),
            *end = cur + n / sizeof(word_t);
        for (; cur != end; ++cur)
            put_word(*cur);
        if (n % sizeof(word_t))
            put_bits(*cur, (n % sizeof(word_t)) * 8);
    }

private:
    void flush_full()
    {
        if (++cur_ == end_)
        {
            const word_t size = (end_ - beg_) * sizeof(word_t);
            dst_(beg_, size);
            memset(beg_, 0, size);
            cur_ = beg_;
        }
    }

    void flush()
    {
        const word_t size = (cur_ - beg_) * sizeof(word_t) +
            impl::align_up<8>(num_) / 8;
        if (size)
        {
            dst_(beg_, size);
            memset(beg_, 0, size);
            cur_ = beg_;
            num_ = 0;
        }
    }

    Output &dst_;
    word_t
        * const beg_,
        * const end_,
        *cur_;
    word_t num_;
};

template <typename Input>
class bit_input
{
    bit_input &operator=(const bit_input&);

public:
    bit_input(Input &src, word_t size = 1 << 16)
        : src_(src)
        , beg_(new word_t [size])
        , end_(beg_ + size)
        , cur_(beg_)
        , num_(0)
    {
        src_(beg_, size * sizeof(word_t));
    }

    ~bit_input()
    {
        delete[] beg_;
    }

    word_t get_bit()
    {
        const word_t bit = *cur_ >> num_ & 1;
        if (++num_ & bits_in_word)
        {
            num_ = 0;
            read_full();
        }
        return bit;
    }

    word_t get_bits(word_t n)
    {
        word_t bits = *cur_ >> num_;
        if ((num_ += n) & bits_in_word)
        {
            num_ &= bits_in_word - 1;
            read_full();
            bits |= *cur_ << (n - num_);
        }
        return bits & (1 << n) - 1;
    }

    word_t get_fib_code()
    {
        for (word_t n = 0, i = 0; ; ++i)
        {
            if (get_bit())
            {
                n += fibs[i++];
                if (get_bit())
                    return n;
            }
        }
    }

    word_t get_word()
    {
        if (num_)
        {
            const word_t dword = *cur_ >> num_;
            read_full();
            return dword | *cur_ << (bits_in_word - num_);
        }
        else
        {
            const word_t dword = *cur_;
            read_full();
            return dword;
        }
    }

    void get_bytes(char *bytes, word_t n)
    {
        word_t
            *cur = reinterpret_cast<word_t*>(bytes),
            *end = cur + n / sizeof(word_t);
        for (; cur != end; ++cur)
            *cur = get_word();
        if (n % sizeof(word_t))
            *cur = get_bits((n % sizeof(word_t)) * 8);
    }

private:
    void read_full()
    {
        if (++cur_ == end_)
        {
            src_(beg_, (end_ - beg_) * sizeof(word_t));
            cur_ = beg_;
        }
    }

    Input &src_;
    word_t
        * const beg_,
        * const end_,
        *cur_;
    word_t num_;
};

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
