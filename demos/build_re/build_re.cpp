#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/partial.hpp>

namespace patl = uxn::patl;

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
    typedef ReservSet::vertex vertex;
	ReservSet rvset(X, X + sizeof(X) / sizeof(X[0]));

	printf("*** Regexp:\n");
    const vertex vtx(&rvset);
	printRegexp(0, vtx.levelorder_begin(8), vtx.levelorder_end(8));
	printf("\n");

	return 0;
}
