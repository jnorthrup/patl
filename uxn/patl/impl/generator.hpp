/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_GENERATOR_HPP
#define PATL_IMPL_GENERATOR_HPP

// generator/continuation for C++
// author: Andrew Fedoniouk @ terrainformatica.com
// idea borrowed from: "coroutines in C" Simon Tatham, 
//                     http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
// from: http://www.rsdn.ru//forum/message/2965247.1.aspx
// patch by remark: http://www.rsdn.ru/forum/message/2967473.1.aspx
// ... and by R.K.: http://www.rsdn.ru/forum/message/2969898.1.aspx

namespace uxn
{
namespace patl
{
namespace impl
{

template<typename Derived, typename Elem>
class generator_base
{
public:
    typedef Derived this_t;
    typedef Elem elem_t;

    generator_base()
        : generator_line(0)
    {
    }

    this_t replica() const
    {
        return *static_cast<const this_t*>(this);
    }

    void init()
    {
        generator_line = 0;
    }

    bool next()
    {
        int generator_cur_line = generator_line;
        generator_line = 0;
        static_cast<this_t*>(this)->generate_value(generator_cur_line);
        return 0 != generator_line;
    }

    const elem_t &value()
    {
        return generator_value;
    }

protected:
    int generator_line;
    elem_t generator_value;
};

#define PATL_EMIT\
    void generate_value(int generator_cur_line)\
    { switch(generator_cur_line) { case 0:;\
/**/

#define PATL_STOP_EMIT }}

#define PATL_YIELD(V)\
        do {\
            this->generator_line=__LINE__;\
            this->generator_value = (V);\
            return;\
            case __LINE__:;\
        } while (0)\
/**/

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
