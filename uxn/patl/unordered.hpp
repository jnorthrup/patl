/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | Not finished yet...
-*/
#ifndef PATL_UNORDERED_HPP
#define PATL_UNORDERED_HPP

#include <algorithm>
#include "impl/trivial.hpp"
#include "impl/primes.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Key, typename Value>
class hash_node
{
    typedef Key key_type;
    typedef Value value_type;

public:
    word_t get_hash() const
    {
        return hash_;
    }

    word_t get_next() const
    {
        return next_;
    }

    const key_type &get_key() const
    {
        return value_.first;
    }

    const value_type &get_value() const
    {
        return value_;
    }
    value_type &get_value()
    {
        return value_;
    }

    void set_next(word_t next)
    {
        next_ = next;
    }

    void set_all(word_t hash, word_t next, const value_type &value)
    {
        hash_ = hash;
        next_ = next;
        value_ = value;
    }

private:
    word_t hash_;
    word_t next_;
    value_type value_;
};

} // namespace impl

template <typename Key>
struct hash
{
    word_t operator()(const Key &a) const
    {
        return static_cast<word_t>(a);
    }
};

template <>
struct hash<word_t>
{
    word_t operator()(word_t a) const
    {
        return a;
    }
};

template <
    typename Key,
    typename Datum,
    typename HashFun = hash<Key>,
    typename Pred = std::equal_to<Key>,
    typename Alloc = std::allocator<std::pair<const Key, Datum> > >
class unordered_map
{
    typedef Datum mapped_type;

public:
    typedef Key key_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef impl::hash_node<key_type, value_type> node_type;
    typedef HashFun hasher;
    typedef Pred key_equal;
    typedef Alloc allocator_type;
    typedef word_t size_type;

    explicit unordered_map(
        word_t capacity = 0, // min buckets count
        const hasher &hf = hasher(),
        const key_equal &eql = key_equal(),
        const allocator_type &alloc = allocator_type())
        : hf_(hf)
        , eql_(eql)
        , alloc_(alloc)
        , size_(0)
        , free_size_(0)
    {
        init(capacity);
    }

    bool empty() const
    {
        return !size();
    }

    word_t size() const
    {
        return size_ - free_size_;
    }

    void clear()
    {
        if (size_)
        {
            std::fill(buckets_.begin(), buckets_.end(), ~word_t(0));
            std::fill(nodes_.begin(), nodes_.end(), node_type()); // ???
            free_list_ = ~word_t(0);
            free_size_ = size_ = 0;
        }
    }

    mapped_type &operator[](const key_type &key)
    {
        if (buckets_.empty())
            init(0);
        const word_t num = hf_(key) & 0x7FFFFFFF;
        word_t index = num % buckets_.size();
        for (word_t i = buckets_[index]; i != ~word_t(0); i = nodes_[i].get_next())
        {
            if (nodes_[i].get_hash() == num && eql_(nodes_[i].get_key(), key))
                return nodes_[i].get_value().second;
        }
        word_t free_list;
        if (free_size_ > 0)
        {
            free_list = free_list_;
            free_list_ = nodes_[free_list].get_next();
            --free_size_;
        }
        else
        {
            if (size_ == nodes_.size())
            {
                resize();
                index = num % buckets_.size();
            }
            free_list = size_++;
        }
        nodes_[free_list].set_all(num, buckets_[index], std::make_pair(key, mapped_type()));
        return nodes_[free_list].get_value().second;
    }

    size_type erase(const key_type &key)
    {
        if (!buckets_.empty())
        {
            const word_t num = hf_(key) & 0x7FFFFFFF;
            word_t index = num % buckets_.size();
            word_t num3 = ~word_t(0);
            for (word_t i = buckets_[index]; i != ~word_t(0); i = nodes_[i].get_next())
            {
                if (nodes_[i].get_hash() == num && eql_(nodes_[i].get_key(), key))
                {
                    if (num3 == ~word_t(0))
                        buckets_[index] = nodes_[i].get_next();
                    else
                        nodes_[num3].set_next(nodes_[i].get_next());
                    nodes_[i].set_all(~word_t(0), free_list_, value_type());
                    free_list_ = i;
                    ++free_size_;
                    return 1;
                }
                num3 = i;
            }
        }
        return 0;
    }

    class iterator
    {
        typedef unordered_map<Key, Datum, HashFun, Pred, Alloc> cont_type;

    public:

    private:
        value_type *current_;
        const cont_type *cont_;
        word_t index_;
    };

private:
    void init(word_t capacity)
    {
        impl::primes<word_t> ps(capacity);
        ps.next();
        const word_t prime = ps.value();
        buckets_.resize(prime, ~word_t(0));
        nodes_.resize(prime);
        free_list_ = ~word_t(0);
    }

    void resize()
    {
        impl::primes<word_t> ps(size_ * 2);
        ps.next();
        const word_t prime = ps.value();
        std::vector<word_t> num_arr(prime, ~word_t(0));
        std::vector<node_type> dest_arr(prime);
        std::copy(nodes_.begin(), nodes_.begin() + size_, dest_arr);
        for (word_t j = 0; j != size_; ++j)
        {
            const word_t index = dest_arr[j].get_hash() % prime;
            dest_arr[j].set_next(num_arr[index]);
            num_arr[index] = j;
        }
        std::swap(buckets_, num_arr);
        std::swap(nodes_, dest_arr);
    }

    hasher hf_;
    key_equal eql_;
    allocator_type alloc_;
    std::vector<sword_t> buckets_;
    word_t size_;
    std::vector<node_type> nodes_;
    word_t free_size_;
    word_t free_list_;
};

} // namespace patl
} // namespace uxn

#endif
