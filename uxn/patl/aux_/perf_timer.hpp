/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_PERF_TIMER_HPP
#define PATL_AUX_PERF_TIMER_HPP

#if _WIN32
#include <windows.h>
#else
#include <cinttypes>
#define __int64 uint64_t
#endif

namespace uxn
{
namespace patl
{
namespace aux
{

class performance_timer
{
public:
    performance_timer()
        : start_(0)
        , finish_(0)
    {
        query_frequency();
        start();
    }

    void start()
    {
#if _WIN32
        LARGE_INTEGER large;
        QueryPerformanceCounter(&large);
        start_ = large.QuadPart;
#endif
    }

    void finish()
    {
#if _WIN32
        LARGE_INTEGER large;
        QueryPerformanceCounter(&large);
        finish_ = large.QuadPart;
#endif
    }

    double get_seconds() const
    {
        return
            static_cast<double>(finish_ - start_) /
            static_cast<double>(frequency_);
    }

private:
    static void query_frequency()
    {
#if _WIN32
        if (frequency_ == 0)
        {
            LARGE_INTEGER large;
            QueryPerformanceFrequency(&large);
            frequency_ = large.QuadPart;
        }
#endif
    }

    static __int64 frequency_;
    __int64 start_;
    __int64 finish_;
};

__int64 performance_timer::frequency_ = 0;

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
