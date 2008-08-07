#include <windows.h>
#include <vector>
#include <uxn/patl/aux_/file_utils.hpp>
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>

namespace patl = uxn::patl;

typedef patl::suffix_set<char*> SuffixSet;
typedef SuffixSet::vertex vertex;
typedef std::vector<std::pair<unsigned, std::string> > MatchVector;

static inline unsigned searchInFile(
    const SuffixSet &suffix,
    const std::string &filename,
    unsigned K)
{
    const HANDLE fh = CreateFile(
        filename.c_str(), GENERIC_READ, FILE_SHARE_READ, 0,
        OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0);
    if (fh == INVALID_HANDLE_VALUE)
        return 0;
    const unsigned N = patl::aux::get_file_length(fh);
    const HANDLE fmh = CreateFileMapping(fh, 0, PAGE_READONLY, 0, N, 0);
    if (!fmh)
        return 0;
    char *huge = static_cast<char*>(MapViewOfFile(fmh, FILE_MAP_READ, 0, 0, N));
    const char *end = huge + N - 1;
    if (!huge)
        return 0;
    MatchVector matches;
    //
    //
    UnmapViewOfFile(huge);
    CloseHandle(fmh);
    CloseHandle(fh);
    //
    if (!matches.empty())
    {
        printf("*** %s\n", filename.c_str());
        for (
            MatchVector::const_iterator cit = matches.begin();
            cit != matches.end();
        ++cit)
            printf("%d\t... %s ...\n", cit->first, cit->second.c_str());
        printf("\n");
    }
    return matches.size();
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("FUZZY <infile> \"string\" <K>\n");
        return 0;
    }
    std::string str(argv[2]);
    SuffixSet suffix(&str[0], str.length());
    for (unsigned i = str.length(); i; --i)
        suffix.push_back();
    patl::aux::performance_timer tim;
    const unsigned matc = searchInFile(suffix, argv[1], atoi(argv[3]));
    tim.finish();
    printf("*** Matches count: %d\n*** Timer: %.2f\n", matc, tim.get_seconds());
}
