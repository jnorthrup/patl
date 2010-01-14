/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Simple demonstration of basic PATL features
-*/
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/maxrep_iterator.hpp>
#include <uxn/patl/super_maxrep_iterator.hpp>
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/partial.hpp>
#include <uxn/patl/levenshtein.hpp>
#include <uxn/patl/patricia_dot_creator.hpp>
#include <iostream>
#include <fstream>

// C4503: decorated name length exceeded, name was truncated
#pragma warning(disable : 4503)

namespace patl = uxn::patl;

template <typename Iter>
void print_regexp(word_t length, const Iter &beg, const Iter &end)
{
    const word_t limit = (length + 1) * 8;
    for (Iter cur = beg; cur != end; cur.increment(limit))
    {
        if (!cur->limited(limit))
            continue;
        //
        if (beg != cur)
            printf("|");
        if (cur->get_qtag())
            printf("%s", cur->key().substr(length).c_str());
        else
        {
            const word_t next_len = cur->next_skip() / 8;
            printf("%s", cur->key().substr(length, next_len - length).c_str());
            Iter
                it1 = cur->preorder_begin((next_len + 1) * 8),
                it2 = cur->preorder_end();
            printf("(");
            const bool question = it1->key().size() == next_len;
            if (question)
                ++it1;
            print_regexp(next_len, it1, it2);
            printf(")");
            if (question)
                printf("?");
        }
    }
}

template <typename Vertex>
struct preorder_iterator_callback
    : public std::unary_function<Vertex, void>
{
    void operator()(const Vertex *vtx) const
    {
        printf("// %d\t%s\n", vtx->skip(), vtx->key().c_str());
    }
};

int main(/*int argc, char *argv[]*/)
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
    test1.merge(test2.begin(), test2.end());
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
        typedef patl::partial_match<StringSet, false> part_match;
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
        typedef patl::hamming_distance<StringSet, false> hamm_dist;
        hamm_dist hd(test1, 1, "bulk");
        StringSet::const_partimator<hamm_dist>
            it = test1.begin(hd),
            itEnd = test1.end(hd);
        printf("*** 'bulk', dist == 1:\n");
        for (; it != itEnd; ++it)
            printf("%s, dist: %u\n", it->c_str(), it.decis().distance());
    }
    //
    printf("\n--- levenshtein_distance\n\n");
    //
    {
        typedef patl::levenshtein_distance<StringSet, false> leven_dist;
        leven_dist ld(test1, 1, "balt");
        StringSet::const_partimator<leven_dist>
            it = test1.begin(ld),
            itEnd = test1.end(ld);
        printf("*** 'balt', dist == 1:\n");
        for (; it != itEnd; ++it)
            printf("%s, dist: %u\n", it->c_str(), it.decis().distance());
    }
    //
    printf("\n--- postorder_iterator\n\n");
    //
    typedef StringSet::const_vertex const_vertex;
    const const_vertex vtx_root = test1.root();
    {
        typedef StringSet::const_postorder_iterator const_postorder_iterator;
        const_postorder_iterator
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
        typedef StringSet::const_preorder_iterator const_preorder_iterator;
        const_preorder_iterator
            itBeg = vtx_root.preorder_begin(),
            itEnd = vtx_root.preorder_end(),
            it = itBeg;
        preorder_iterator_callback<typename StringSet::const_vertex> icb;
        for (; it != itEnd; /*++it*/it.increment(icb))
            printf("%d\t%s\n", it->skip(), it->key().c_str());
        printf("---\n");
        while (it != itBeg)
        {
            //--it;
            it.decrement(icb);
            printf("%d\t%s\n", it->skip(), it->key().c_str());
        }
    }
    //
    printf("\n--- preorder_iterator with levelorder behavior\n\n");
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
            //
            typedef patl::trie_set<std::string> ReservSet;
            typedef ReservSet::const_vertex const_vertex;
            const ReservSet rvset(X, X + sizeof(X) / sizeof(X[0]));
            //
            printf("*** Regexp:\n");
            const_vertex vtx = rvset.root();
            print_regexp(0, vtx.preorder_begin(8), vtx.preorder_end());
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
        for (word_t i = 0; i != sizeof(str) - 1; ++i)
            suffix.push_back();
        for (uxn::patl::maxrep_iterator<SuffixSet> mrit(&suffix)
            ; !mrit->is_root()
            ; ++mrit)
        {
            printf("'%s' x %d:",
                std::string(mrit->key(), mrit->length()).c_str(),
                mrit.freq());
            const SuffixSet::const_vertex vtx = mrit->get_vertex();
            for (SuffixSet::const_iterator it = vtx.begin()
                ; it != vtx.end()
                ; ++it)
                printf(" at %u", suffix.index_of(
                    static_cast<const SuffixSet::const_vertex&>(it)));
            printf("\n");
        }
        printf("---\n");
        for (uxn::patl::super_maxrep_iterator<SuffixSet> mrit(&suffix)
            ; !mrit->is_root()
            ; ++mrit)
        {
            printf("'%s' x %d:",
                std::string(mrit->key(), mrit->length()).c_str(),
                mrit.freq());
            const SuffixSet::const_vertex vtx = mrit->get_vertex();
            for (SuffixSet::const_iterator it = vtx.begin()
                ; it != vtx.end()
                ; ++it)
                printf(" at %u", suffix.index_of(
                    static_cast<const SuffixSet::const_vertex&>(it)));
            printf("\n");
        }
    }
    //
    printf("\n--- search tandem repeats in O(n)\n\n");
    //
    {
        //typedef int typ;
        typedef char typ;
        typedef patl::suffix_set<const typ*> suffix_t;
        //typ arr[] = {10, 5, 6, 7, 5, 6, 7, 89, 64};
        typ arr[] = "qabrabrabrweabrabraaaad";
        //typ arr[] = "qweabrabrrrrd";
        printf("*** string: '%s':\n", arr);
        suffix_t suf(arr, 16 /* maximal length of tandem repeat + 1 */);
        do
        {
            const suffix_t::const_vertex
                vtx = suf.push_back(),
                sibl = vtx.sibling();
            if (suf.size() > 1)
            {
                const int skip = vtx.skip() / 8 / sizeof(typ);
                const word_t
                    delta = vtx.key() - sibl.key(),
                    count = skip / delta + 1;
                if (count > 1)
                {
                    printf("begin: %u, length: %u, count: %u\n",
                        sibl.key() - arr,
                        delta,
                        count);
                    suf.rebind(sibl.key() + delta * count);
                    suf.clear();
                }
                if (!suf.empty() && suf.endpoint())
                    suf.pop_front();
            }
        } while (suf.keys() + suf.size() != arr + sizeof(arr) / sizeof(arr[0]));
    }
    //
    printf("\n--- generate graphviz dot\n\n");
    //
    patl::patricia_dot_creator<StringSet>().create(vtx_root);
