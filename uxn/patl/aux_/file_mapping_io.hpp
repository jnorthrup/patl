/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_FILE_MAPPING_IO_HPP
#define PATL_AUX_FILE_MAPPING_IO_HPP

#include "../config.hpp"
#include "../impl/trivial.hpp"
#include "file_utils.hpp"
#include <windows.h>

namespace uxn
{
namespace patl
{
namespace aux
{

class file_mapping_view
{
public:
    file_mapping_view(HANDLE fmh, long long offset, word_t size)
        : ptr_(::MapViewOfFile(
            fmh,
            FILE_MAP_READ,
            static_cast<DWORD>(offset >> 32),
            static_cast<DWORD>(offset),
            size))
        , size_(size)
    {
        if (!ptr_)
            PATL_WIN32_EXCEPTION(MapViewOfFile);
    }

    ~file_mapping_view()
    {
        if (ptr_)
            ::UnmapViewOfFile(ptr_);
    }

    void *get_view() const
    {
        return ptr_;
    }

    word_t size() const
    {
        return size_;
    }

private:
    void *ptr_;
    word_t size_;
};

class file_mapping_input
{
public:
    file_mapping_input(const char *fname)
        : fh_(::CreateFile(
            fname, GENERIC_READ, FILE_SHARE_READ, 0,
            OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0))
        , fmh_(fh_ != INVALID_HANDLE_VALUE
            ? ::CreateFileMapping(fh_, 0, PAGE_READONLY, 0, 0, 0)
            : 0)
        , length_(fh_ != INVALID_HANDLE_VALUE
            ? patl::aux::get_long_file_length(fh_)
            : -1)
        , offset_(0)
    {
        if (fh_ == INVALID_HANDLE_VALUE)
            PATL_WIN32_EXCEPTION(CreateFile);
        if (!fmh_)
            PATL_WIN32_EXCEPTION(CreateFileMapping);
    }

    ~file_mapping_input()
    {
        if (fmh_)
            ::CloseHandle(fmh_);
        ::CloseHandle(fh_);
    }

    long long get_file_length() const
    {
        return get_long_file_length(fh_);
    }

    long long residue_length() const
    {
        return length_ - offset_;
    }

    std::auto_ptr<file_mapping_view> create_view(word_t size)
    {
        size = static_cast<word_t>(patl::impl::get_min(
                static_cast<long long>(patl::impl::align_up<1 << 16>(size)),
                residue_length()));
        std::auto_ptr<file_mapping_view> v(
            new file_mapping_view(fmh_, offset_, size));
        offset_ += size;
        return v;
    }

    word_t drop_back(word_t dropped)
    {
        dropped = static_cast<word_t>(offset_ - patl::impl::align_down<1 << 16>(
            static_cast<word_t>(offset_ - dropped)));
        offset_ = dropped < offset_ ? offset_ - dropped : 0;
        return dropped;
    }

private:
    HANDLE fh_, fmh_;
    long long length_;
    long long offset_;
};

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
