/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Simplest implementation of multiple string alignment problem
 | Using suffix trees with O(1) LCA after O(N) postprocessing
-*/
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/lca_oracle.hpp>
#include <uxn/patl/patricia_dot_creator.hpp>
#include <map>
#include <fstream>

namespace patl = uxn::patl;

//#define MULTI_ALIGN_DEBUG

typedef unsigned char id_type; // max 256 input strings

template <typename Cont, typename OutStream = std::ostream>
class patricia_dot_base
{
public:
    typedef Cont cont_type;
    typedef typename cont_type::const_vertex const_vertex;
    typedef typename cont_type::vertex vertex;
    typedef typename cont_type::bit_compare bit_compare;

    static const word_t bit_size = bit_compare::bit_size;

    patricia_dot_base(const std::vector<id_type> *ids, const std::vector<word_t> *h)
        : ids_(ids)
        , h_(h)
    {
    }

    void out_node(OutStream &out, const const_vertex &vtx) const
    {
        out << "[label = \""
            << vtx.parent_key() - vtx.cont()->keys() << ": '"
            << patl::impl::graphviz_printable_string(
                vtx.parent_key(),
                patl::impl::get_max<word_t>(10, patl::impl::align_up<bit_size>(patl::impl::max0(vtx.skip())) / bit_size))
            << "'\\nid: " << static_cast<word_t>((*ids_)[vtx.parent_key() - vtx.cont()->keys()])
            << "\\n"
            << static_cast<sword_t>(vtx.skip())
            << "\"]";
    }

    void out_edge(OutStream &out, const vertex &vtx) const
    {
        if (!vtx.get_qtag())
            out << "[taillabel = " << (*h_)[vtx.cont()->vertex_index_of(vtx)] << ']';
    }

private:
    const std::vector<id_type> *ids_;
    const std::vector<word_t> *h_;
};

typedef patl::suffix_set<char*> suffix_t;
typedef suffix_t::const_vertex const_vertex;
typedef suffix_t::vertex vertex;

