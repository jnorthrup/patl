#ifndef PATL_IMPL_ASSOC_GENERIC_HPP
#define PATL_IMPL_ASSOC_GENERIC_HPP

#include "levelorder_iterator.hpp"
#include "preorder_iterator.hpp"
#include "postorder_iterator.hpp"
#include "iterator.hpp"
#include "partimator.hpp"
#include "prefix.hpp"
#include "vertex.hpp"

namespace uxn
{
namespace patl
{
namespace impl
{

#define SELF static_cast<this_t*>(this)
#define CSELF static_cast<const this_t*>(this)

template <typename This, typename T>
class assoc_generic
{
    typedef This this_t;

protected:
    typedef typename T::algorithm algorithm;
    typedef typename T::node_type node_type;

public:
    typedef prefix_generic<this_t, node_type> prefix;
    typedef vertex_generic<algorithm> vertex;
    typedef levelorder_iterator_generic<vertex> levelorder_iterator;
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

    assoc_generic()
        : root_(0)
    {
    }

    explicit assoc_generic(const bit_compare &bit_comp)
        : bit_comp_(bit_comp)
        , root_(0)
    {
    }

    typedef const_iterator_generic<vertex> const_iterator;

    typedef iterator_generic<vertex> iterator;

    // begin() declarations
    const_iterator begin() const
    {
        return const_iterator(*vertex(CSELF).postorder_begin());
    }
    iterator begin()
    {
        return iterator(*vertex(CSELF).postorder_begin());
    }

    // end() declarations
    const_iterator end() const
    {
        return const_iterator(*vertex(CSELF).postorder_end());
    }
    iterator end()
    {
        return iterator(*vertex(CSELF).postorder_end());
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

    // rbegin() declarations
    const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator(end());
    }
    reverse_iterator rbegin()
    {
        return reverse_iterator(end());
    }

    // rend() declarations
    const_reverse_iterator rend() const
    {
        return const_reverse_iterator(begin());
    }
    reverse_iterator rend()
    {
        return reverse_iterator(begin());
    }

    template <typename Decision>
    struct const_partimator
        : public const_partimator_generic<vertex, Decision>
    {
        explicit const_partimator(
            const Decision &decis = Decision(),
            const vertex &vtx = vertex())
            : const_partimator_generic(decis, vtx)
        {
        }

        const_partimator(const const_partimator_generic<vertex, Decision> &obj)
            : const_partimator_generic(obj)
        {
        }
    };

    template <typename Decision>
    struct partimator
        : public partimator_generic<vertex, Decision>
    {
        explicit partimator(
            const Decision &decis = Decision(),
            const vertex &vtx = vertex())
            : partimator_generic(decis, vtx)
        {
        }

        partimator(const partimator_generic<vertex, Decision> &obj)
            : partimator_generic(obj)
        {
        }
    };

    // begin() partimator declarations
    template <typename Decision>
    const_partimator<Decision> begin(const Decision &decis) const
    {
        vertex vtx(CSELF);
        if (root_)
            return ++const_partimator<Decision>(decis, vtx);
        else
            vtx.toggle();
        return const_partimator<Decision>(decis, vtx);
    }
    template <typename Decision>
    partimator<Decision> begin(const Decision &decis)
    {
        vertex vtx(CSELF);
        if (root_)
            return ++partimator<Decision>(decis, vtx);
        else
            vtx.toggle();
        return partimator<Decision>(decis, vtx);
    }

    // end() partimator declarations
    template <typename Decision>
    const_partimator<Decision> end(const Decision &decis) const
    {
        return const_partimator<Decision>(decis, vertex(root(), 1));
    }
    template <typename Decision>
    partimator<Decision> end(const Decision &decis)
    {
        return partimator<Decision>(decis, vertex(root(), 1));
    }

    // find(const key_type&) declarations
    const_iterator find(const key_type &key) const
    {
        vertex vtx(CSELF);
        // if number of first mismatching bit end at infinity
        if (root_ && ~word_t(0) == vtx.mismatch(key))
            // then return iterator on finding
            return const_iterator(vtx);
        return end();
    }
    iterator find(const key_type &key)
    {
        vertex vtx(CSELF);
        if (root_ && ~word_t(0) == vtx.mismatch(key))
            return iterator(vtx);
        return end();
    }

    const_iterator lower_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0)) const
    {
        vertex vtx(CSELF);
        // find a nearest match
        if (root_)
        {
            vtx.mismatch(key, prefixLen);
            vtx.descend<0>();
            return const_iterator(vtx);
        }
        return end();
    }
    iterator lower_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        vertex vtx(CSELF);
        // find a nearest match
        if (root_)
        {
            vtx.mismatch(key, prefixLen);
            vtx.descend<0>();
            return iterator(vtx);
        }
        return end();
    }

    const_iterator upper_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0)) const
    {
        vertex vtx(CSELF);
        if (root_)
        {
            if (vtx.mismatch(key, prefixLen) >= prefixLen)
                vtx.move<1>();
            vtx.descend<0>();
            return const_iterator(vtx);
        }
        return end();
    }
    iterator upper_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        vertex vtx(CSELF);
        if (root_)
        {
            if (vtx.mismatch(key, prefixLen) >= prefixLen)
                vtx.move<1>();
            vtx.descend<0>();
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
        word_t prefixLen = ~word_t(0)) const
    {
        vertex vtx(CSELF);
        if (root_)
        {
            const word_t len = vtx.mismatch(key, prefixLen);
            vertex lower(vtx);
            lower.descend<0>();
            if (len < prefixLen)
                return const_iter_range(
                    const_iterator(lower),
                    const_iterator(lower));
            vtx.move<1>();
            vtx.descend<0>();
            return const_iter_range(
                const_iterator(lower),
                const_iterator(vtx));
        }
        return const_iter_range(end(), end());
    }
    iter_range equal_range(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        vertex vtx(CSELF);
        if (root_)
        {
            const word_t len = vtx.mismatch(key, prefixLen);
            vertex lower(vtx);
            lower.descend<0>();
            if (len < prefixLen)
                return iter_range(
                    iterator(lower),
                    iterator(lower));
            vtx.move<1>();
            vtx.descend<0>();
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

    prefix root() const
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
