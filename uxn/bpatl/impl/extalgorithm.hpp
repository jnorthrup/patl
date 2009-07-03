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
    typedef typename cont_type::coralgorithm coralgorithm; 

public:
    typedef cont_type::coralgorithm exploit_t; // avatar of external_algorithm
    typedef exploit_t const_exploit_t;

    std::auto_ptr<const_exploit_t> exploit() const
    {
        // кто должен запрашивать у кэша блок с узлом для работы?
        // скорее всего, external_algorithm, потому что он же будет отпускать этот блок
        // и запрашивать/отпускать новые блоки при навигации по узлам
        return std::auto_ptr<const_exploit_t>(
            new const_exploit_t(this, ));
    }

    // эта функция будет отличаться от предыдущей не только возвращаемым значением,
    // но и способом захвата участка кэша с установкой флага dirty
    std::auto_ptr<exploit_t> exploit()
    {
        return std::auto_ptr<exploit_t>(
            new exploit_t(this, ));
    }

    // этот класс является ContainerProvider'ом для algorithm_generic
    const cont_type *cont() const
    {
        return cont_;
    }
    cont_type *cont()
    {
        return cont_;
    }

    void release_provider(const_exploit_t *cor)
    {
        // TODO преобразовать состояние cor в состояние this->outref_
        // node_block нам известен, так же как и node_type*
        // неплохо бы иметь возможность по этой информации получить outref
        // этим может заняться block_cache
        const_node_type_ref q(cor->get_q());
        const node_block *block = q.get_block();
        outref_ = cont_->get_bid_type_by_block(block); // эта функция зовется из block_cache через cont
        outref_.set_offsetid(block->get_offset(q.get_node()), cor->get_qid());
    }

private:
    cont_type *cont_;
    outref_t outref_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
