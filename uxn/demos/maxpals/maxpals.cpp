/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Finding all maximal palindromes in linear time with and without suffix tree & O(1) LCA
 | ---
 | были найдены простые алгоритмы нахождени€ максимальных палиндромов,
 | без построени€ суффиксного дерева и структур дл€ нахождени€ LCA за O(1)
 | http://e-maxx.ru/algo/palindromes_count
-*/
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/lca_oracle.hpp>
#include <uxn/patl/leaf_oracle.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>

namespace patl = uxn::patl;

typedef patl::suffix_set<char*> SuffixSet;
typedef SuffixSet::vertex vertex;

int main()
{
    char pals[1024] =
        "mississippiarozaupalanalapuazoraaaaasobakabosavjfhdlomosmokinggnikomsomolsfsdlfhbvhamanaplanacanalpanamajfgjbf4603974fg4u";
    const int n = strlen(pals);
    strcpy(pals + n + 1, pals);
    _strrev(pals + n + 1);
    //
    SuffixSet sufs(pals, 2 * n + 1);
    printf("building suffix tree...");
    for (int i = 0; i != 2 * n + 1; ++i)
        sufs.push_back();
    printf("done!\n");
    printf("lca preprocessing...");
    patl::lca_oracle<SuffixSet> lca(sufs);
    printf("done!\n");
    printf("leaf preprocessing...");
    patl::leaf_oracle<SuffixSet> leaf(sufs);
    printf("done!\n");
    //
    printf("---\n");
    for (int i = 1; i != n; ++i)
    {
        const vertex v = lca(
            leaf(pals + i),
            leaf(pals + 2 * n + 1 - i));
        const int q = v.prefix_length() / 8;
        if (q)
            printf("% 4d %s\n", i, std::string(pals + i - q, 2 * q).c_str());
    }
    {
        printf("***\n");
        std::vector<int> d(n);
        int l = 0, r = -1;
        for (int i = 0; i < n; ++i)
        {
            int k = (i > r ? 0 : patl::impl::get_min(d[l + r - i + 1], r - i + 1)) + 1;
            while (i + k - 1 < n && i - k >= 0 && pals[i + k - 1] == pals[i - k])
                ++k;
            d[i] = --k;
            if (i + k - 1 > r)
            {
                l = i - k;
                r = i + k - 1;
            }
            if (d[i])
                printf("% 4d %s\n", i, std::string(pals + i - d[i], 2 * d[i]).c_str());
        }
    }
    //
    printf("---\n");
    for (int i = 1; i != n; ++i)
    {
        const vertex v = lca(
            leaf(pals + i),
            leaf(pals + 2 * n - i));
        const int q = v.prefix_length() / 8 - 1;
        if (q)
            printf("% 4d %s\n", i, std::string(pals + i - q, 2 * q + 1).c_str());
    }
    {
        printf("***\n");
        std::vector<int> d(n);
        int l = 0, r = -1;
        for (int i = 0; i != n; ++i)
        {
            int k = (i > r ? 0 : patl::impl::get_min(d[l + r - i], r - i)) + 1;
            while (i + k < n && i - k >= 0 && pals[i + k] == pals[i - k])
                ++k;
            d[i] = --k;
            if (i + k > r)
            {
                l = i - k;
                r = i + k;
            }
            if (d[i])
                printf("% 4d %s\n", i, std::string(pals + i - d[i], 2 * d[i] + 1).c_str());
        }
    }
}
