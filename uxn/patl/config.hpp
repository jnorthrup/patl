/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_CONFIG_HPP
#define PATL_CONFIG_HPP

#ifdef __GNUG__

# if defined(_LP64) \
     || defined(__LP64__)
#  define PATL_64
# else
#  define PATL_32
# endif

// TODO: consider using uintptr_t and intptr_t.
// TODO: use this typedefs unconditionally.
typedef size_t word_t;
typedef ssize_t sword_t;

#else // PATL_GCC

#ifdef _WIN64

#define PATL_64
typedef unsigned long long word_t;
typedef long long sword_t;

#elif _WIN32

#define PATL_32
typedef unsigned int word_t;
typedef int sword_t;

#else

#define PATL_NN
typedef undefined word_t;
typedef undefined sword_t;
#endif

#define PATL_INTRINSIC

#endif // PATL_GCC

const word_t
    bits_in_word = 8 * sizeof(word_t),
    highest_bit = word_t(1) << (bits_in_word - 1);

// Sedgewick's method for identifying backtrack links
//#define PATL_SEDGEWICK

#ifndef PATL_SEDGEWICK

// allow to store additional info in less significant bits in some pointers
// used in "impl/node.hpp" and "impl/algorithm.hpp"
#define PATL_ALIGNHACK

#endif

#ifdef _DEBUG

#define PATL_DEBUG

#undef PATL_ALIGNHACK
#undef PATL_INTRINSIC

#define PATL_ASSERT(x) assert(x)

#else

#define PATL_ASSERT(x)

#endif // _DEBUG

#endif
