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
    typedef This this_t;
    typedef assoc_generic<This, T, 0> super;

protected:
    typedef typename T::algorithm algorithm;
    typedef typename T::node_type node_type;

public:
    typedef typename T::prefix prefix;
    typedef vertex_generic<algorithm> vertex;
    typedef preorder_iterator_generic<vertex> preorder_iterator;
    typedef postorder_iterator_generic<vertex> postorder_iterator;
    typedef typename T::key_type key_type;
    typedef typename T::value_type value_type;
    typedef typename T::bit_compare bit_compare;
    typedef typename T::allocator_type allocator_type;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef typename allocator_type::difference_type difference_type;
    typedef word_t size_type;
    //typedef shortcut_generic<T> shortcut; // TODO

    typedef const_iterator_generic<vertex> const_iterator;
    typedef iterator_generic<vertex> iterator;

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
    typedef This this_t;

protected:
    typedef typename T::algorithm algorithm;
    typedef typename T::node_type node_type;

public:
    typedef typename T::prefix prefix;
    typedef const_vertex_generic<algorithm> const_vertex;
    typedef vertex_generic<algorithm> vertex;
    typedef const_preorder_iterator_generic<const_vertex> const_preorder_iterator;
    typedef const_postorder_iterator_generic<const_vertex> const_postorder_iterator;
    typedef preorder_iterator_generic<vertex> preorder_iterator;
    typedef postorder_iterator_generic<vertex> postorder_iterator;
    typedef typename T::key_type key_type;
    typedef typename T::value_type value_type;
    typedef typename T::bit_compare bit_compare;
    typedef typename T::allocator_type allocator_type;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef typename allocator_type::difference_type difference_type;
    typedef word_t size_type;

    static const word_t N_ = 0;

    assoc_generic()
        : root_(0)
    {
    }

    explicit assoc_generic(const bit_compare &bit_comp)
        : bit_comp_(bit_comp)
        , root_(0)
    {
    }

    typedef const_iterator_generic<const_postorder_iterator> const_iterator;
    typedef iterator_generic<postorder_iterator> iterator;

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
    class partimator
        : public partimator_generic<preorder_iterator, Decision>
    {
        typedef partimator_generic<preorder_iterator, Decision> super;

    public:
        explicit partimator(
            const Decision &decis = Decision(),
            const vertex &vtx = vertex())
            : super(decis, vtx)
        {
        }

        partimator(const super &obj)
            : super(obj)
        {
        }
    };

    template <typename Decision>
    class const_partimator
        : public const_partimator_generic<const_preorder_iterator, Decision>
    {
        typedef const_partimator_generic<const_preorder_iterator, Decision> super;
        typedef const_partimator<Decision> this_t;

    public:
        explicit const_partimator(
            const Decision &decis = Decision(),
            const const_vertex &vtx = const_vertex())
            : super(decis, vtx)
        {
        }

        const_partimator(const super &obj)
            : super(obj)
        {
        }

        const_partimator(const partimator<Decision> &obj)
            : super(obj)
        {
        }
/*
        this_t &operator=(const super &obj)
        {
            if (&obj != this)
                *static_cast<super*>(this) = obj;
            return *this;
        }*/
    };

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

    // find(const key_type&) declarations
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

    const_iterator lower_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0)) const
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
    iterator lower_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
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

    const_iterator upper_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0)) const
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
    iterator upper_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
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
    typedef std::pair<const_iterator, const_iterator> const_iter_range;
    typedef std::pair<iterator, iterator> iter_range;

public:
    const_iter_range equal_range(
        const key_type &key,
        word_t prefix_len = ~word_t(0)) const
    {
        const_vertex vtx(root());
        if (root_)
        {
            const word_t len = vtx.mismatch(key, prefix_len);
            vertex lower(vtx);
            lower.template descend<0>();
            if (len < prefix_len)
                return const_iter_range(
                    const_iterator(lower),
                    const_iterator(lower));
            vtx.template move<1>();
            vtx.template descend<0>();
            return const_iter_range(
                const_iterator(lower),
                const_iterator(vtx));
        }
        return const_iter_range(end(), end());
    }
    iter_range equal_range(
        const key_type &key,
        word_t prefix_len = ~word_t(0))
    {
        vertex vtx(root());
        if (root_)
        {
            const word_t len = vtx.mismatch(key, prefix_len);
            vertex lower(vtx);
            lower.template descend<0>();
            if (len < prefix_len)
                return iter_range(
                    iterator(lower),
                    iterator(lower));
            vtx.template move<1>();
            vtx.template descend<0>();
            return iter_range(
                iterator(lower),
                iterator(vtx));
        }
        return iter_range(end(), end());
    }

    size_type count(const key_type &key, word_t prefixLen = ~word_t(0)) const
    {
        const_iter_range pcc(equal_range(key, prefixLen));
        return std::distance(pcc.first, pcc.second);
    }

    // change root
    void change_root(vertex &vtx)
    {
        node_type *p = static_cast<algorithm&>(vtx).get_p();
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
