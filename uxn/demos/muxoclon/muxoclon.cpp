/**
* fly-to-elephant transform program
* http://community.livejournal.com/coding4fun_ru/1510.html
* examples:
*   hit ace (length 4)
*   pig sty (6)
*   four five (7)
*   play game (8)
*   green grass (5)
*   wheat bread (7)
*   order chaos (12)
*   sixth hubby (10)
*   speedy comedy (19)
*   chasing robbers (20)
*   griming goblets (23)
*   effaces cabaret (50)
*/

// C4503: decorated name length exceeded, name was truncated
#pragma warning(disable : 4503)

#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/partial.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>

namespace patl = uxn::patl;

typedef patl::trie_set<std::string> trie_string;
typedef std::map<unsigned, trie_string> map_sized_string;
typedef trie_string::vertex vertex;
typedef std::vector<vertex> vector_vertex;

typedef std::map<vertex, vertex> map_vertex;
typedef std::vector<map_vertex> vector_wave;

void directed_search(
    const trie_string &dict,
    vector_wave &wave,
    vector_wave &antiwave,
    std::set<vertex> &used,
    bool reversed,
    vector_vertex &chain)
{
    wave.push_back(map_vertex());
    map_vertex &next = wave.back();
    const map_vertex
        &current = wave[wave.size() - 2],
        &antipode = antiwave.back();
    for (map_vertex::const_iterator it = current.begin()
        ; it != current.end()
        ; ++it)
    {
        const vertex &seed = it->first;
        typedef patl::hamming_distance<trie_string, true> hamm_dist;
        hamm_dist hd(dict, 1, seed.key());
        trie_string::const_partimator<hamm_dist>
            pmi = dict.begin(hd),
            pmi_end = dict.end(hd);
        for (; pmi != pmi_end; ++pmi)
        {
            const vertex &vtx = pmi;
            if (used.find(vtx) == used.end())
            {
                const map_vertex::const_iterator match = antipode.find(vtx);
                if (match != antipode.end())
                {
                    wave.pop_back();
                    vertex cur = reversed ? vtx : seed;
                    vector_wave
                        &front_wave = reversed ? antiwave : wave,
                        &back_wave = reversed ? wave : antiwave;
                    // G++ not allowed to use const_reverse_iterator
                    // error: no match for 'operator!='
                    for (vector_wave::reverse_iterator rit = front_wave.rbegin()
                        ; rit != front_wave.rend()
                        ; cur = rit++->find(cur)->second)
                        chain.push_back(cur);
                    std::reverse(chain.begin(), chain.end());
                    cur = reversed ? seed : vtx;
                    for (vector_wave::reverse_iterator rit = back_wave.rbegin()
                        ; rit != back_wave.rend()
                        ; cur = rit++->find(cur)->second)
                        chain.push_back(cur);
                    return;
                }
                used.insert(vtx);
                next.insert(std::make_pair(vtx, seed));
            }
        }
    }
}

bool search_work(
    const trie_string &dict,
    const vertex &src_vtx,
    const vertex &dst_vtx,
    vector_vertex &chain)
{
    if (src_vtx == dst_vtx)
    {
        chain.push_back(src_vtx);
        return true;
    }
    const vertex dict_end = dict.end();
    vector_wave front_wave, back_wave;
    front_wave.push_back(map_vertex());
    front_wave.back().insert(std::make_pair(src_vtx, dict_end));
    back_wave.push_back(map_vertex());
    back_wave.back().insert(std::make_pair(dst_vtx, dict_end));
    std::set<vertex> front_used, back_used;
    front_used.insert(src_vtx);
    back_used.insert(dst_vtx);
    for (int level = 0; ; ++level)
    {
        // front
        directed_search(dict, front_wave, back_wave, front_used, false, chain);
        if (!chain.empty())
            return true;
        if (front_wave.back().empty())
            return false;
        // back
        directed_search(dict, back_wave, front_wave, back_used, true, chain);
        if (!chain.empty())
            return true;
        if (back_wave.back().empty())
            return false;
    }
}

int main(int argc, char *argv[])
{
    std::ifstream fin(argc > 1 ? argv[1] : "WORD.LST");
    if (!fin.is_open())
    {
        printf("Unable to open input file!\n");
        return 0;
    }
    map_sized_string sdict;
    std::string str;
    while (fin >> str)
        sdict[str.length()].insert(str);
    for (;;)
    {
        printf("Input two words: ");
        char word0[256] = "", word1[256] = "";
        scanf("%s %s", word0, word1);
        if (!*word0 || !*word1)
        {
            printf("\nBye.\n");
            break;
        }
        if (strlen(word0) != strlen(word1))
        {
            printf("Words must be equal in length!\n");
            continue;
        }
        const trie_string &dict = sdict[strlen(word0)];
        trie_string::const_iterator
            src_it = dict.find(word0),
            dst_it = dict.find(word1);
        if (src_it == dict.end() || dst_it == dict.end())
        {
            printf("Both words must be in dictionary!\n");
            continue;
        }
        printf("Transform: '%s' ==> '%s'\n", word0, word1);
        vector_vertex chain;
        patl::aux::performance_timer tim;
        if (search_work(dict, src_it, dst_it, chain))
        {
            for (vector_vertex::const_iterator it = chain.begin()
                ; it != chain.end()
                ; ++it)
                printf("%s ", it->key().c_str());
            printf("\n");
        }
        else
            printf("Chain not found!\n");
        tim.finish();
        printf("time: %.2f sec.\n", tim.get_seconds());
    }
}
