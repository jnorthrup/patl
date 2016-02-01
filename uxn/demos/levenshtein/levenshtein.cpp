/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Interactive demo of filtering words using Levenshtein distance
-*/
#include <fstream>
#include <uxn/patl/aux_/perf_timer.hpp>
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/levenshtein.hpp>
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/patricia_dot_creator.hpp>

namespace patl = uxn::patl;

template <typename T>
class forward_string
{
public:
    forward_string(): p_(0), s_(0) { }
    forward_string(const T *p, word_t s): p_(p), s_(s) { }

    bool operator==(const forward_string<T> &b) const { return p_ == b.p_; }
    bool operator!=(const forward_string<T> &b) const { return p_ != b.p_; }

    bool operator<(const forward_string<T> &b) const { return p_ < b.p_; }

    word_t length() const { return s_; }
    const T &operator[](int i) const { return p_[i]; }
    const T *c_str() const { return p_; }

protected:
    const T *p_;
    word_t s_;
};

template <typename T>
class backward_string
    : public forward_string<T>
{
public:
    backward_string(): forward_string() { }
    backward_string(const T *p, word_t s): forward_string(p, s) { }

    const T &operator[](int i) const { return p_[s_ - 1 - i]; }
};
/*
template <typename T>
struct backward_ptr
{
public:
    backward_ptr(const T *p)
        : p_(p)
    {
    }

    const T &operator[](int i) const { return p_[-i]; }

private:
    const T *p_;
};*/

namespace uxn
{
namespace patl
{

/// finite forward_string
template <typename T>
class bit_comparator<forward_string<T>, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;
    typedef forward_string<T> value_type;

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

/// finite backward_string
template <typename T>
class bit_comparator<backward_string<T>, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;
    typedef backward_string<T> value_type;

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
/*
/// standard finite std::vector
template <typename T>
class bit_comparator<backward_ptr<T>, 0>
    : public bit_comparator<T, 0>
{
    typedef bit_comparator<T, 0> super;
    typedef backward_ptr<T> value_type;

public:
    typedef T char_type;

    static const word_t bit_size = super::bit_size;

    word_t bit_length(const value_type&) const
    {
        return ~word_t(0);
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
        for (word_t i = skip / bit_size; ; ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
    }
};*/

} // namespace patl
} // namespace uxn

