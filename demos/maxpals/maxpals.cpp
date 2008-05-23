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
        "mississippiarozaupalanalapuazoraaaaasobakabosavjfhdsfsdlfhbvhamanaplanacanalpanamajfgjbf4603974fg4u";
    const unsigned n = strlen(pals);
    strcpy(pals + n + 1, pals);
    _strrev(pals + n + 1);
    //
    SuffixSet sufs(pals, 2 * n + 1);
    printf("building suffix tree...");
    for (unsigned i = 0; i != 2 * n + 1; ++i)
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
    for (unsigned i = 1; i != n; ++i)
    {
        const vertex v = lca(
            leaf(pals + i),
            leaf(pals + 2 * n + 1 - i));
        const unsigned q = v.prefix_length() / 8;
        if (q)
            printf("% 4d %s\n", i, std::string(pals + i - q, 2 * q).c_str());
    }
    //
    printf("---\n");
    for (unsigned i = 1; i != n; ++i)
    {
        const vertex v = lca(
            leaf(pals + i),
            leaf(pals + 2 * n - i));
        const unsigned q = v.prefix_length() / 8 - 1;
        if (q)
            printf("% 4d %s\n", i, std::string(pals + i - q, 2 * q + 1).c_str());
    }
}
