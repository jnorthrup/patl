/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_PATRICIA_DOT_BASE_HPP
#define PATL_IMPL_PATRICIA_DOT_BASE_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

inline std::string graphviz_printable_string(const char *p, word_t l)
{
    std::string s(l, ' ');
    for (word_t i = 0; i != l; ++i)
        s[i] = !p[i] || p[i] < 0 ? ' ' : p[i];
    return s;
}

template<typename Cont, typename OutStream>
class patricia_dot_base
{
public:
    typedef Cont cont_type;
    typedef typename cont_type::vertex vertex;

    static void out_node(OutStream &out, const vertex &vtx)
    {
        out << "[label = \""
            << vtx.parent_key()
            << "\\n"
            << static_cast<sword_t>(vtx.skip())
            << "\"]";
    }

    static void out_edge(OutStream&, const vertex&)
    {
    }
};

template <
    typename Type, word_t N, word_t Delta, typename BitComp, typename Allocator,
    typename OutStream>
class patricia_dot_base<suffix_set<Type, N, Delta, BitComp, Allocator>, OutStream>
{
public:
    typedef suffix_set<Type, N, Delta, BitComp, Allocator> cont_type;
    typedef typename cont_type::vertex vertex;
    typedef typename cont_type::bit_compare bit_compare;

    static const word_t bit_size = bit_compare::bit_size;

    static void out_node(OutStream &out, const vertex &vtx)
    {
        out << "[label = \""
            << vtx.parent_key() - vtx.cont()->keys() << ": '"
            << graphviz_printable_string(vtx.parent_key(), align_up<bit_size>(max0(vtx.skip())) / bit_size)
            << "'\\n"
            << static_cast<sword_t>(vtx.skip())
            << "\"]\n";
    }

    static void out_edge(OutStream&, const vertex&)
    {
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
