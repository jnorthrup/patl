#include <uxn/bpatl/trie_map.hpp>

namespace bpatl = uxn::bpatl;

inline double some_value(word_t i, word_t j)
{
    return i * j;
}

inline void do_something()
{
}

template <typename T>
inline void do_something1(const T&, const T&)
{
}

struct node
{
    word_t a_;
    unsigned char b_;
};

int main()
{
#if 0
    typedef bpatl::trie_map<char*, int> btrie;
    btrie bt;
    //bt.insert("balon");
#elif 0
    typedef stxxl::typed_block<1 << 16, double> block_type;
    std::vector<block_type::bid_type> bids;
    std::vector<stxxl::request_ptr> requests;
    stxxl::block_manager *bm = stxxl::block_manager::get_instance();
    bm->new_blocks<block_type>(32, stxxl::striping(), std::back_inserter(bids));
    std::vector<block_type, stxxl::new_alloc<block_type> > blocks(32);
    for (word_t i = 0; i != 32; ++i)
    {
        printf("block #%u, size: %u, raw_size: %u\n", i, block_type::size, block_type::raw_size);
        for (word_t j = 0; j != block_type::size; ++j)
            blocks[i][j] = some_value(i, j);
    }
    for (word_t i = 0; i != 32; ++i)
        requests.push_back(blocks[i].write(bids[i]));
    do_something();
    wait_all(requests.begin(), requests.end());
    do_something1(bids.begin(), bids.end());
    bm->delete_blocks(bids.begin(), bids.end());
#else
    typedef bpatl::impl::node_block<1 << 16, node> nb_type;
    nb_type nb;
#endif
}
