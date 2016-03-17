/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_ASSOC_GENERIC_HPP
#define PATL_IMPL_ASSOC_GENERIC_HPP

#include "preorder_iterator.hpp"
#include "postorder_iterator.hpp"
#include "iterator.hpp"
#include "partimator.hpp"
#include "vertex.hpp"
#include <map>

namespace uxn
{
namespace patl
{
namespace impl
{
/*
// TODO
template <typename T>
class shortcut_generic
{
};*/

#define SELF static_cast<this_t*>(this)
#define CSELF static_cast<const this_t*>(this)

template <typename This, typename T, word_t N = 0>
class assoc_generic
    : public assoc_generic<This, T, 0>
{
    using this_t = This;
    using super = assoc_generic<This, T, 0>;

protected:
    using algorithm = typename T::algorithm;
    using node_type = typename T::node_type;

public:
    using prefix = typename T::prefix;
    using vertex = vertex_generic<algorithm>;
    using preorder_iterator = preorder_iterator_generic<vertex>;
    using postorder_iterator = postorder_iterator_generic<vertex>;
    using key_type = typename T::key_type;
    using value_type = typename T::value_type;
    using bit_compare = typename T::bit_compare;
    using allocator_type = typename T::allocator_type;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = typename allocator_type::difference_type;
    using size_type = word_t;
    //using shortcut = shortcut_generic<T>; // TODO

    using const_iterator = const_iterator_generic<vertex>;
    using iterator = iterator_generic<vertex>;

    static const word_t N_ = N;

    assoc_generic()
        : super(0)
    {
    }

    explicit assoc_generic(const bit_compare &bit_comp)
        : super(bit_comp)
    {
    }

protected:
    word_t *get_shortcut(word_t a) const
    {
        const shortcuts_t::const_iterator it(shortcuts_.find(a));
        return a == shortcuts_.end() ? 0 : it->second;
    }

    word_t *create_shortcut(word_t a)
    {
        word_t *sh = new word_t [1 << N]; // заменить на аллокатор
        shortcuts_.insert(std::make_pair(a, sh));
        return sh;
    }

private:
    typedef std::map<word_t, word_t*> shortcuts_t;
    // отображение algorithm::compact на массив компактов длиной (1 << N)
    shortcuts_t shortcuts_;
};

template <typename This, typename T>
class assoc_generic<This, T, 0>
{
    using this_t = This;

protected:
    using algorithm = typename T::algorithm;
    using node_type = typename T::node_type;

public:
    using prefix = typename T::prefix;
    using const_vertex = const_vertex_generic<algorithm>;
    using vertex = vertex_generic<algorithm>;
    using const_preorder_iterator = const_preorder_iterator_generic<const_vertex>;
    using const_postorder_iterator = const_postorder_iterator_generic<const_vertex>;
    using preorder_iterator = preorder_iterator_generic<vertex>;
    using postorder_iterator = postorder_iterator_generic<vertex>;
    using key_type = typename T::key_type;
    using value_type = typename T::value_type;
    using bit_compare = typename T::bit_compare;
    using allocator_type = typename T::allocator_type;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using reference = value_type&;
    using difference_type = typename allocator_type::difference_type;
    using size_type = word_t;

    static const word_t N_ = 0;

    assoc_generic()
        : root_(0)
    {
    }

    explicit assoc_generic(const bit_compare &bit_comp)
        : root_(0)
        , bit_comp_(bit_comp)
    {
    }

    using const_iterator = const_iterator_generic<const_postorder_iterator>;
    using iterator = iterator_generic<postorder_iterator>;

    const_iterator begin() const
    {
        return root().begin();
    }
    iterator begin()
    {
        return root().begin();
    }

    const_iterator end() const
    {
        return root().end();
    }
    iterator end()
    {
        return root().end();
    }

    class const_reverse_iterator
        : public std::reverse_iterator<const_iterator>
    {
    public:
        explicit const_reverse_iterator(const const_iterator &cit = const_iterator())
            : std::reverse_iterator<const_iterator>(cit)
        {
        }
    };

    class reverse_iterator
        : public std::reverse_iterator<iterator>
    {
    public:
        explicit reverse_iterator(const iterator &it = iterator())
            : std::reverse_iterator<iterator>(it)
        {
        }
    };

    const_reverse_iterator rbegin() const
    {
        return root().rbegin();
    }
    reverse_iterator rbegin()
    {
        return root().rbegin();
    }

    const_reverse_iterator rend() const
    {
        return root().rend();
    }
    reverse_iterator rend()
    {
        return root().rend();
    }

    template <typename Decision>
    using partimator = partimator_generic<preorder_iterator, Decision>;

    template <typename Decision>
    using const_partimator = const_partimator_generic<const_preorder_iterator, Decision>;