#if 0 // pat_.dot & pat_.clust.dot creation
    {
        std::ifstream fin(argc > 1 ? argv[1] : "WORD.LST");
        if (fin.is_open())
        {
            StringSet dict;
            std::string str;
            while (fin >> str)
                dict.insert(str);
            const_vertex vtx(dict.root());
            //
#if 0
            vtx.mismatch("patch", 5 * 8);
            typedef StringSet::const_preorder_iterator const_preorder_iterator;
            const_preorder_iterator
                itBeg = vtx.preorder_begin(),
                itEnd = vtx.preorder_end(),
                it = itBeg;
            for (; it != itEnd; it.increment(6 * 8))
            {
                if (it->limited(6 * 8))
                    printf("* ");
                printf("%d\t%s\n", it->skip(), it->key().c_str());
            }
            printf("---\n");
            while (it != itBeg)
            {
                it.decrement(6 * 8);
                if (it->limited(6 * 8))
                    printf("* ");
                printf("%d\t%s\n", it->skip(), it->key().c_str());
            }
#else
            vtx.mismatch("patr", 4 * 8);
            {
                std::ofstream fout("patr_.dot");
                patl::patricia_dot_creator<StringSet, std::ofstream>(fout).create(vtx);
            }
            printf("\npatr_.dot created!\n");
            {
                std::ofstream fout("patr_.clust.dot");
                patl::patricia_dot_creator<StringSet, std::ofstream>(fout).create(vtx, true);
            }
            printf("\npatr_.clust.dot created!\n");
#endif
        }
        else
            printf("Unable to open input file!\n");
    }
#endif
    //
    printf("\n--- show/read serialization\n\n");
    //
    {
        typedef patl::trie_set<char*, 0, patl::bit_comparator_0<char> > char_star_trie;
        char_star_trie trie;
        trie.insert("balon");
        trie.insert("balka");
        trie.insert("balet");
        trie.insert("bulat");
        trie.insert("bulka");
        trie.insert("bal");
        trie.insert("balet");
        trie.insert("bal");
        trie.insert("baton");
        printf("*** before show:\n");
        for (char_star_trie::const_iterator it = trie.begin()
            ; it != trie.end()
            ; ++it)
            printf("%s\n", *it);
        std::vector<word_t> vec;
        trie.show(std::back_inserter(vec));
        //
        printf("\n*** serialized data:\n");
        for (word_t i = 0; i != vec.size(); ++i)
            printf("0x%08x ", vec[i]);
        printf("\n");
        //
        trie.read(vec.begin(), vec.end());
        printf("\n*** after read:\n");
        for (char_star_trie::const_iterator it = trie.begin()
            ; it != trie.end()
            ; ++it)
            printf("%s\n", *it);
    }
}
