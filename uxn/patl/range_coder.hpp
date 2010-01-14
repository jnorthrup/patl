/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Based on Russian Folk 'Carryless' Rangecoder (c) 1999, Dmitry Subbotin
-*/
#ifndef PATL_RANGE_CODER_HPP
#define PATL_RANGE_CODER_HPP

#include <cassert>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename This>
class range_coder_base
{
    typedef This this_t;

public:
    enum { BOT = 1 << 16, TOP = 1 << 24 }; // currently 32-bit only

    range_coder()
        : low_(0)
        , range_(~word_t(0))
    {
    }

private:
    void normalize()
    {
        while (
            (low_ ^ low_ + range_) < TOP ||
            range_ < BOT && ((range_ = (0 - low_) & BOT - 1), 1))
        {
            inner_normalize();
            range_ <<= 8;
            low_ <<= 8;
        }
    }

    word_t low_, range_;
};

} // namespace impl

template <typename Out>
class range_encoder
    : public impl::range_coder<range_encoder<Out> >
{
public:
    range_encoder(Out out)
        : out_(out)
    {
    }

    ~range_encoder()
    {
        for (word_t i = 0; i != sizeof(word_t); ++i)
        {
            out_(static_cast<unsigned char>(low_ >> 24));
            low_ <<= 8;
        }
    }

    void encode(word_t cum_freq, word_t freq, word_t tot_freq)
    {
        PATL_ASSERT(cum_freq + freq <= tot_freq && freq && tot_freq <= BOT);
        range_ /= tot_freq;
        low_ += cum_freq * range_;
        range_ *= freq;
        normalize();
    }

    void encode_bit(word_t bit)
    {
        range_ >>= 1;
        if (bit)
            low_ += range_;
        normalize();
    }

    void inner_normalize()
    {
        out_(static_cast<unsigned char>(low_ >> 24));
    }

private:
    Out out_;
};

template <typename In>
class range_decoder
    : public range_coder<range_decoder<In> >
{
public:
    range_decoder(In in)
        : in_(in)
        , code_(0)
    {
        for (word_t i = 0; i != 4; ++i)
            code_ = (code_ << 8) | in_();
    }

    word_t get_rng_freq(word_t tot_freq)
    {
        range_ /= tot_freq;
        const word_t freq = (code_ - low_) / range_;
        PATL_ASSERT(freq < tot_freq);
        return freq;
    }

    void decode(word_t cum_freq, word_t freq, word_t tot_freq)
    {
        PATL_ASSERT(cum_freq + freq <= tot_freq && freq && tot_freq <= BOT);
        low_ += cum_freq * range_;
        range_ *= freq;
        normalize()
    }

    word_t decode_bit()
    {
        range_ >>= 1;
        const word_t bit = (code_ - low_) / range_;
        PATL_ASSERT(bit < 2);
        if (bit)
            low_ += range_;
        normalize();
        return bit;
    }

    void inner_normalize()
    {
        code_ = (code_ << 8) | in_();
    }

private:
    In in_;
    word_t code_;
};

} // namespace patl
} // namespace uxn
