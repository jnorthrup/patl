/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | LZ77 data compression codec using online suffix tree
-*/
#include <uxn/patl/aux_/bitwise_io.hpp>
#include <uxn/patl/suffix_set.hpp>

namespace patl = uxn::patl;

typedef patl::suffix_set<char*> suffix_t;
typedef suffix_t::vertex vertex;

template <typename Input, typename Output>
int lz_compress(unsigned n_pow, unsigned w_pow, unsigned min_match, Input &src, Output &dst)
{
    int retcod = 0;
    //
    const unsigned
        n = 1 << n_pow,
        w = 1 << w_pow;
    unsigned count = 0;
    long long total = 0;
    const char stop_bytes[] =
        "\x81\xAE\xA4\x11\xB6\x18\x1C\xA7"
        "\xF5\xE8\xF9\xD2\x7C\x2D\x0E\x3C";
    char
        *const buf = new char [2 * n + w + sizeof(stop_bytes)],
        *const buf2 = buf + n,
        *const buf3 = buf2 + n;
    memcpy(buf3 + w, stop_bytes, sizeof(stop_bytes));
    suffix_t sufs(buf2, n);
    patl::aux::bit_output<Output> bdst(dst);
    bdst.put_fib_code(n_pow);
    //put_fib_code(bdst,w_pow);
    bdst.put_fib_code(min_match);
    char *tail;
    for (bool first_time = true, incomplete = false;
        ;
        first_time = false)
    {
        if (!incomplete)
        {
            char *beg = first_time ? buf2 : buf2 + w;
            const unsigned reindex_cnt = first_time ? 0 : sufs.count_reindex(beg);
            const bool need_reindex = reindex_cnt < sufs.size() / 4;
            if (!first_time && need_reindex)
                sufs.pop_reindex(reindex_cnt);
            const unsigned
                toread = first_time ? n + w : n,
                avail = src(beg, toread);
            if (!first_time)
            {
                if (need_reindex)
                    sufs.push_reindex(reindex_cnt);
                else
                {
                    unsigned size = sufs.size();
                    sufs.clear();
                    sufs.push_reindex(size);
                }
            }
            if (!avail)
                break;
            incomplete = avail < toread;
            tail = beg + std::min(toread, avail);
        }
        bool copy_state = false;
        unsigned
            copy_count = 0,
            skip = 0;
        const char
            *cur = buf2,
            *end = std::min(buf3, tail);
        for (;
            cur != end;
            ++cur, skip = patl::impl::max0(skip - 1))
        {
            if (!first_time)
                sufs.pop_front();
            vertex v = sufs.push_back();
            if (!skip)
            {
                skip = std::min(
                    w,
                    std::min(
                        patl::impl::max0(v.skip()) / 8,
                        patl::impl::unsigned_cast(end - cur)));
                const char *match = v.sibling().key();
                ++count;
                if (skip < min_match)
                {
                    ++total;
                    if (copy_state)
                    {
                        if (copy_count == 65536)
                        {
                            bdst.put_fib_code(1);
                            bdst.put_fib_code(copy_count);
                            bdst.put_bytes(cur - copy_count, copy_count);
                            copy_count = 1;
                        }
                        else
                            ++copy_count;
                    }
                    else
                    {
                        copy_count = 1;
                        copy_state = true;
                    }
                    skip = 0;
                }
                else
                {
                    total += skip;
                    if (copy_state)
                    {
                        bdst.put_fib_code(1);
                        bdst.put_fib_code(copy_count);
                        bdst.put_bytes(cur - copy_count, copy_count);
                        copy_state = false;
                    }
                    bdst.put_fib_code(skip - min_match + 3);
                    bdst.put_bits(cur - match, patl::impl::get_highest_bit_id(sufs.size() - 1) + 1);
                }
            }
        }
        if (copy_state)
        {
            bdst.put_fib_code(1);
            bdst.put_fib_code(copy_count);
            bdst.put_bytes(cur - copy_count, copy_count);
        }
        if (incomplete && tail < buf3)
            break;
        memmove(buf, buf2, n + w + sizeof(stop_bytes));
        tail -= n;
        sufs.rebind(buf);
    }
    bdst.put_fib_code(2); // EOF
    delete[] buf;
    //
    return retcod;
}

//  опирует данные из буфера в буфер, ид€ в пор€дке возрастани€ адресов
// (это важно, поскольку буфера могут пересекатьс€ и в этом случае нужно
// размножить существующие данные) by Bulat Ziganshin
inline void copymem(char *dst, const char *src, unsigned len)
{
    while (len--)
        *dst++ = *src++;
}

template <typename Input, typename Output>
int lz_decompress(Input &src, Output &dst)
{
    int retcod = 0;
    //
    patl::aux::bit_input<Input> bsrc(src);
    const unsigned
        n_pow = bsrc.get_fib_code(),
        n = 1 << n_pow,
        min_match = bsrc.get_fib_code();
    char
        *const buf = new char [2 * n/*+w*/],
        *const buf2 = buf + n,
        *const buf3 = buf2 + n;
    for (bool first_time = true; ; first_time = false)
    {
        char
            *cur = buf2,
            *end = buf3;
        for (; cur != end; )
        {
            const unsigned code = bsrc.get_fib_code();
            if (code == 1)
            {
                const unsigned copy_count = bsrc.get_fib_code();
                bsrc.get_bytes(cur, copy_count);
                cur += copy_count;
            }
            else if (code == 2)
            {
                dst(buf2, cur - buf2);
                goto eof;
            }
            else
            {
                const unsigned
                    skip = code - 3 + min_match,
                    match = bsrc.get_bits(patl::impl::get_highest_bit_id(
                        first_time ? cur - buf2 : n - 1) + 1);
                copymem(cur, cur - match, skip);
                cur += skip;
            }
        }
        dst(buf2, cur - buf2);
        memcpy(buf, buf2, n);
    }
eof:
    delete[] buf;
    //
    return retcod;
}

class input_functor
{
public:
    input_functor(const char *fname)
        : fin_(fopen(fname, "rb"))
    {
    }

    ~input_functor()
    {
        fclose(fin_);
    }

    bool operator!() const
    {
        return !fin_;
    }

    unsigned operator()(void *buf, unsigned size)
    {
        return fread(buf, 1, size, fin_);
    }

private:
    FILE *fin_;
};

class output_functor
{
public:
    output_functor(const char *fname)
        : fout_(fopen(fname, "wb"))
    {
    }

    ~output_functor()
    {
        fclose(fout_);
    }

    bool operator!() const
    {
        return !fout_;
    }

    unsigned operator()(const void *buf, unsigned size)
    {
        return fwrite(buf, 1, size, fout_);
    }

private:
    FILE *fout_;
};

void info()
{
    printf("LZ77 <e|d> infile outfile\n");
}

int main(int argc, char *argv[])
{
    if (argc < 4 || argv[1][1])
    {
        info();
        return 0;
    }
    // open input
    input_functor fin(argv[2]);
    if (!fin)
    {
        printf("Can't open %s for read\n", argv[2]);
        return 3;
    }
    // open output
    output_functor fout(argv[3]);
    if (!fout)
    {
        printf("Can't open %s for write\n", argv[3]);
        return 4;
    }
    switch (*argv[1])
    {
    case 'e':
        // buffer length: 1M, window length: 64K, min 4 match chars
        lz_compress(20, 16, 4, fin, fout);
        break;
    case 'd':
        lz_decompress(fin, fout);
        break;
    default:
        info();
    }
}
