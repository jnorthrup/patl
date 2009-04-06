/*-
 | Interactive demo of filtering words using Levenshtein distance
-*/

#include <fstream>
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
    std::string str;
    while (fin >> str)
        dict.insert(str);
    printf("dict size: %u\n", dict.size());
    //
    for (;;)
    {
        printf("Input dist & word: ");
        unsigned  dist;
        char word[256] = "";
        scanf("%u %s", &dist, word);
        if (!*word)
        {
            printf("\nBye.\n");
            break;
        }
        leven_dist ld(dict, dist, word);
        string_set::const_partimator<leven_dist>
            it = dict.begin(ld),
            end = dict.end(ld);
        for (; it != end; ++it)
            printf("%s:%u ", it->c_str(), it.decis().distance());
        printf("\n");
    }
}
