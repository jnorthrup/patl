#ifndef BPATL_IMPL_BLOCK_CACHE_HPP
#define BPATL_IMPL_BLOCK_CACHE_HPP

#include "node_block.hpp"
#include <deque>

namespace uxn
{
namespace bpatl
{
namespace impl
{

class block_cache
{
public:
    block_cache(word_t count)
        : blocks_(count)
    {
        for (word_t i = 0; i != count; ++i)
            free_.push_back(blocks_[i]);
    }

private:
    // хранилище блоков в кэше
    std::deque<node_block> blocks_;
    // свободные блоки
    std::vector<node_block*> free_;
    // по bid_type можно узнать расположение блока в кэше и его атрибуты
    std::map<
        node_block::bid_type,
        std::pair<
            node_block*,
            word_t /* fixed count and dirty in msb */> > map_;
    // reverse map - найти bid_type по заданному указателю на кэшированный блок
    // теперь можно по node_block* определить bid_type в extalgorithm_generic::release_provider
    std::map<node_block*, node_block::bid_type> rmap_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
