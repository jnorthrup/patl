#include <vector>
#include <functional>
#include <uxn/patl/aux_/buffered_io.hpp>
#include <uxn/patl/aux_/char.hpp>
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/maxrep_iterator.hpp>
#include <uxn/patl/super_maxrep_iterator.hpp>
#include <uxn/patl/suffix_set.hpp>

namespace patl = uxn::patl;

typedef std::string WordType;
typedef std::vector<WordType> Words;
typedef patl::trie_set<WordType*, patl::ptr_bit_comparator<WordType> > WordSet;
typedef std::vector<WordType*> Text;
typedef patl::suffix_set<WordType**> SuffixType;

static Words words;
static WordSet
    noiseWordset,
    wordset;
static Text text;

/*static buffered_output log1("log1.txt");
static buffered_output log2("log2.txt");*/

template <typename T>
void parse(patl::aux::buffered_input &inp, T insertIt)
{
    bool
        inword = false,
        inpunct = false,
        isdots = false;
    WordType curword;
    while (inp.is_data())
    {
        const unsigned char c = patl::aux::to_lower(inp.read_byte());
        if (patl::aux::is_hyphen(c))
        {
            if (inword || inpunct)
                curword.push_back(c);
            else
            {
                curword.clear();
                curword.push_back(c);
                inpunct = true;
            }
        }
        else if (patl::aux::is_letter(c))
        {
            if (inword)
                curword.push_back(c);
            else
            {
                if (inpunct)
                {
                    if (isdots)
                        insertIt(".");
                    isdots = inpunct = false;
                }
                curword.clear();
                curword.push_back(c);
                inword = true;
            }
        }
        else if (patl::aux::is_punctuation(c))
        {
            if (inpunct)
                curword.push_back(c);
            else
            {
                if (inword)
                {
                    insertIt(curword);
                    inword = false;
                }
                curword.clear();
                curword.push_back(c);
                inpunct = true;
            }
            isdots = isdots || patl::aux::is_dot_symbol(c);
        }
        else // is_blank_char or else
        {
            if (inword)
            {
                insertIt(curword);
                inword = false;
            }
            else if (inpunct)
            {
                if (isdots)
                    insertIt(".");
                isdots = inpunct = false;
            }
        }
    }
    if (inword || inpunct)
        insertIt(curword);
}

inline void insertIt(const WordType &word_t)
{
    unsigned i = words.size();
    words.push_back(word_t);
    WordSet::const_iterator cit = noiseWordset.find(&words[i]);
    if (cit != noiseWordset.end())
    {
        words.pop_back();
        return;
    }
    std::pair<WordSet::iterator, bool> itIns = wordset.insert(&words[i]);
    if (!itIns.second)
        words.pop_back();
    /*else
    {
        log2.write_string(word_t.c_str());
        log2.write_byte('\n');
    }*/
    text.push_back(*itIns.first);
    /*log1.write_string(word_t.c_str());
    log1.write_byte(' ');*/
}

inline void insertItNoise(const WordType &word_t)
{
    unsigned i = words.size();
    words.push_back(word_t);
    std::pair<WordSet::iterator, bool> itIns = noiseWordset.insert(&words[i]);
    if (!itIns.second)
        words.pop_back();
}

void loadNoiseWords(const char *fname)
{
    patl::aux::buffered_input bufIn(fname);
    parse(bufIn, insertItNoise);
}

template <typename T>
class PtrGreaterPred
    : std::binary_function<T*, T*, bool>
{
public:
    bool operator()(const T *a, const T *b)
    {
        return *a > *b;
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
        return 0;
    //
    words.reserve(1 << 20);
    //loadNoiseWords("noiseRUS.txt");
    patl::aux::buffered_input inp(argv[1]);
    parse(inp, insertIt);
    insertIt("***EOF***");
    //
    printf("Words count: %d\n", words.size());
    printf("Words in text: %d\n", text.size());
    //
    SuffixType suffix(&text[0], text.size());
    for (unsigned i = 0; i != text.size(); ++i)
        suffix.push_back();
    //
    /*printf("Checking integrity... ");
    const unsigned failedId = suffix.integrity();
    if (failedId == ~word_t(0))
    printf("successfully!\n");
    else
    {
    printf("failed on %d\n", failedId);
    const SuffixType::node_type *id0 = suffix.node_by(0);
    printf(suffix.node_by(failedId)->get_parent()->visualize(id0));
    printf(suffix.node_by(failedId)->visualize(id0));
    printf(suffix.node_by(failedId)->get_xlink(0)->visualize(id0));
    printf(suffix.node_by(failedId)->get_xlink(1)->visualize(id0));
    }*/
    //
    typedef std::vector<unsigned> CntVector;
    CntVector counts(suffix.size());
    patl::maxrep_iterator<SuffixType> mrit(&suffix);
    for (; !mrit->is_root(); ++mrit)
        counts[mrit->key() - suffix.keys()] = mrit.freq();
    for (unsigned i = 0; i != counts.size(); ++i)
    {
        const unsigned cnt = counts[i];
        if (cnt)
        {
            SuffixType::prefix pref = suffix.prefix_by(i);
            const unsigned num = pref.skip() / SuffixType::bit_size;
            //counts[i] *= num;
            counts[i] = (cnt + cnt * num) / 2 * num;
        }
    }
    typedef std::vector<unsigned*> PtrVector;
    PtrVector cptrs;
    for (CntVector::iterator cit = counts.begin(); cit != counts.end(); ++cit)
    {
        if (*cit)
            cptrs.push_back(&*cit);
    }
    std::sort(cptrs.begin(), cptrs.end(), PtrGreaterPred<unsigned>());
    //
    patl::aux::buffered_output log3("maxreps.txt");
    for (PtrVector::const_iterator cit = cptrs.begin(); cit != cptrs.end(); ++cit)
    {
        const unsigned id = *cit - &counts[0];
        SuffixType::prefix pref = suffix.prefix_by(id);
        const unsigned
            num = pref.skip() / SuffixType::bit_size,
            //cnt = **cit / num;
            cnt = 2 * **cit / (num + num * num);
        if (cnt < 3 || num < 2)
            continue;
        char buf[32];
        sprintf(buf, "% 8d ", cnt);
        log3.write_string(buf);
        for (unsigned i = 0; i != num; ++i)
        {
            log3.write_string(text[id + i]->c_str());
            log3.write_byte(' ');
        }
        log3.write_byte('\n');
    }
}
