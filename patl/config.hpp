#ifndef PATL_CONFIG_HPP
#define PATL_CONFIG_HPP

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

const word_t bits_in_word = 8 * sizeof(word_t);

// allow to store additional info in less significant bit in some pointers
// used in "impl/node.hpp" and "impl/algorithm.hpp"
#define PATL_ALIGNHACK

#define PATL_INTRINSIC

#ifdef _DEBUG

#undef PATL_ALIGNHACK
#undef PATL_INTRINSIC

#endif

#endif
