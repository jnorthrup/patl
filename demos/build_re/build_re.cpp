#include <string>
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/partial.hpp>

namespace patl = uxn::patl;

template <typename Iter>
void printRegexp(unsigned length, const Iter &beg, const Iter &end)
{
	for (Iter cur = beg; cur != end; ++cur)
	{
		if (beg != cur)
			printf("|");
		if (cur.leaf())
			printf("%s", cur->substr(length - 1).c_str());
		else
		{
			printf("%c", (*cur)[length - 1]);
			unsigned nextLen = length + 1;
			const patl::fixed_length fl(8 * nextLen);
			Iter
				it1 = cur.begin(fl),
				it2 = cur.end(fl);
			while (++Iter(it1) == it2)
			{
				printf("%c", (*cur)[nextLen++ - 1]);
				const patl::fixed_length fl(8 * nextLen);
				it1 = cur.begin(fl);
				it2 = cur.end(fl);
			}
			printf("(");
			const bool question = !(*it1)[nextLen - 1];
			if (question)
				++it1;
			printRegexp(nextLen, it1, it2);
			printf(")");
			if (question)
				printf("?");
		}
	}
}

template <typename Iter>
void printRegexpRev(unsigned length, const Iter &beg, const Iter &end)
{
	for (Iter cur = beg; cur != end; ++cur)
	{
		if (beg != cur)
			printf("|");
		if (cur.leaf())
			printf("%s", cur->substr(length - 1).c_str());
		else
		{
			printf("%c", (*cur)[length - 1]);
			unsigned nextLen = length + 1;
			const patl::fixed_length fl(8 * nextLen);
			Iter
				it1 = cur.rbegin(fl),
				it2 = cur.rend(fl);
			while (++Iter(it1) == it2)
			{
				printf("%c", (*cur)[nextLen++ - 1]);
				const patl::fixed_length fl(8 * nextLen);
				it1 = cur.rbegin(fl);
				it2 = cur.rend(fl);
			}
			printf("(");
			const bool question = !(*(--Iter(it2)))[nextLen - 1];
			if (question)
				--it2;
			printRegexpRev(nextLen, it1, it2);
			printf(")");
			if (question)
				printf("?");
		}
	}
}

int main()
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
	ReservSet rvset(X, X + sizeof(X) / sizeof(X[0]));

	const patl::fixed_length fl(8);
	printf("*** Forward:\n");
	printRegexp(1, rvset.begin(fl), rvset.end(fl));
	printf("\n");

	printf("*** Backward:\n");
	printRegexpRev(1, rvset.rbegin(fl), rvset.rend(fl));
	printf("\n");

	return 0;
}
