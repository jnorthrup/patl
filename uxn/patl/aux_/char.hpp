/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_CHAR_HPP
#define PATL_AUX_CHAR_HPP

namespace uxn
{
namespace patl
{
namespace aux
{

inline bool is_blank_char(unsigned char c)
{
    return c <= 0x20;
}

inline bool is_latin_letter(unsigned char c)
{
    return 0x41 <= c && c <= 0x5A || 0x61 <= c && c <= 0x7A;
}

inline bool is_cyrillic_letter(unsigned char c)
{
    return 0xC0 <= c || c == 0xA8 || c == 0xB8;
}

inline bool is_proper_char(unsigned char c)
{
    return 0x21 <= c && c <= 0x7E || is_cyrillic_letter(c);
}

inline bool is_letter(unsigned char c)
{
    return is_cyrillic_letter(c) || is_latin_letter(c);
}

// дефис ли символ?
inline bool is_hyphen(unsigned char c)
{
    return c == '-';
}

inline bool is_punctuation(unsigned char c)
{
    return 0x21 <= c && c <= 0x2F || 0x3A <= c && c <= 0x3F;
}

inline bool is_numeric(unsigned char c)
{
    return 0x30 <= c && c <= 0x39;
}

inline bool is_dot_symbol(unsigned char c)
{
    return c == '!' || c == '.' || c == '?';
}

inline unsigned char to_lower(unsigned char c)
{
    return c + (0x41 <= c && c <= 0x5A || 0xC0 <= c && c <= 0xDF ? 0x20 : 0);
}

inline unsigned char to_upper(unsigned char c)
{
    return c - (0x61 <= c && c <= 0x7A || 0xE0 <= c ? 0x20 : 0);
}

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
