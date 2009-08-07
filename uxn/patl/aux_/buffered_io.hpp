/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_BUFFERED_IO_HPP
#define PATL_AUX_BUFFERED_IO_HPP

#include "../config.hpp"
#include "file_utils.hpp"
#include <algorithm>
#include <exception>
#include <windows.h>

namespace uxn
{
namespace patl
{
namespace aux
{

class buffered_base
{
    buffered_base &operator=(const buffered_base&);

protected:
    buffered_base(const char *fname, bool isWrite)
        : fh_(::CreateFile(
            fname,
            isWrite ? GENERIC_WRITE : GENERIC_READ,
            FILE_SHARE_READ,
            0,
            isWrite ? CREATE_ALWAYS : OPEN_EXISTING,
            FILE_FLAG_SEQUENTIAL_SCAN,
            0))
        , buf_(static_cast<unsigned char*>(::VirtualAlloc(
            0,
            BufferSize,
            MEM_COMMIT,
            PAGE_READWRITE)))
        , end_(buf_)
        , ownHandle_(true)
    {
        if (fh_ == INVALID_HANDLE_VALUE)
            PATL_WIN32_EXCEPTION(CreateFile);
        if (!buf_)
            PATL_WIN32_EXCEPTION(VirtualAlloc);
    }

    explicit buffered_base(HANDLE fh)
        : fh_(fh)
        , buf_(static_cast<unsigned char*>(::VirtualAlloc(
            0,
            BufferSize,
            MEM_COMMIT,
            PAGE_READWRITE)))
        , end_(buf_)
        , ownHandle_(false)
    {
    }

    ~buffered_base()
    {
        ::VirtualFree(buf_, 0, MEM_RELEASE);
        if (ownHandle_)
            ::CloseHandle(fh_);
    }

public:
    long long get_file_length()
    {
        return get_long_file_length(fh_);
    }

protected:
    void set_file_ptr(word_t offs)
    {
        ::SetFilePointer(fh_, offs, 0, FILE_BEGIN);
    }

    word_t get_file_ptr()
    {
        return ::SetFilePointer(fh_, 0, 0, FILE_CURRENT);
    }

    enum
    {
        BufferSize = 1<<12
    };

    const HANDLE fh_;
    unsigned char *const buf_;
    unsigned char *end_;

private:
    buffered_base();

    const bool ownHandle_;
};

class buffered_input
    : public buffered_base
{
public:
    explicit buffered_input(const char *fname)
        : buffered_base(fname, false)
        , cur_(buf_)
    {
        fill_buffer();
    }

    explicit buffered_input(HANDLE fh)
        : buffered_base(fh)
        , cur_(buf_)
    {
        fill_buffer();
    }

    bool is_data() const
    {
        return cur_ != end_;
    }

    unsigned char read_byte()
    {
        const unsigned char byte = *cur_++;
        if (cur_ == end_)
            fill_buffer();
        return byte;
    }

    word_t read_word()
    {
        word_t w = static_cast<word_t>(read_byte());
        w |= static_cast<word_t>(read_byte()) << 8;
        w |= static_cast<word_t>(read_byte()) << 16;
        w |= static_cast<word_t>(read_byte()) << 24;
#ifdef PATL_64
        w |= static_cast<word_t>(read_byte()) << 32;
        w |= static_cast<word_t>(read_byte()) << 40;
        w |= static_cast<word_t>(read_byte()) << 48;
        w |= static_cast<word_t>(read_byte()) << 56;
#endif
        return w;
    }

    word_t read_bytes(unsigned char *ptr, word_t len)
    {
        const word_t
            avail = static_cast<word_t>(end_ - cur_),
            chunk = avail < len ? avail : len;
        std::copy(cur_, cur_ + chunk, ptr);
        cur_ += chunk;
        len -= chunk;
        if (len)
            len = fill_buffer(ptr + chunk, len);
        if (cur_ == end_)
            fill_buffer();
        return chunk + len;
    }

    void drop_back(word_t offs)
    {
        if (static_cast<word_t>(cur_ - buf_) < offs)
        {
            set_file_ptr(get_file_ptr() - (end_ - cur_) - offs);
            fill_buffer();
        }
        else
            cur_ -= offs;
    }

    void skip_data(word_t offs)
    {
        if (static_cast<word_t>(end_ - cur_) < offs)
        {
            set_file_ptr(get_file_ptr() - (end_ - cur_) + offs);
            fill_buffer();
        }
        else
            cur_ += offs;
    }

private:
    void fill_buffer()
    {
        end_ = (cur_ = buf_) + fill_buffer(buf_, BufferSize);
    }

    word_t fill_buffer(unsigned char *ptr, word_t len)
    {
        DWORD realSize;
        const BOOL ret = ::ReadFile(fh_, ptr, len, &realSize, 0);
        if (!ret)
            throw std::exception();
        return realSize;
    }

    unsigned char *cur_;
};

class buffered_output
    : public buffered_base
{
public:
    explicit buffered_output(const char *fname)
        : buffered_base(fname, true)
    {
    }

    explicit buffered_output(HANDLE fh)
        : buffered_base(fh)
    {
    }

    ~buffered_output()
    {
        flush_buffer();
    }

    void write_byte(unsigned char byte)
    {
        *end_++ = byte;
        if (end_ == buf_ + BufferSize)
            flush_buffer();
    }

    void write_word(word_t dw)
    {
        const unsigned char *b =
            reinterpret_cast<unsigned char*>(&dw);
        write_byte(b[0]);
        write_byte(b[1]);
        write_byte(b[2]);
        write_byte(b[3]);
#ifdef PATL_64
        write_byte(b[4]);
        write_byte(b[5]);
        write_byte(b[6]);
        write_byte(b[7]);
#endif
    }

    void write_string(const char *str)
    {
        for (; *str != 0; ++str)
            write_byte(*str);
    }

    void write_bytes(const unsigned char *ptr, word_t len)
    {
        const word_t
            avail = static_cast<word_t>(buf_ + BufferSize - end_),
            chunk = avail < len ? avail : len;
        std::copy(ptr, ptr + chunk, end_);
        end_ += chunk;
        len -= chunk;
        if (end_ == buf_ + BufferSize)
            flush_buffer();
        if (len)
            flush_buffer(ptr + chunk, len);
    }

    void move_offset(sword_t offs)
    {
        flush_buffer();
        set_file_ptr(get_file_ptr() + offs);
    }

    void flush_buffer()
    {
        flush_buffer(buf_, end_ - buf_);
        end_ = buf_;
    }

private:
    void flush_buffer(const unsigned char *ptr, word_t len)
    {
        DWORD realSize;
        const BOOL ret = ::WriteFile(fh_, ptr, len, &realSize, 0);
        if (!ret || realSize != len)
            throw std::exception();
    }
};

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
