#ifndef BPATL_IMPL_EXTALGORITHM_HPP
#define BPATL_IMPL_EXTALGORITHM_HPP

#include "node_block.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

template <typename Container>
class extalgorithm_generic
{
    typedef Container cont_type;
    typedef typename cont_type::node_block node_block;
    typedef typename node_block::outref outref_t;
    typedef typename cont_type::coralgorithm coralgorithm; // avatar of external_algorithm

public:
    void release_provider(const coralgorithm *cor)
    {
        // TODO преобразовать состояние cor в состояние this->outref_
        // node_block нам известен, так же как и 
    }

private:
    cont_type *cont_;
    outref_t outref_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
