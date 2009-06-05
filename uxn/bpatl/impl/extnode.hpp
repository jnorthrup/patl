#ifndef BPATL_IMPL_EXTNODE_HPP
#define BPATL_IMPL_EXTNODE_HPP

#include "node_block.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

/// external memory node_generic
template <typename Container>
class extnode_generic
{
    typedef Container cont_type;
    typedef typename cont_type::node_block node_block;
    typedef typename node_block::outref outref_t;

public:

private:
    cont_type *cont_;
    outref_t outref_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
