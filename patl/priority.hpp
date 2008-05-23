#ifndef PATL_PRIORITY_HPP
#define PATL_PRIORITY_HPP

#include "trie_set.hpp"

namespace uxn
{
namespace patl
{

// class template unique priority queue adaptor for [trie_]set containers
template <typename Key, typename Cont = trie_set<Key> >
class unique_priority_queue
{
public:
    typedef typename Cont::value_type value_type;
    typedef typename Cont::size_type size_type;

    explicit unique_priority_queue(const Cont &set = Cont())
        : set_(set)
    {
    }

    void push(const value_type &val)
    {
        set_.insert(val);
    }

    void pop()
    {
        set_.erase(--set_.end());
    }

    bool empty() const
    {
        return set_.empty();
    }

    size_type size() const
    {
        return set_.size();
    }

    const value_type &top() const
    {
        return *--set_.end();
    }

protected:
    Cont set_;
};

} // namespace patl
} // namespace uxn

#endif
