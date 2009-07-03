#ifndef BPATL_IMPL_EXTVERTEX_HPP
#define BPATL_IMPL_EXTVERTEX_HPP

#include "../../patl/impl/vertex.hpp"
#include "extalgorithm.hpp"
#include <memory>

namespace uxn
{
namespace bpatl
{
namespace impl
{

template <typename ExtAlgorithm, typename Vertex>
class const_extvertex_generic
{
    typedef ExtAlgorithm extalgorithm;

public:
    typedef Vertex exploit_t;
    typedef exploit_t const_exploit_t;

    // тут должны быть кторы

    std::auto_ptr<const_exploit_t> exploit() const
    {
        return std::auto_ptr<const_exploit_t>(
            new const_exploit_t(pal_.exploit()));
    }

protected:
    extalgorithm pal_;
};

template <typename ExtAlgorithm, typename Vertex>
class extvertex_generic
    : public const_extvertex_generic<ExtAlgorithm, Vertex>
{
protected:
    typedef const_extvertex_generic<ExtAlgorithm, Vertex> super;
    typedef extvertex_generic<ExtAlgorithm, Vertex> this_t;

public:
    //typedef super const_extvertex; // хм... мне одному кажется, что здесь нечисто?
    typedef Vertex exploit_t;
    typedef typename exploit_t::const_vertex const_exploit_t;

    std::auto_ptr<exploit_t> exploit()
    {
        return std::auto_ptr<exploit_t>(
            new exploit_t(this->pal_.exploit());
    }
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
