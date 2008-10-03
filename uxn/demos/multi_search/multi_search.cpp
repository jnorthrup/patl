#include <algorithm>
#include <vector>
#include <uxn/patl/aux_/file_utils.hpp>
#include <uxn/patl/suffix_set.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>

namespace patl = uxn::patl;

typedef patl::suffix_set<char*> SuffixSet;
typedef std::vector<std::pair<unsigned, std::string> > MatchVector;

static inline unsigned searchInFile(
    SuffixSet &suffix,
    const std::string &filename)
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
    //
    SuffixSet::match_iterator mit(&suffix, huge);
    unsigned line = 1;
    const char *lineBeg = huge;
    MatchVector matches;
    for (unsigned i = 0; i != N; ++i, ++mit)
    {
        const char
            *pattern = mit->key(),
            *str = mit.key();
        //
        if (*str == '\n')
        {
            ++line;
            lineBeg = str + 1;
        }
        // ��������� �� ���� ����� ����� '\n'?
        // �.�., ������� �� ������ ���������� � ����� �� ������?
        if ('\n' == pattern[-1] && '\n' == pattern[mit.length()])
        {
            const char
                *stop = "\r\n",
                *lineEnd = std::find_first_of(
                str,
                patl::impl::get_min(str + 160, end),
                stop,
                stop + sizeof(stop));
            std::string lineS(
                patl::impl::get_max(lineBeg, str - 12),
                patl::impl::get_min(lineEnd, str + mit.length() + 12));
            std::replace(lineS.begin(), lineS.end(), '\t', ' ');
            matches.push_back(std::make_pair(line, lineS));
        }
    }
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

static inline unsigned searchByMask(
    SuffixSet &suffix,
    const std::string &folder,
    const std::string &mask)
{
    const bool starp = mask == "*" || mask == "*.*";
    unsigned matc = 0;
    WIN32_FIND_DATA wfData;
    HANDLE hFind = FindFirstFile((folder + '\\' + mask).c_str(), &wfData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(wfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                matc += searchInFile(suffix, folder + '\\' + wfData.cFileName);
            else if (starp && !patl::aux::is_dots_folder(wfData.cFileName))
                matc += searchByMask(suffix, folder + '\\' + wfData.cFileName, mask);
        } while (FindNextFile(hFind, &wfData));
        FindClose(hFind);
    }
    if (!starp)
    {
        hFind = FindFirstFile((folder + "\\*").c_str(), &wfData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (
                    (wfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
                    !patl::aux::is_dots_folder(wfData.cFileName))
                    matc += searchByMask(
                    suffix,
                    folder + '\\' + wfData.cFileName,
                    mask);
            } while (FindNextFile(hFind, &wfData));
            FindClose(hFind);
        }
    }
    return matc;
}

static inline unsigned search(SuffixSet &suffix, const std::string &str)
{
    const std::string folder = patl::aux::take_file_folder(str);
    std::string nameExt = patl::aux::take_file_name_ext(str);
    if (nameExt.empty())
        nameExt = "*";
    const bool isWildcard = nameExt.find_first_of("?*") != std::string::npos;
    if (isWildcard)
        return searchByMask(suffix, folder, nameExt);
    WIN32_FIND_DATA wfData;
    HANDLE hFind = FindFirstFile((folder + '\\' + nameExt).c_str(), &wfData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        if (wfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            return searchByMask(suffix, str, "*");
        else
            return searchInFile(suffix, folder + '\\' + wfData.cFileName);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("MFIND <infile> \"string1\" .. \"stringN\"\n");
        return 0;
    }
    std::string reserv("\n");
    printf("***");
    for (int i = 2; i != argc; ++i)
    {
        (reserv += argv[i]) += '\n';
        printf(" \"%s\"", argv[i]);
    }
    printf("\n\n");
    SuffixSet suffix(&reserv[1], reserv.length());
    for (unsigned i = reserv.size() - 2; i; --i)
        suffix.push_back();
    //
    patl::aux::performance_timer tim;
    const unsigned matc = search(suffix, argv[1]);
    tim.finish();
    printf("*** Matches count: %d\n*** Timer: %.2f\n", matc, tim.get_seconds());
}
