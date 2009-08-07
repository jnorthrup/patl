/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_FILE_UTILS_HPP
#define PATL_AUX_FILE_UTILS_HPP

#include "../config.hpp"
#include <string>
#include <vector>
#include <windows.h>

namespace uxn
{
namespace patl
{
namespace aux
{

class win32_exception
    : public std::exception
{
public:
    win32_exception(const char *fun_name, const char *fname, int loc)
        : mess_(64 << 10)
    {
        const DWORD mess_id(::GetLastError());
        char msg_buf[1024];
        ::FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM,
            0,
            mess_id,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            msg_buf,
            1024,
            0);
        sprintf(&mess_[0], "%s (%d) : code %d in %s: %s",
            fname,
            loc,
            mess_id,
            fun_name,
            msg_buf);
    }

    ~win32_exception() throw()
    {
    }

    const char *what() const throw()
    {
        return &mess_[0];
    }

private:
    std::vector<char> mess_;
};

#define PATL_WIN32_EXCEPTION(fun_name)\
    throw uxn::patl::aux::win32_exception(#fun_name, __FILE__, __LINE__)

inline void delete_file(const char *fname)
{
    if (!::DeleteFile(fname))
        PATL_WIN32_EXCEPTION(DeleteFile);
}

inline void move_file(const char *from_path, const char *to_path)
{
    if (!::MoveFileEx(from_path, to_path,
        MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
        PATL_WIN32_EXCEPTION(MoveFileEx);
}

inline std::string get_temp_file_name(
    const char *prefix = "TMP",
    const char *path = ".")
{
    char buf[MAX_PATH];
    if (!::GetTempFileName(path, prefix, 0, buf))
        PATL_WIN32_EXCEPTION(GetTempFileName);
    return std::string(buf);
}

inline word_t get_file_length(HANDLE fh)
{
    const word_t cur = ::SetFilePointer(fh, 0, 0, FILE_CURRENT);
    const word_t len = ::SetFilePointer(fh, 0, 0, FILE_END);
    if (cur == INVALID_SET_FILE_POINTER || len == INVALID_SET_FILE_POINTER ||
        ::SetFilePointer(fh, cur, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        PATL_WIN32_EXCEPTION(SetFilePointer);
    return len;
}

inline long long get_long_file_length(HANDLE fh)
{
    LONG cur_h = 0, len_h = 0;
    const DWORD cur = ::SetFilePointer(fh, 0, &cur_h, FILE_CURRENT);
    const DWORD len = ::SetFilePointer(fh, 0, &len_h, FILE_END);
    if (cur == INVALID_SET_FILE_POINTER || len == INVALID_SET_FILE_POINTER ||
        ::SetFilePointer(fh, cur, &cur_h, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        PATL_WIN32_EXCEPTION(SetFilePointer);
    return
        static_cast<long long>(len) |
        (static_cast<long long>(len_h) << (8 * sizeof(long)));
}

inline long long get_long_file_length(const char *fname)
{
    WIN32_FIND_DATA data;
    const HANDLE ffh = ::FindFirstFile(fname, &data);
    if (ffh == INVALID_HANDLE_VALUE)
        PATL_WIN32_EXCEPTION(FindFirstFile);
    ::FindClose(ffh);
    return
        static_cast<long long>(data.nFileSizeLow) |
        (static_cast<long long>(data.nFileSizeHigh) << (8 * sizeof(long)));
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
