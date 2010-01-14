/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_LEAF_ORACLE_HPP
#define PATL_LEAF_ORACLE_HPP

namespace uxn
{
namespace patl
{

template <typename SuffixCont>
class leaf_oracle
{
    typedef SuffixCont suffix_cont;
    typedef typename suffix_cont::allocator_type allocator_type;
    typedef typename suffix_cont::const_vertex const_vertex;
    typedef typename suffix_cont::vertex vertex;
    typedef typename suffix_cont::const_iterator const_iterator;
    typedef typename suffix_cont::key_type key_type;

public:
    leaf_oracle(const suffix_cont &cont)
        : cont_(&cont)
        , unsigned_alloc_(allocator_type())
        , leaf_map_(0)
    {
        init();
    }

    ~leaf_oracle()
    {
        unsigned_alloc_.deallocate(leaf_map_, leaf_size_);
    }

    void init()
    {
        // deallocate old
        unsigned_alloc_.deallocate(leaf_map_, leaf_size_);
        leaf_map_ = 0;
        //
        if (!cont_->size())
            return;
        leaf_map_ = unsigned_alloc_.allocate(leaf_size_ = cont_->size());
        //
        for (const_iterator cit = cont_->begin()
            ; cit != cont_->end()
            ; ++cit)
            leaf_map_[cont_->index_of(cit)] = static_cast<const const_vertex&>(cit).compact();
    }

    const_vertex operator()(key_type suf) const
    {
        return const_vertex(cont_, leaf_map_[suf - cont_->keys()]);
    }

private:
    const suffix_cont *cont_;
    typename allocator_type::template rebind<word_t>::other unsigned_alloc_;
    word_t *leaf_map_;
    word_t leaf_size_;
};

} // namespace patl
} // namespace uxn

#endif
