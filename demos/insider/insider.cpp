#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/maxrep_iterator.hpp>
#include <uxn/patl/super_maxrep_iterator.hpp>
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/partial.hpp>

namespace patl = uxn::patl;

template <typename Iter>
void printRegexp(word_t length, const Iter &beg, const Iter &end)
{
	for (Iter cur = beg; cur != end; ++cur)
	{
		if (beg != cur)
			printf("|");
		if (cur->leaf())
			printf("%s", cur->key().substr(length).c_str());
		else
		{
            const word_t
                next_len = cur->next_skip() / 8,
                bit_len = 8 * (next_len + 1);
			printf("%s", cur->key().substr(length, next_len - length).c_str());
			Iter
                it1 = cur->levelorder_begin(bit_len),
                it2 = cur->levelorder_end(bit_len);
			printf("(");
			const bool question = it1->key().size() == next_len;
			if (question)
				++it1;
            printRegexp(next_len, it1, it2);
			printf(")");
			if (question)
				printf("?");
		}
	}
}

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
    {
        typedef StringSet::const_iterator const_iterator;
        typedef std::pair<const_iterator, const_iterator> pair_cit_cit;
        pair_cit_cit pcc = test1.equal_range("balda");
        const_iterator
            low_cit = test1.lower_bound("balda"),
            upp_cit = test1.upper_bound("balda");
        printf("equal == <lower, upper>: %s\n",
            pcc == std::make_pair(low_cit, upp_cit) ? "true" : "false");
        printf("range of 'balda' (first: %s, limit: %s):\n",
            pcc.first != test1.end() ? pcc.first->c_str() : "end",
            pcc.second != test1.end() ? pcc.second->c_str() : "end");
        for (const_iterator cit = pcc.first; cit != pcc.second; ++cit)
            printf("\t%s\n", cit->c_str());
        //
        printf("\n---\n\n");
        //
        pcc = test1.equal_range("balda", 3 * 8);
        low_cit = test1.lower_bound("balda", 3 * 8);
        upp_cit = test1.upper_bound("balda", 3 * 8);
        printf("equal == <lower, upper>: %s\n",
            pcc == std::make_pair(low_cit, upp_cit) ? "true" : "false");
        printf("range of 'balda' [3] (first: %s, limit: %s):\n",
            pcc.first != test1.end() ? pcc.first->c_str() : "end",
            pcc.second != test1.end() ? pcc.second->c_str() : "end");
        for (const_iterator cit = pcc.first; cit != pcc.second; ++cit)
            printf("\t%s\n", cit->c_str());
    }
    //
    printf("\n--- iterator\n\n");
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
    printf("\n--- partial_match\n\n");
    //
    {
        typedef patl::partial_match<StringSet> part_match;
        part_match pm(test1, "b?l?t");
        StringSet::const_partimator<part_match>
            it = test1.begin(pm),
            itEnd = test1.end(pm);
        printf("*** 'b?l?t':\n");
        for (; it != itEnd; ++it)
            printf("%s\n", it->c_str());
        printf("---\n");
        pm = part_match(test1, "b?l??");
        it = test1.begin(pm);
        itEnd = test1.end(pm);
        printf("*** 'b?l??\':\n");
        for (; it != itEnd; ++it)
            printf("%s\n", it->c_str());
    }
    //
    printf("\n--- hamming_distance\n\n");
    //
    {
        typedef patl::hamming_distance<StringSet> hamm_dist;
        hamm_dist hd(test1, "balda", 2);
        StringSet::const_partimator<hamm_dist>
            it = test1.begin(hd),
            itEnd = test1.end(hd);
        printf("*** 'balda', dist == 2:\n");
        for (; it != itEnd; ++it)
            printf("%s\n", it->c_str());
    }
    //
    printf("\n--- postorder_iterator\n\n");
    //
    typedef StringSet::vertex vertex;
    const vertex vtx_root = test1.root();
    {
        typedef StringSet::postorder_iterator postorder_iterator;
        postorder_iterator
            itBeg = vtx_root.postorder_begin(),
            itEnd = vtx_root.postorder_end(),
            it = itBeg;
        for (; it != itEnd; ++it)
            printf("%d\t%s\n", it->skip(), it->key().c_str());
        printf("---\n");
        while (it != itBeg)
        {
            --it;
            printf("%d\t%s\n", it->skip(), it->key().c_str());
        }
    }
    //
    printf("\n--- preorder_iterator\n\n");
    //
    {
        typedef StringSet::preorder_iterator preorder_iterator;
        preorder_iterator
            itBeg = vtx_root.preorder_begin(),
            itEnd = vtx_root.preorder_end(),
            it = itBeg;
        for (; it != itEnd; ++it)
            printf("%d\t%s\n", it->skip(), it->key().c_str());
        printf("---\n");
        while (it != itBeg)
        {
            --it;
            printf("%d\t%s\n", it->skip(), it->key().c_str());
        }
    }
    //
    printf("\n--- levelorder_iterator\n\n");
    //
    {
        const char *const X[] = {
            "asm", "auto", "bool", "break", "case", "catch", "char", "class", "const", 
            "const_cast", "continue", "default", "delete", "do", "double", 
            "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false",
            "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable",
            "namespace", "new", "operator", "private", "protected", "public", 
            "register", "reinterpret_cast", "return", "short", "signed", "sizeof",
            "static", "static_cast", "struct", "switch", "template", "this", "throw",
            "true", "try", "typedef", "typeid", "typename", "union", "unsigned", 
            "using", "virtual", "void", "volatile", "wchar_t", "while"};

            typedef patl::trie_set<std::string> ReservSet;
            typedef ReservSet::vertex vertex;
            ReservSet rvset(X, X + sizeof(X) / sizeof(X[0]));

            printf("*** Regexp:\n");
            const vertex vtx = rvset.root();
            printRegexp(0, vtx.levelorder_begin(8), vtx.levelorder_end(8));
            printf("\n");
    }
    //
    printf("\n--- [super_]maxrep_iterator\n\n");
    //
    {
        typedef patl::suffix_set<char*> SuffixSet;
        char str[] =
            //"abrakadabraa";
            "xabcyiiizabcqabcyr";
            //"cxxaxxaxxb";
            //"How many wood would a woodchuck chuck.";
            //"xgtcacaytgtgacz";
        printf("*** string: '%s':\n", str);
        SuffixSet suffix(str);
        for (unsigned i = 0; i != sizeof(str) - 1; ++i)
            suffix.push_back();
        for (uxn::patl::maxrep_iterator<SuffixSet> mrit(&suffix)
            ; !mrit->is_root()
            ; ++mrit)
            printf("'%s' x %d\n",
            std::string(mrit->key(), mrit->length()).c_str(),
            mrit.freq());
        printf("---\n");
        for (uxn::patl::super_maxrep_iterator<SuffixSet> mrit(&suffix)
            ; !mrit->is_root()
            ; ++mrit)
            printf("'%s' x %d\n",
            std::string(mrit->key(), mrit->length()).c_str(),
            mrit.freq());
    }
    //
    printf("\n--- search tandem repeats in O(n)\n\n");
    //
    {
        //typedef int typ;
        typedef char typ;
        typedef patl::suffix_set<const typ*> suffix_t;
        //typ arr[] = {10, 5, 6, 7, 5, 6, 7, 89, 64};
        typ arr[] = "qweabrabrabraad";
        //typ arr[] = "qweabrabrrrrd";
        printf("*** string: '%s':\n", arr);
        suffix_t suf(arr, sizeof(arr) / sizeof(arr[0]));
        const typ *repend = arr;
        do
        {
            const suffix_t::vertex
                vtx = suf.push_back(),
                sibl = vtx.sibling();
            if (suf.size() > 1 && suf.keys() + suf.size() > repend)
            {
                const int skip = vtx.skip() / 8 / sizeof(typ);
                const unsigned
                    delta = vtx.key() - sibl.key(),
                    count = skip / delta + 1;
                if (count > 1)
                {
                    printf("begin: %u, length: %u, count: %u\n",
                        sibl.key() - suf.keys(),
                        delta,
                        count);
                    repend = sibl.key() + delta * count + 1;
                }
            }
        } while (!suf.endpoint());
    }
}