void multiple_common_substring(
    const std::vector<std::string> &strs,
    std::vector<std::string> &align)
{
    if (strs.size() == 1)
    {
        align.push_back(strs[0]);
        align.push_back(std::string(strs[0].length(), ' '));
        return;
    }
    //
    std::vector<char> seq;
    std::vector<id_type> ids;
    std::vector<word_t> offs;
    for (id_type id = 0; id != strs.size(); ++id)
    {
        offs.push_back(seq.size());
        std::copy(strs[id].begin(), strs[id].end(), std::back_inserter(seq));
        seq.push_back('\0');
        seq.push_back(id);
        ids.insert(ids.end(), strs[id].length() + 2, id);
    }
    offs.push_back(seq.size());
    //
    const word_t K = strs.size();
    //
    suffix_t suf(&seq[0], seq.size());
    for (word_t i = 0; i != seq.size() - 1; ++i)
        suf.push_back();
    //
    std::vector<word_t> h(suf.size() * 2);
    //
    {
        patl::lca_oracle<suffix_t> lca(suf);
        //
        std::vector<const_vertex> last_vtx(K);
        //
        for (suffix_t::const_iterator it = suf.begin(); it != suf.end(); ++it)
        {
            const word_t k = ids[*it - suf.keys()];
            const const_vertex &vtx = static_cast<const const_vertex&>(it);
            if (last_vtx[k].compact())
                ++h[suf.vertex_index_of(lca(last_vtx[k], vtx))];
            last_vtx[k] = vtx;
        }
    }
    //
#ifdef MULTI_ALIGN_DEBUG
    {
        std::ofstream fout("malign_suf.dot");
        patricia_dot_base<suffix_t> pdb(&ids, &h);
        patl::patricia_dot_creator<
            suffix_t,
            std::ofstream, patricia_dot_base<suffix_t> > dotcr(fout, pdb);
        dotcr.create(suf.root());
    }
#endif
    //
    std::vector<const_vertex> v(K + 1);
    std::vector<word_t> v_len(K + 1);
    //
    std::vector<word_t> s, u;
    const const_vertex root = suf.root();
    const suffix_t::const_postorder_iterator
        pit_beg = root.postorder_begin(),
        pit_end = root.postorder_end();
    for (suffix_t::const_postorder_iterator pit = pit_beg; pit != pit_end; ++pit)
    {
        if (pit->get_qtag())
        {
            s.push_back(1);
            u.push_back(0);
        }
        else
        {
            const word_t
                s_v = s.back() + s[s.size() - 2],
                u_v = u.back() + u[u.size() - 2] + h[suf.vertex_index_of(*pit)],
                c_v = s_v - u_v,
                idx = suf.index_of(*pit),
                len = patl::impl::get_min(
                    pit->next_skip() / 8,
                    patl::impl::max0(strs[ids[idx]].length() - (idx - offs[ids[idx]])));
            if (len > v_len[c_v])
            {
                v[c_v] = *pit;
                v_len[c_v] = len;
            }
            s.pop_back();
            s.back() = s_v;
            u.pop_back();
            u.back() = u_v;
        }
    }
    //
    for (word_t k = K - 1; k != 1; --k)
    {
        if (v_len[k] < v_len[k + 1])
        {
            v[k] = v[k + 1];
            v_len[k] = v_len[k + 1];
        }
    }
    // multiple common substrings
#ifdef MULTI_ALIGN_DEBUG
    printf("---\n");
    for (word_t k = 2; k != v.size(); ++k)
    {
        if (v[k].compact())
            printf("%u\t%u\t%s\n", k, v_len[k], std::string(v[k].key(), v_len[k]).c_str());
    }
#endif
    // эвристический выбор подход€щего k
    word_t k_cur = v.size() - 1;
    for (; k_cur != 1 && v_len[k_cur] < 2; --k_cur) ;
    if (k_cur == 1)
    {
        for (k_cur = v.size() - 1; k_cur != 1 && !v_len[k_cur]; --k_cur) ;
        if (k_cur == 1)
        {
            word_t max_len = 0;
            for (id_type i = 0; i != strs.size(); ++i)
            {
                if (strs[i].length() > max_len)
                    max_len = strs[i].length();
            }
            for (id_type i = 0; i != strs.size(); ++i)
            {
                align.push_back(strs[i]);
                std::string &ref = align.back();
                while (ref.length() != max_len)
                    ref.push_back(' ');
            }
            align.push_back(std::string(max_len, ' '));
            return;
        }
    }
    //
    // std::pair<номер строки, смещение в строке>
    typedef std::map<id_type, word_t> motif_map;
    motif_map motifs; // номера строк с повтор€ющимс€ мотивом
    {
        const suffix_t::const_iterator it_end = v[k_cur].end();
        for (suffix_t::const_iterator it = v[k_cur].begin(); it != it_end; ++it)
        {
            const id_type id = ids[*it - suf.keys()];
            const word_t off = *it - suf.keys() - offs[id];
            motifs[id] = off;
        }
    }
    const word_t motif_len = v_len[k_cur];
    const std::string
        motif(v[k_cur].key(), motif_len),
        padding(motif_len, ' ');
    align.clear();
    for (id_type i = 0; i != K; ++i)
        align.push_back(motifs.find(i) != motifs.end() ? motif : padding);
    align.push_back(std::string(motif_len, '^'));
    word_t
        sum_left = 0,
        sum_right = 0;
    for (motif_map::const_iterator it = motifs.begin(); it != motifs.end(); ++it)
    {
        if (it->second)
            ++sum_left;
        if (strs[it->first].length() - it->second - motif_len)
            ++sum_right;
    }
    // left side
    if (sum_left)
    {
        std::vector<id_type> left_ids;
        std::vector<std::string> left_strs;
        for (id_type i = 0; i != K; ++i)
        {
            const motif_map::const_iterator it = motifs.find(i);
            if (it != motifs.end())
            {
                if (it->second)
                {
                    left_ids.push_back(it->first);
                    left_strs.push_back(strs[it->first].substr(0, it->second));
                }
            }
            else
            {
                left_ids.push_back(i);
                left_strs.push_back(strs[i]);
            }
        }
        std::vector<std::string> left_align;
        multiple_common_substring(left_strs, left_align);
        const word_t left_len = left_align[0].length();
        const std::string left_pad(left_len, ' ');
        for (id_type i = 0, j = 0; i != K; ++i)
        {
            align[i].insert(0, j != left_ids.size() && left_ids[j] == i
                ? left_align[j++]
                : left_pad);
        }
        align.back().insert(0, left_align.back());
    }
    // right side
    if (sum_right)
    {
        std::vector<id_type> right_ids;
        std::vector<std::string> right_strs;
        for (id_type i = 0; i != K; ++i)
        {
            const motif_map::const_iterator it = motifs.find(i);
            if (it != motifs.end())
            {
                if (strs[it->first].length() - it->second - motif_len)
                {
                    right_ids.push_back(it->first);
                    right_strs.push_back(strs[it->first].substr(it->second + motif_len));
                }
            }
            else if (!sum_left)
            {
                right_ids.push_back(i);
                right_strs.push_back(strs[i]);
            }
        }
        std::vector<std::string> right_align;
        multiple_common_substring(right_strs, right_align);
        const word_t right_len = right_align[0].length();
        const std::string right_pad(right_len, ' ');
        for (id_type i = 0, j = 0; i != K; ++i)
        {
            align[i].append(j != right_ids.size() && right_ids[j] == i
                ? right_align[j++]
                : right_pad);
        }
        align.back().append(right_align.back());
    }
    // neither left nor right
    if (!sum_left && !sum_right && K - k_cur)
    {
        std::vector<id_type> neither_ids;
        std::vector<std::string> neither_strs;
        for (id_type i = 0; i != K; ++i)
        {
            const motif_map::const_iterator it = motifs.find(i);
            if (it == motifs.end())
            {
                neither_ids.push_back(i);
                neither_strs.push_back(strs[i]);
            }
        }
        std::vector<std::string> neither_align;
        multiple_common_substring(neither_strs, neither_align);
        const word_t neither_len = neither_align[0].length();
        const std::string neither_pad(neither_len, ' ');
        for (id_type i = 0, j = 0; i != K; ++i)
        {
            align[i].append(j != neither_ids.size() && neither_ids[j] == i
                ? neither_align[j++]
                : neither_pad);
        }
        align.back().append(neither_align.back());
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("MULTI_ALIGN <infile> <outfile>\n");
        return 0;
    }
    std::vector<std::string> strs;
    {
        std::ifstream fin(argv[1]);
        if (!fin.is_open())
        {
            printf("input file not found: %s\n", argv[1]);
            return 0;
        }
        std::string str;
        char *buf = new char [64 << 10];
        for (id_type id = 0; fin.getline(buf, 64 << 10); ++id)
            strs.push_back(std::string(buf, strlen(buf)));
        delete[] buf;
    }
    //
    std::vector<std::string> align;
    multiple_common_substring(strs, align);
    {
        std::ofstream fout(argv[2]);
        if (!fout.is_open())
        {
            printf("unable to open output file: %s\n", argv[2]);
            return 0;
        }
        for (word_t i = 0; i != align.size(); ++i)
            fout << align[i] << '\n';
    }
}
