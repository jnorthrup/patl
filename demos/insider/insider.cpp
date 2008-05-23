#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/maxrep_iterator.hpp>
#include <uxn/patl/super_maxrep_iterator.hpp>
#include <uxn/patl/suffix_set.hpp>

namespace patl = uxn::patl;

int main()
{
    typedef patl::trie_set<std::string> StringSet;
    StringSet
        test1,
        test2;
    //&test2 = test1;
    //
    test1.insert("balka");
    test1.insert("balet");
    test1.insert("bulat");
    test1.insert("bulka");
    test1.insert("bal");
    //
    test2.insert("balet");
    test2.insert("balon");
    test2.insert("bal");
    test2.insert("baton");
    //
    test1.merge(test2.begin(), test2.end(), StringSet::void_merge_handler());
    test1.change_root(test1.find("balon"));
    //
    typedef StringSet::vertex vertex;
    {
        vertex
            itBeg = test1.postorder_begin(),
            itEnd = test1.postorder_end(),
            it = itBeg;
        for (; it != itEnd; it.postorder_incr())
            printf("%d\t%s\n", it.skip(), it.key().c_str());
        printf("---\n");
        while (it != itBeg)
        {
            it.postorder_decr();
            printf("%d\t%s\n", it.skip(), it.key().c_str());
        }
    }
    //
    printf("\n---\n\n");
    //
    {
        vertex
            itBeg = test1.preorder_begin(),
            itEnd = test1.preorder_end(),
            it = itBeg;
        for (; it != itEnd; it.preorder_incr())
            printf("%d\t%s\n", it.skip(), it.key().c_str());
        printf("---\n");
        while (it != itBeg)
        {
            it.preorder_decr();
            printf("%d\t%s\n", it.skip(), it.key().c_str());
        }
    }
    //
    printf("\n---\n\n");
    //
    {
        typedef StringSet::const_iterator const_iter;
        const_iter
            itBeg = test1.begin(),
            itEnd = test1.end(),
            it = itBeg;
        for (; it != itEnd; ++it)
            printf("%s\n", it->c_str());
        printf("---\n");
        while (it != itBeg)
        {
            --it;
            printf("%s\n", it->c_str());
        }
    }
    //
    typedef patl::suffix_set<char*> SuffixSet;
    char str[] =
        //"abrakadabraa";
        //"xabcyiiizabcqabcyr";
        //"cxxaxxaxxb";
        "How many wood would a woodchuck chuck.";
    //"xgtcacaytgtgacz";
    SuffixSet suffix(str);
    for (unsigned i = 0; i != sizeof(str) - 1; ++i)
        suffix.push_back();
    for (
        uxn::patl::super_maxrep_iterator<SuffixSet> mrit(&suffix);
        !mrit->is_root();
        ++mrit)
        printf("\"%s\" x %d\n",
            std::string(mrit->key(), mrit->length()).c_str(),
            mrit.freq());
}