int main(int argc, char *argv[])
{
    typedef patl::trie_set<forward_string<char> > string_set;
    typedef patl::levenshtein_distance<string_set, false> leven_dist;
    typedef string_set::const_vertex const_vertex;
    //
    typedef patl::trie_set<backward_string<char> > back_string_set;
    typedef patl::levenshtein_distance<back_string_set, false> back_leven_dist;
    typedef back_string_set::const_vertex back_const_vertex;
    //
    std::ifstream fin(argc > 1 ? argv[1] : "WORD.LST");
    if (!fin.is_open())
    {
        printf("Unable to open input file!\n");
        return 0;
    }
    string_set dict;
    back_string_set back_dict;
    const char stop_bytes[] =
        "\x81\xAE\xA4\x11\xB6\x18\x1C\xA7"
        "\xF5\xE8\xF9\xD2\x7C\x2D\x0E\x3C";
    std::vector<char> words;
    std::vector<word_t> nulls;
    words.insert(words.end(), stop_bytes, stop_bytes + 16);
    nulls.push_back(words.size());
    words.push_back('\0');
    patl::aux::performance_timer tim;
    {
        std::string str;
        while (fin >> str)
        {
            words.insert(words.end(), str.begin(), str.end());
            nulls.push_back(words.size());
            words.push_back('\0');
        }
        words.insert(words.end(), stop_bytes, stop_bytes + 16);
    }
    {
        string_set::iterator hint(dict.end());
        for (word_t i = 0; i != nulls.size() - 1; ++i)
        {
            hint = dict.insert(
                hint,
                forward_string<char>(&words[nulls[i] + 1], nulls[i + 1] - nulls[i] - 1));
            if (patl::impl::max0(static_cast<const_vertex&>(hint).skip()) / 8 > hint->length() + 1)//nulls[i + 1] - nulls[i])
            {
                printf("hint:%s\n", &words[nulls[i] + 1]);
                dict.erase(hint);
                hint = dict.end();
            }
        }
    }
    {   // ускорить с помощью hint'а невозможно - слова сортируются с заду наперед
        for (word_t i = 0; i != nulls.size() - 1; ++i)
        {
            auto pib = back_dict.insert(
                backward_string<char>(&words[nulls[i] + 1], nulls[i + 1] - nulls[i] - 1));
            if (patl::impl::max0(static_cast<back_const_vertex&>(pib.first).skip()) / 8 > pib.first->length() + 1)//nulls[i] - nulls[i - 1])
                back_dict.erase(pib.first);
        }
    }
    tim.finish();
    printf("dict size: %u, loaded in %0.3f sec.\n", dict.size(), tim.get_seconds());
    //
    /*{
        const_vertex vtx(dict.root());
        vtx.mismatch("aa", 2 * 8);
        std::ofstream fout("aa_.dot");
        patl::patricia_dot_creator<string_set>(fout).create(vtx);
    }*/
    for (;;)
    {
        fseek(stdin, 0, SEEK_END);
        printf("Input dist & word: ");
        unsigned dist;
        char word[256] = "";
        if (scanf("%u %s", &dist, word) != 2 || !*word)
        {
            printf("\nBye.\n");
            break;
        }
        std::vector<std::pair<forward_string<char>, word_t> > strings;
        tim.start();
        const word_t len = strlen(word);
        const word_t len1 = len / 2, len2 = len - len1;
        if (!len1)
        {
            leven_dist ld(dict, dist, forward_string<char>(word, len));
            auto sim = dict.root().whole_sim(ld);
            for (sim.init(); !sim.the_end(); sim.next())
                strings.push_back(std::make_pair(sim.vertex().value(), sim.decis().distance()));
        }
        else
        {
            int dist1 = 0, dist2 = dist;
            {
                const_vertex vtx(dict.root());
                vtx.mismatch(forward_string<char>(word, len), 8 * len1);
                auto sim = vtx.whole_sim(leven_dist(dict, dist2, forward_string<char>(word + len1, len2)), len1);
                for (sim.init(); !sim.the_end(); sim.next())
                    strings.push_back(std::make_pair(sim.vertex().value(), sim.decis().distance()));
                ++dist1; --dist2;
            }
            if (dist)
            {
                for (; dist2 >= dist1; ++dist1, --dist2)
                {
                    leven_dist
                        ld1(dict, dist1, forward_string<char>(word, len1))/*,
                        ld2(dict, dist2, forward_string<char>(word + len1, len2))*/;
#if 0
                    printf("ld1: '");
                    for (word_t i = 0; i != ld1.mask().length(); ++i)
                        printf("%c", ld1.mask()[i]);
                    printf("'\n");
                    printf("ld2: '");
                    for (word_t i = 0; i != ld2.mask().length(); ++i)
                        printf("%c", ld2.mask()[i]);
                    printf("'\n");
#endif
                    printf("len1 %u:", len1);
                    auto sim1 = dict.root().narrow_sim(ld1/*, len1*/);
                    for (sim1.init(); !sim1.the_end(); sim1.next())
                    {
                        printf(" %s:%u", std::string(sim1.vertex().value().c_str(), sim1.matched()).c_str(), sim1.decis().distance());
                        leven_dist ld2(dict, dist - sim1.decis().distance(), forward_string<char>(word + len1, len2));
                        auto sim2 = sim1.vertex().whole_sim(ld2, /*len1*/sim1.matched());
                        for (sim2.init(); !sim2.the_end(); sim2.next())
                            strings.push_back(std::make_pair(
                                sim2.vertex().value(),
                                sim1.decis().distance() + sim2.decis().distance()));
                    }
                    printf("\n");
                }
                for (; dist2 >= 0; ++dist1, --dist2)
                {
                    back_leven_dist
                        ld1(back_dict, dist2, backward_string<char>(word + len1, len2))/*,
                        ld2(back_dict, dist1, backward_string<char>(word, len1))*/;
#if 0
                    printf("ld1: '");
                    for (word_t i = 0; i != ld1.mask().length(); ++i)
                        printf("%c", ld1.mask()[i]);
                    printf("'\n");
                    printf("ld2: '");
                    for (word_t i = 0; i != ld2.mask().length(); ++i)
                        printf("%c", ld2.mask()[i]);
                    printf("'\n");
#endif
                    printf("len2 %u:", len2);
                    auto sim1 = back_dict.root().narrow_sim(ld1/*, len2*/);
                    for (sim1.init(); !sim1.the_end(); sim1.next())
                    {
                        printf(" %s:%u", std::string(sim1.vertex().value().c_str(), sim1.matched()).c_str(), sim1.decis().distance());
                        back_leven_dist ld2(back_dict, dist - sim1.decis().distance(), backward_string<char>(word, len1));
                        auto sim2 = sim1.vertex().whole_sim(ld2, /*len2*/sim1.matched());
                        for (sim2.init(); !sim2.the_end(); sim2.next())
                            strings.push_back(std::make_pair(
                                sim2.vertex().value(),
                                sim1.decis().distance() + sim2.decis().distance()));
                    }
                    printf("\n");
                }
                std::sort(strings.begin(), strings.end());
                strings.resize(std::unique(strings.begin(), strings.end()) - strings.begin());
            }
        }
        tim.finish();
        for (word_t i = 0; i != strings.size(); ++i)
            printf("%s:%u ", strings[i].first.c_str(), strings[i].second);
        printf("\n%0.3f msec.\n", tim.get_seconds() * 1000.0);
#if 1
        {
            leven_dist ld(dict, dist, forward_string<char>(word, len));
            auto sim = dict.root().whole_sim(ld);
            for (sim.init(); !sim.the_end(); sim.next())
                printf("%s:%u ", sim.vertex().value().c_str(), sim.decis().distance());
            printf("\n");
        }
#endif
#if 0
        back_leven_dist ld(back_dict, dist, word + len - 1, len);
        tim.start();
        /*string_set::const_partimator<leven_dist>
            beg(dict.begin(ld)),
            end(dict.end(ld)),
            it(beg);
        for (; it != end; ++it) ;*/
        back_const_vertex vtx = back_dict.root();
        //vtx.mismatch("patriarch", 5 * 8);
        auto sim = vtx.whole_sim(ld);
        for (sim.init(); !sim.the_end(); sim.next()) ;
        tim.finish();
        //for (it = beg; it != end; ++it)
        for (sim.init(); !sim.the_end(); sim.next())
        {
            printf("%u(matched %u):", sim.decis().distance(), sim.matched());
            back_string_set::const_iterator
                beg2(sim.vertex().begin()),
                end2(sim.vertex().end());
            printf("\t");
            for (back_string_set::const_iterator it2(beg2); it2 != end2; ++it2)
            {
                std::string s;
                for (int i = 0; (*it2)[i]; ++i)
                    s.push_back((*it2)[i]);
                std::reverse(s.begin(), s.end());
                printf("%s ", s.c_str());
                //if (it.prefix() == it2->size())
                    //printf("%s ", *it2/*->c_str()*/);
            }
            //printf("%s ", sim.vertex().key().c_str());
            printf("\n");
        }
        printf("\n%0.3f msec.\n", tim.get_seconds() * 1000.0);
#endif
    }
}
