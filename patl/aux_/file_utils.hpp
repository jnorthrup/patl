#ifndef PATL_AUX_FILE_UTILS_HPP
#define PATL_AUX_FILE_UTILS_HPP

#include <windows.h>
#include "../config.hpp"

namespace uxn
{
namespace patl
{
namespace aux
{

inline word_t get_file_length(HANDLE fh)
{
    const word_t
        cur = ::SetFilePointer(fh, 0, 0, FILE_CURRENT),
        len = ::SetFilePointer(fh, 0, 0, FILE_END);
    ::SetFilePointer(fh, cur, 0, FILE_BEGIN);
    return len;
}

inline std::string take_file_folder(const std::string &str)
{
    const std::string::size_type n = str.find_last_of("\\/:");
    if (n == std::string::npos)
        return ".";
    switch (str[n])
    {
    case ':':
        return str.substr(0, n + 1);
    case '\\':
    case '/':
        if (str[n - 1] == ':')
            return str.substr(0, n + 1);
        else
            return str.substr(0, n);
    default:
        return "error";
    }
}

inline std::string take_file_name_ext(const std::string &str)
{
    const std::string::size_type n = str.find_last_of("\\/:");
    return str.substr(0, n);
}

inline bool is_dots_folder(const char *ref)
{
    return ref[0] == '.' && (ref[1] == 0 || ref[1] == '.' && ref[2] == 0);
}

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
