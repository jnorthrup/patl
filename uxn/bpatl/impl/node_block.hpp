#ifndef BPATL_IMPL_NODE_BLOCK_HPP
#define BPATL_IMPL_NODE_BLOCK_HPP

#include "../../patl/config.hpp"

namespace uxn
{
namespace bpatl
{
namespace impl
{

struct block_info
{
    word_t free_begin_, free_end_;
    word_t node_alloc_, outref_alloc_;
    word_t hole_count_; // сумма дыр с обеих сторон
    //word_t flags_; // флаг readonly должен быть в кэше узлов блоков
};

template <word_t RawSize, typename NodeType>
class node_block
    : public stxxl::typed_block<RawSize, word_t, 0, block_info>
{
    typedef stxxl::typed_block<RawSize, word_t, 0, block_info> super;
    typedef NodeType node_type;

    static const word_t sizeof_node_;
    static const word_t sizeof_outref_;

public:
    typedef typename super::bid_type bid_type;

    // ссылка на узел во внешнем блоке узлов
    // будет использоваться как указатель на узел
    // ??? должен быть внешним классом
    // наследует block-id (bid_type)
    // содержит смещение узла в блоке
    // опционально algorithm::qid в msb
    class outref
        : public bid_type
    {
    public:
        word_t get_highest_bit() const
        {
            return item_off_ >> bits_in_word - 1;
        }

        void set_offsetid(word_t item_off, word_t id)
        {
            item_off_ = item_off | (id << bits_in_word - 1);
        }

    private:
        word_t item_off_; // most significant bit used for id
    };

    node_block()
    {
        info.free_begin_ = 0;
        info.free_end_ = size;
        info.node_alloc_ = info.free_begin_;
        info.outref_alloc_ = info.free_end_ - sizeof_outref_;
        info.hole_count_ = 0;
    }

    bool full() const
    {
        return
            info.free_end_ - info.free_begin_ + hole_count_ <
            sizeof_node + 4 * sizeof_outref_;
    }

    bool is_node(word_t off) const
    {
        return off < info.free_begin_;
    }

    bool is_outref(word_t off) const
    {
        return off >= info.free_end_;
    }

    const node_type *get_node(word_t off) const
    {
        return reinterpret_cast<const node_type*>(&(*this)[off]);
    }
    node_type *get_node(word_t off)
    {
        return reinterpret_cast<node_type*>(&(*this)[off]);
    }

    word_t get_offset(const node_type *nod) const
    {
        return reinterpret_cast<word_t*>(nod) - &(*this)[0];
    }

    const node_type *get_outref(word_t off) const
    {
        return reinterpret_cast<const outref*>(&(*this)[off]);
    }
    node_type *get_outref(word_t off)
    {
        return reinterpret_cast<outref*>(&(*this)[off]);
    }

    word_t new_node()
    {
        const word_t off = info.node_alloc_;
        if (info.node_alloc_ == info.free_begin_)
        {
            info.node_alloc_ += sizeof_node_;
            info.free_begin_ += sizeof_node_;
        }
        else
        {
            info.node_alloc_ = (*this)[info.node_alloc_];
            --info.hole_count_;
        }
        return off;
    }

    word_t new_outref()
    {
        const word_t off = info.outref_alloc_;
        if (info.outref_alloc_ == info.free_end_ - sizeof_outref_)
        {
            info.outref_alloc_ -= sizeof_outref_;
            info.free_end_ -= sizeof_outref_;
        }
        else
        {
            info.outref_alloc_ = (*this)[info.outref_alloc_];
            --info.hole_count_;
        }
        return off;
    }

    void del_node(word_t off)
    {
        if (off + sizeof_node_ == info.free_begin_)
        {
            if (info.node_alloc_ == info.free_begin_)
                info.node_alloc_ -= sizeof_node_;
            info.free_begin_ -= sizeof_node_;
        }
        else
        {
            (*this)[off] = info.node_alloc_;
            info.node_alloc_ = off;
            ++info.hole_count_;
        }
    }

    void del_outref(word_t off)
    {
        if (off - sizeof_outref_ == info.free_end_)
        {
            if (info.outref_alloc_ == info.free_end_ - sizeof_outref_)
                info.outref_alloc_ += sizeof_outref_;
            info.free_end_ += sizeof_outref_;
        }
        else
        {
            (*this)[off] = info.outref_alloc_;
            info.outref_alloc_ = off;
            ++info.hole_count_;
        }
    }
};

template <word_t RawSize, typename NodeType>
const word_t node_block<RawSize, NodeType>::sizeof_node_ =
    patl::impl::align_up<sizeof(word_t)>(sizeof(node_type)) / sizeof(word_t);
template <word_t RawSize, typename NodeType>
const word_t node_block<RawSize, NodeType>::sizeof_outref_ =
    patl::impl::align_up<sizeof(word_t)>(sizeof(outref)) / sizeof(word_t);

} // namespace impl
} // namespace bpatl
} // namespace uxn

#endif