    template <typename Decision>
    const_partimator<Decision> begin(const Decision &decis) const
    {
        const_vertex vtx(root());
        if (!root_)
            vtx.toggle();
        return const_partimator<Decision>(decis, vtx);
    }
    template <typename Decision>
    partimator<Decision> begin(const Decision &decis)
    {
        vertex vtx(root());
        if (!root_)
            vtx.toggle();
        return partimator<Decision>(decis, vtx);
    }

    template <typename Decision>
    const_partimator<Decision> end(const Decision &decis) const
    {
        const_vertex vtx(root());
        vtx.toggle();
        return const_partimator<Decision>(decis, vtx);
    }
    template <typename Decision>
    partimator<Decision> end(const Decision &decis)
    {
        vertex vtx(root());
        vtx.toggle();
        return partimator<Decision>(decis, vtx);
    }

    const_iterator find(const key_type &key) const
    {
        const_vertex vtx(root());
        // if number of first mismatching bit end at infinity
        if (root_ && ~word_t(0) == vtx.mismatch(key))
            // then return iterator on finding
            return const_iterator(vtx);
        return end();
    }
    iterator find(const key_type &key)
    {
        vertex vtx(root());
        if (root_ && ~word_t(0) == vtx.mismatch(key))
            return iterator(vtx);
        return end();
    }

    const_iterator lower_bound(const key_type &key, word_t prefixLen = ~word_t(0)) const
    {
        const_vertex vtx(root());
        // find a nearest match
        if (root_)
        {
            vtx.mismatch(key, prefixLen);
            vtx.template descend<0>();
            return const_iterator(vtx);
        }
        return end();
    }
    iterator lower_bound(const key_type &key, word_t prefixLen = ~word_t(0))
    {
        vertex vtx(root());
        // find a nearest match
        if (root_)
        {
            vtx.mismatch(key, prefixLen);
            vtx.template descend<0>();
            return iterator(vtx);
        }
        return end();
    }

    const_iterator upper_bound(const key_type &key, word_t prefixLen = ~word_t(0)) const
    {
        const_vertex vtx(root());
        if (root_)
        {
            if (vtx.mismatch(key, prefixLen) >= prefixLen)
                vtx.template move<1>();
            vtx.template descend<0>();
            return const_iterator(vtx);
        }
        return end();
    }
    iterator upper_bound(const key_type &key, word_t prefixLen = ~word_t(0))
    {
        vertex vtx(root());
        if (root_)
        {
            if (vtx.mismatch(key, prefixLen) >= prefixLen)
                vtx.template move<1>();
            vtx.template descend<0>();
            return iterator(vtx);
        }
        return end();
    }

private:
    using const_iter_range = std::pair<const_iterator, const_iterator>;
    using iter_range = std::pair<iterator, iterator>;

public:
    const_iter_range equal_range(const key_type &key, word_t prefix_len = ~word_t(0)) const
    {
        const_vertex vtx(root());
        if (root_)
        {
            const word_t len = vtx.mismatch(key, prefix_len);
            vertex lower(vtx);
            lower.template descend<0>();
            if (len < prefix_len)
                return const_iter_range(const_iterator(lower), const_iterator(lower));
            vtx.template move<1>();
            vtx.template descend<0>();
            return const_iter_range(const_iterator(lower), const_iterator(vtx));
        }
        return const_iter_range(end(), end());
    }
    iter_range equal_range(const key_type &key, word_t prefix_len = ~word_t(0))
    {
        vertex vtx(root());
        if (root_)
        {
            const word_t len = vtx.mismatch(key, prefix_len);
            vertex lower(vtx);
            lower.template descend<0>();
            if (len < prefix_len)
                return iter_range(iterator(lower), iterator(lower));
            vtx.template move<1>();
            vtx.template descend<0>();
            return iter_range(iterator(lower), iterator(vtx));
        }
        return iter_range(end(), end());
    }

    size_type count(const key_type &key, word_t prefixLen = ~word_t(0)) const
    {
        const_iter_range pcc(equal_range(key, prefixLen));
        return std::distance(pcc.first, pcc.second);
    }

    void change_root(vertex &vtx)
    {
        node_type *p = vtx.get_p();
        if (p != root_)
        {
            p->make_root(root_);
            root_ = p;
        }
    }

    bool empty() const
    {
        return !root_;
    }

    const_vertex root() const
    {
        return const_vertex(CSELF, root_, 0);
    }
    vertex root()
    {
        return vertex(CSELF, root_, 0);
    }

    prefix root_prefix() const
    {
        return prefix(CSELF, root_);
    }

    // bit_comp() declaration (like key_comp() in standard assoc containers)
    bit_compare bit_comp() const
    {
        return bit_comp_;
    }

protected:
    node_type *root_;
    bit_compare bit_comp_;
};

#undef SELF
#undef CSELF

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
