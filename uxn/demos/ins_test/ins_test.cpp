/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Insertion methods test
-*/
#include <fstream>
#include <vector>
#include <algorithm>
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>
#include <uxn/patl/aux_/static_alloc.hpp>

namespace patl = uxn::patl;

const word_t N = 4;

template <typename Iter>
void insertions(Iter first, Iter last)
{
    typedef patl::trie_set<
        std::string,
        0,
        patl::bit_comparator<std::string, 0>,
        patl::aux::static_alloc<std::string> > string_set;
    patl::aux::performance_timer tim;
    double min_tim = 0.0;
    //
    const word_t pool_size =
        (5 * sizeof(word_t) + sizeof(std::string)) * std::distance(first, last);
    unsigned char
        *pool_0 = new unsigned char [2 * pool_size],
        *pool_1 = pool_0 + pool_size;
    //
    {
        string_set dict_0;
        printf("Casual insertion method... ");
        for (word_t n = 0; n != N; ++n)
        {
            dict_0.clear();
            patl::aux::static_alloc_state::init(pool_0, pool_size);
            tim.start();
            for (Iter it(first); it != last; ++it)
                dict_0.insert(*it);
            tim.finish();
            if (!n || tim.get_seconds() < min_tim)
                min_tim = tim.get_seconds();
        }
        printf("%0.4f sec.\n", min_tim);
        //
        string_set dict_1;
        printf("Hinted insertion method... ");
        for (word_t n = 0; n != N; ++n)
        {
            dict_1.clear();
            patl::aux::static_alloc_state::init(pool_1, pool_size);
            tim.start();
            string_set::iterator hint(dict_1.end());
            for (Iter it(first); it != last; ++it)
                hint = dict_1.insert(hint, *it);
            tim.finish();
            if (!n || tim.get_seconds() < min_tim)
                min_tim = tim.get_seconds();
        }
        printf("%0.4f sec.\n", min_tim);
        //
        printf("Compare: %s\n", dict_0 == dict_1 ? "identical" : "mismatched");
    }
    //
    delete[] pool_0;
}

int main(int argc, char *argv[])
{
    std::ifstream fin(argc > 1 ? argv[1] : "WORD.LST");
    if (!fin.is_open())
    {
        printf("Unable to open input file!\n");
        return 0;
    }
    std::vector<std::string> dict_vec;
    std::string str;
    while (fin >> str)
        dict_vec.push_back(str);
    printf("dict_vec size: %u\n", dict_vec.size());
    //
    printf("\n\n--- raw data\n\n");
    insertions(dict_vec.begin(), dict_vec.end());
    //
    printf("\n\n--- sorted data\n\n");
    std::sort(dict_vec.begin(), dict_vec.end());
    insertions(dict_vec.begin(), dict_vec.end());
    //
    printf("\n\n--- shuffled data\n\n");
    std::random_shuffle(dict_vec.begin(), dict_vec.end());
    insertions(dict_vec.begin(), dict_vec.end());
}
