#ifndef PATL_IMPL_ASSOC_GENERIC_HPP
#define PATL_IMPL_ASSOC_GENERIC_HPP

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

    postorder_iterator postorder_begin() const
    {
        vertex vtx(CSELF, root_, 0);
        if (root_)
            vtx.descend(0);
        else
            vtx.toggle();
        return postorder_iterator(vtx);
    }

    postorder_iterator postorder_end() const
    {
        return postorder_iterator(vertex(CSELF, root_, 1));
    }

    preorder_iterator preorder_begin() const
    {
        return preorder_iterator(vertex(CSELF, root_, root_ ? 0 : 1));
    }

    preorder_iterator preorder_end() const
    {
        return preorder_iterator(vertex(CSELF, root_, 1));
    }

    typedef const_iterator<vertex> const_iterator;

    typedef iterator<vertex> iterator;

    // begin() declarations
    const_iterator begin() const
    {
        return const_iterator(*postorder_begin());
    }
    iterator begin()
    {
        return iterator(*postorder_begin());
    }

    // end() declarations
    const_iterator end() const
    {
        return const_iterator(*postorder_end());
    }
    iterator end()
    {
        return iterator(*postorder_end());
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
        : public const_partimator_generic<algorithm, Decision>
    {
        explicit const_partimator(
            const Decision &decis = Decision(),
            const algorithm &pal = algorithm())
            : const_partimator_generic(decis, pal)
        {
        }

        const_partimator(const const_partimator_generic<algorithm, Decision> &obj)
            : const_partimator_generic(obj)
        {
        }
    };

    template <typename Decision>
    struct partimator
        : public partimator_generic<algorithm, Decision>
    {
        explicit partimator(
            const Decision &decis = Decision(),
            const algorithm &pal = algorithm())
            : partimator_generic(decis, pal)
        {
        }

        partimator(const partimator_generic<algorithm, Decision> &obj)
            : partimator_generic(obj)
        {
        }
    };

    // begin() partimator declarations
    template <typename Decision>
    const_partimator<Decision> begin(const Decision &decis) const
    {
        algorithm pal(CSELF, root_, 0);
        if (root_)
            pal.descend_decision(0, decis);
        return const_partimator<Decision>(decis, pal);
    }
    template <typename Decision>
    partimator<Decision> begin(const Decision &decis)
    {
        algorithm pal(CSELF, root_, 0);
        if (root_)
            pal.descend_decision(0, decis);
        return partimator<Decision>(decis, pal);
    }

    // end() partimator declarations
    template <typename Decision>
    const_partimator<Decision> end(const Decision &decis) const
    {
        return const_partimator<Decision>(
            decis,
            algorithm(CSELF, root_, 1));
    }
    template <typename Decision>
    partimator<Decision> end(const Decision &decis)
    {
        return partimator<Decision>(
            decis,
            algorithm(CSELF, root_, 1));
    }

    template <typename Decision>
    class const_reverse_partimator
        : public std::reverse_iterator<const_partimator<Decision> >
    {
        typedef std::reverse_iterator<const_partimator<Decision> > super;

    public:
        explicit const_reverse_partimator(
            const const_partimator<Decision> &obj = const_partimator<Decision>())
            : super(obj)
        {
        }

        bool leaf() const
        {
            return (--super::base()).leaf();
        }

        const_reverse_iterator rbegin() const
        {
            return const_reverse_iterator((--super::base()).end());
        }

        const_reverse_iterator rend() const
        {
            return const_reverse_iterator((--super::base()).begin());
        }

        template <typename Decis2>
        const_reverse_partimator<Decis2> rbegin(const Decis2 &decis) const
        {
            return const_reverse_partimator<Decis2>((--super::base()).end(decis));
        }

        template <typename Decis2>
        const_reverse_partimator<Decis2> rend(const Decis2 &decis) const
        {
            return const_reverse_partimator<Decis2>((--super::base()).begin(decis));
        }
    };

    template <typename Decision>
    class reverse_partimator
        : public std::reverse_iterator<partimator<Decision> >
    {
        typedef std::reverse_iterator<partimator<Decision> > super;

    public:
        explicit reverse_partimator(
            const partimator<Decision> &obj = partimator<Decision>())
            : super(obj)
        {
        }

        bool leaf() const
        {
            return (--super::base()).leaf();
        }

        reverse_iterator rbegin() const
        {
            return reverse_iterator((--super::base()).end());
        }

        reverse_iterator rend() const
        {
            return reverse_iterator((--super::base()).begin());
        }

        template <typename Decis2>
        reverse_partimator<Decis2> rbegin(const Decis2 &decis) const
        {
            return reverse_partimator<Decis2>((--super::base()).end(decis));
        }

        template <typename Decis2>
        reverse_partimator<Decis2> rend(const Decis2 &decis) const
        {
            return reverse_partimator<Decis2>((--super::base()).begin(decis));
        }
    };

    // rbegin() partimator declarations
    template <typename Decision>
    const_reverse_partimator<Decision> rbegin(const Decision &decis) const
    {
        return const_reverse_partimator<Decision>(end(decis));
    }
    template <typename Decision>
    reverse_partimator<Decision> rbegin(const Decision &decis)
    {
        return reverse_partimator<Decision>(end(decis));
    }

    // rend() partimator declarations
    template <typename Decision>
    const_reverse_partimator<Decision> rend(const Decision &decis) const
    {
        return const_reverse_partimator<Decision>(begin(decis));
    }
    template <typename Decision>
    reverse_partimator<Decision> rend(const Decision &decis)
    {
        return reverse_partimator<Decision>(begin(decis));
    }

    // find(const key_type&) declarations
    const_iterator find(const key_type &key) const
    {
        algorithm pal(CSELF, root_, 0);
        // if number of first mismatching bit end at infinity
        if (root_ && ~word_t(0) == pal.mismatch(bit_comp_, key))
            // then return iterator on finding
            return const_iterator(vertex(pal));
        return end();
    }
    iterator find(const key_type &key)
    {
        algorithm pal(CSELF, root_, 0);
        if (root_ && ~word_t(0) == pal.mismatch(key))
            return iterator(vertex(pal));
        return end();
    }

    const_iterator lower_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0)) const
    {
        algorithm pal(this, root_, 0);
        // find a nearest match
        if (root_)
        {
            pal.mismatch(key, prefixLen);
            pal.descend(0);
            return const_iterator(vertex(pal));
        }
        return end();
    }
    iterator lower_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        algorithm pal(this, root_, 0);
        // find a nearest match
        if (root_)
        {
            pal.mismatch(key, prefixLen);
            pal.descend(0);
            return iterator(vertex(pal));
        }
        return end();
    }

    const_iterator upper_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0)) const
    {
        algorithm pal(this, root_, 0);
        if (root_)
        {
            if (pal.mismatch(key, prefixLen) >= prefixLen)
                pal.move(1);
            pal.descend(0);
            return const_iterator(vertex(pal));
        }
        return end();
    }
    iterator upper_bound(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        algorithm pal(this, root_, 0);
        if (root_)
        {
            if (pal.mismatch(key, prefixLen) >= prefixLen)
                pal.move(1);
            pal.descend(0);
            return iterator(vertex(pal));
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
        algorithm pal(CSELF, root_, 0);
        if (root_)
        {
            const word_t len = pal.mismatch(key, prefixLen);
            algorithm lower(pal);
            lower.descend(0);
            if (len < prefixLen)
                return const_iter_range(
                    const_iterator(vertex(lower)),
                    const_iterator(vertex(lower)));
            pal.move(1);
            pal.descend(0);
            return const_iter_range(
                const_iterator(vertex(lower)),
                const_iterator(vertex(pal)));
        }
        return const_iter_range(this->end(), this->end());
    }
    iter_range equal_range(
        const key_type &key,
        word_t prefixLen = ~word_t(0))
    {
        algorithm pal(CSELF, root_, 0);
        if (root_)
        {
            const word_t len = pal.mismatch(key, prefixLen);
            algorithm lower(pal);
            lower.descend(0);
            if (len < prefixLen)
                return iter_range(
                    iterator(vertex(lower)),
                    iterator(vertex(lower)));
            pal.move(1);
            pal.descend(0);
            return iter_range(
                iterator(vertex(lower)),
                iterator(vertex(pal)));
        }
        return iter_range(this->end(), this->end());
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
