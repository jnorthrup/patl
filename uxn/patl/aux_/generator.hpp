#ifndef PATL_AUX_GENERATOR_HPP
#define PATL_AUX_GENERATOR_HPP

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
namespace aux
{

template<typename derived_t, typename elem_t>
class generator_base
{
protected:
    int generator_line;
    elem_t generator_value;

public:
    generator_base()
        : generator_line()
    {}

    bool next()
    {
        int generator_cur_line = generator_line;
        generator_line = 0;
        static_cast<derived_t*>(this)->generate_value(generator_cur_line);
        return 0 != generator_line;
    }

    elem_t const& value()
    {
        return generator_value;
    }
};

#define PATL_GENERATOR(NAME, T, NAME_T) class NAME : public uxn::patl::aux::generator_base<NAME_T, T>

#define PATL_EMIT(T)\
    void generate_value(int generator_cur_line)\
    { switch(generator_cur_line) { case 0:;\
/**/

#define PATL_STOP_EMIT() }}

#define PATL_YIELD(V)\
        do {\
            this->generator_line=__LINE__;\
            this->generator_value = (V);\
            return;\
            case __LINE__:;\
        } while (0)\
/**/

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
