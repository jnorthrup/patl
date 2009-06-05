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
    std::deque<node_block> blocks_;
    std::vector<node_block*> free_;
    std::map<
        node_block::bid_type,
        std::pair<
            node_block*,
            word_t /* fixed count and dirty in msb */> > map_;
};

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
