#ifndef PATL_PATRICIA_DOT_CREATOR_HPP
#define PATL_PATRICIA_DOT_CREATOR_HPP

#include <iostream>
#include <iomanip>

namespace uxn
{
namespace patl
{

template <typename Cont, typename OutStream = std::ostream>
class patricia_dot_creator
{
    typedef Cont cont_type;
    typedef typename cont_type::vertex vertex;
    typedef typename cont_type::preorder_iterator preorder_iterator;

public:
    patricia_dot_creator(word_t dpi = 96)
        : dpi_(dpi)
    {
    }

    void create(const vertex &vtx, OutStream &out = std::cout) const
    {
        out << "strict digraph\n";
        out << "{\n";
        out << "// common props\n";
        out << "dpi = " << dpi_ << "\n";
        out << "edge[arrowhead = empty]\n";
        out << "\n// node definitions (preorder depth-first)\n";
        const word_t
            init_id = vtx.get_qid(),
            sibl_qtag = vtx.sibling().get_qtag();
        out << "sibling[shape = plaintext, label = "
            << (vtx.skip() == ~word_t(0) ? "\"nil\"" : "\"...\"")
            << "]\n";
        const preorder_iterator pit_end(vtx.preorder_end());
        for (preorder_iterator pit(vtx.preorder_begin()); pit != pit_end; ++pit)
        {
            if (init_id == pit->get_qid())
                out << 'n' << std::hex << pit->node_q_uid() << std::dec
                    << "[label = \""
                    << pit->parent_key()
                    << "\\n"
                    << static_cast<sword_t>(pit->skip())
                    << "\"]\n";
        }
        //
        out << std::hex;
        //
        out << "\n// root link to sibling\n";
        out << 'n' << vtx.node_q_uid() << "->sibling[tailport = "
            << (init_id ? "sw" : "se") << ", headport = "
            << (sibl_qtag ? (init_id ? "se" : "sw") : (init_id ? "ne" : "nw")) << ", style = "
            << (sibl_qtag ? "dotted" : "solid") << "]\n";
        out << "\n// forward left links\n";
        out << "edge[tailport = sw]\n";
        {
            bool linkp = false;
            for (preorder_iterator pit(vtx.preorder_begin()); pit != pit_end; ++pit)
            {
                if (linkp)
                    out << "->" << 'n' << pit->node_q_uid();
                if (!pit->get_qtag() && !pit->get_qid())
                {
                    if (!linkp)
                        out << 'n' << pit->node_q_uid();
                    linkp = true;
                }
                else
                {
                    if (linkp)
                        out << "\n";
                    linkp = false;
                }
            }
        }
        out << "\n// forward right links\n";
        out << "edge[tailport = se]\n";
        {
            word_t prev_node = 0;
            for (preorder_iterator pit(vtx.preorder_begin()); pit != pit_end; ++pit)
            {
                if (!pit->get_qtag() && pit->get_qid())
                {
                    if (prev_node != pit->node_q_uid())
                    {
                        if (prev_node)
                            out << '\n';
                        out << 'n' << pit->node_q_uid();
                    }
                    prev_node = pit->node_p_uid();
                    out << "->" << 'n' << prev_node;
                }
            }
            if (prev_node)
                out << '\n';
        }
        out << "\n// backward left links\n";
        out << "edge[style = dotted, tailport = sw]\n";
        for (preorder_iterator pit(vtx.preorder_begin()); pit != pit_end; ++pit)
        {
            if (pit->get_qtag() && !pit->get_qid())
                out << 'n' << pit->node_q_uid() << "->" << 'n' << pit->node_p_uid() << "\n";
        }
        out << "\n// backward right links\n";
        out << "edge[tailport = se]\n";
        for (preorder_iterator pit(vtx.preorder_begin()); pit != pit_end; ++pit)
        {
            if (pit->get_qtag() && pit->get_qid())
                out << 'n' << pit->node_q_uid() << "->" << 'n' << pit->node_p_uid() << "\n";
        }
        out << "}\n";
    }

private:
    word_t dpi_;
};

} // namespace patl
} // namespace uxn

#endif
