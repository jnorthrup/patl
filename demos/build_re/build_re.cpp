#include <uxn/patl/trie_set.hpp>
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
            const word_t next_len = cur->next_skip() / 8;
			printf("%s", cur->key().substr(length, next_len - length).c_str());
			Iter
				it1 = cur->levelorder_begin(8 * (next_len + 1)),
				it2 = cur->levelorder_end(8 * (next_len + 1));
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
	const char *const X[] = {
		"asm", "auto", "bool", "break", "case", "catch", "char", "class", "const", 
		"const_cast", "continue", "default", "delete", "do", "double", 
		"dynamic_cast", "else", "enum", "explicit", "export", "extern", "false",
		"float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable",
		"namespace", "new", "operator", "private", "protected", "public", 
		"register", "reinterpret_cast", "return", "short", "signed", "sizeof",
		"static", "static_cast", "struct", "switch", "template", "this", "throw",
		"true", "try", "typedef", "typeid", "typename", "union", "word_t", 
		"using", "virtual", "void", "volatile", "wchar_t", "while"};

	typedef patl::trie_set<std::string> ReservSet;
    typedef ReservSet::vertex vertex;
	ReservSet rvset(X, X + sizeof(X) / sizeof(X[0]));

	printf("*** Regexp:\n");
    const vertex vtx(&rvset);
	printRegexp(0, vtx.levelorder_begin(8), vtx.levelorder_end(8));
	printf("\n");

	return 0;
}
