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

namespace patl = uxn::patl;

int main(int argc, char *argv[])
{
    typedef patl::trie_set<std::string> string_set;
    typedef patl::levenshtein_distance<string_set, true> leven_dist;
    //
    std::ifstream fin(argc > 1 ? argv[1] : "WORD.LST");
    if (!fin.is_open())
    {
        printf("Unable to open input file!\n");
        return 0;
    }
    string_set dict;
    patl::aux::performance_timer tim;
    {
        std::string str;
        string_set::iterator hint(dict.end());
        while (fin >> str)
            hint = dict.insert(hint, str);
    }
    tim.finish();
    printf("dict size: %u, loaded in %0.3f sec.\n", dict.size(), tim.get_seconds());
    //
    for (;;)
    {
        fseek(stdin, 0, SEEK_END);
        printf("Input dist & word: ");
        unsigned  dist;
        char word[256] = "";
        if (scanf("%u %s", &dist, word) != 2 || !*word)
        {
            printf("\nBye.\n");
            break;
        }
        leven_dist ld(dict, dist, word);
        tim.start();
        string_set::const_partimator<leven_dist>
            beg(dict.begin(ld)),
            end(dict.end(ld)),
            it(beg);
        for (; it != end; ++it) ;
        tim.finish();
        for (it = beg; it != end; ++it)
            printf("%s:%u ", it->c_str(), it.decis().distance());
        printf("\n%0.3f msec.\n", tim.get_seconds() * 1000.0);
    }
}
