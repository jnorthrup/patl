#ifndef BPATL_IMPL_EXTVERTEX_HPP
#define BPATL_IMPL_EXTVERTEX_HPP

#include "../../patl/impl/vertex.hpp"
#include "extalgorithm.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

class extvertex_generic
{
public:
    typedef patl::impl::vertex_generic exploit_t;
    typedef exploit_t const_exploit_t;

    const_exploit_t exploit() const
    {
        return const_exploit_t(pal_.exploit());
    }
    exploit_t exploit()
    {
        return exploit_t(pal_.exploit());
    }

private:
    extalgorithm pal_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
