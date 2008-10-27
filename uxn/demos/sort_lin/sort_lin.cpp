#include <uxn/patl/aux_/file_utils.hpp>
#include <uxn/patl/aux_/buffered_io.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>
#include <uxn/patl/trie_set.hpp>

namespace patl = uxn::patl;

template <typename T>
class piece
{
public:
    piece(const T *begin, const T *end)
        : begin_(begin)
        , end_(end)
    {
    }

    word_t length() const
    {
        return end_ - begin_;
    }

    const T &operator[](sword_t off) const
    {
        return begin_[off];
    }

    T &operator[](sword_t off)
    {
        return begin_[off];
    }

    const T *begin() const
    {
        return begin_;
    }

    const T *end() const
    {
        return end_;
    }

private:
    const T *begin_, *end_;
};

namespace uxn
{
namespace patl
{

/// bit-comparator for piece<T>
template <typename T>
class bit_comparator<piece<T> >
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;
    typedef piece<T> value_type;

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

} // namespace patl
} // namespace uxn

void sort_huge_text_file(char *infname, char *outfname)
{
    const HANDLE fh = CreateFile(
        infname, GENERIC_READ, FILE_SHARE_READ, 0,
        OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
    if (fh == INVALID_HANDLE_VALUE)
        return;
    const unsigned N = patl::aux::get_file_length(fh);
    const HANDLE fmh = CreateFileMapping(fh, 0, PAGE_READONLY, 0, N, 0);
    if (!fmh)
        return;
    unsigned char * const huge = static_cast<unsigned char*>(
        MapViewOfFile(fmh, FILE_MAP_READ, 0, 0, N));
    const unsigned char * const end = huge + N;
    if (!huge)
        return;
    typedef piece<unsigned char> textline_t;
    typedef patl::trie_set<textline_t> trie_t;
    trie_t trie;
    bool newline = false;
    word_t count = 0;
    unsigned char
        *cur = huge,
        *curbeg = huge;
    patl::aux::performance_timer tim;
    for (; cur != end; ++cur)
    {
        if (newline)
        {
            ++count;
            trie.insert(textline_t(curbeg, cur));
            curbeg = cur;
        }
        newline = *cur == '\n';
    }
    trie.insert(piece<unsigned char>(curbeg, cur));
    tim.finish();
    printf("read lines: %u\nunique lines: %u\ntimer: %.2f\n",
        count,
        trie.size(),
        tim.get_seconds());
    //
    {
        patl::aux::buffered_output outbuf(outfname);
        for (trie_t::const_iterator it = trie.begin(); it != trie.end(); ++it)
            outbuf.write_bytes(it->begin(), it->length());
    }
    //
    UnmapViewOfFile(huge);
    CloseHandle(fmh);
    CloseHandle(fh);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("SORT_LIN <infile> <outfile>\n");
        return 0;
    }
    sort_huge_text_file(argv[1], argv[2]);
}
