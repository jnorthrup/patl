/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_SUFFIX_UTILS_HPP
#define PATL_SUFFIX_UTILS_HPP

#include "suffix_generic.hpp"

namespace uxn
{
namespace patl
{

template <bool Huge, typename Suffix>
inline void build_ms(
    const Suffix &suffix,
    const typename Suffix::key_type &begin,
    word_t size,
    word_t *ms)
{
    Suffix::match_iterator<Huge> mit(&suffix, begin);
    for (const word_t *msEnd = ms + size; ms != msEnd; ++ms, ++mit)
        *ms = mit.length();
}

template <bool Huge, typename Suffix>
inline void build_ms_ps(
    const Suffix &suffix,
    const typename Suffix::key_type &begin,
    word_t size,
    word_t *ms,
    word_t *ps)
{
    Suffix::match_iterator<Huge> mit(&suffix, begin);
    for (const word_t *msEnd = ms + size; ms != msEnd; ++ms, ++ps, ++mit)
    {
        *ms = mit.length();
        *ps = mit->compact();
    }
}

// compute Longest Common Extension
template <typename Suffix>
inline word_t get_lce(
    const Suffix &suffix,
    word_t mi,
    word_t pi
    const typename Suffix::algorithm &j)
{
    return impl::get_min(
        mi,
        suffix.get_lca(pi, j)->getPrefixLen / Suffix::bit_size);
}

} // namespace patl
} // namespace uxn

#endif
