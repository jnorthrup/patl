#ifndef PATL_IMPL_PATRICIA_DOT_BASE_HPP
#define PATL_IMPL_PATRICIA_DOT_BASE_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

inline std::string printable_string(const char *p, word_t l)
{
    std::string s(l, ' ');
    for (word_t i = 0; i != l; ++i)
        s[i] = !p[i] || p[i] < 0 ? ' ' : p[i];
    return s;
}

template<typename Cont>
class patricia_dot_base
{
public:
    typedef Cont cont_type;
    typedef typename cont_type::vertex vertex;

protected:
    template <typename OutStream>
    static void out_node(OutStream &out, const vertex &vtx)
    {
        out << 'n' << std::hex << vtx.node_q_uid() << std::dec
            << "[label = \""
            << vtx.parent_key()
            << "\\n"
            << static_cast<sword_t>(vtx.skip())
            << "\"]\n";
    }

    template <typename OutStream>
    static void out_edge(OutStream &out, const vertex &/*vtx*/)
    {
        out << '\n';
    }
};

template <typename Type, sword_t Delta, typename BitComp, typename Allocator>
class patricia_dot_base<suffix_set<Type, Delta, BitComp, Allocator> >
{
public:
    typedef suffix_set<Type, Delta, BitComp, Allocator> cont_type;
    typedef typename cont_type::vertex vertex;
    typedef typename cont_type::bit_compare bit_compare;

    static const word_t bit_size = bit_compare::bit_size;

protected:
    template <typename OutStream>
    static void out_node(OutStream &out, const vertex &vtx)
    {
        out << 'n' << std::hex << vtx.node_q_uid() << std::dec
            << "[label = \""
            << vtx.parent_key() - vtx.cont()->keys() << ": '"
            << printable_string(vtx.parent_key(), align_up<bit_size>(max0(vtx.skip())) / bit_size)
            << "'\\n"
            << static_cast<sword_t>(vtx.skip())
            << "\"]\n";
    }

    template <typename OutStream>
    static void out_edge(OutStream &out, const vertex &/*vtx*/)
    {
        out << '\n';
    }
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
