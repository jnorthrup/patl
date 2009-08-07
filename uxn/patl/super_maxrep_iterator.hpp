/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_SUPER_MAXREP_ITERATOR_HPP
#define PATL_SUPER_MAXREP_ITERATOR_HPP

#include <set>
#include <vector>
#include "impl/prefix.hpp"

namespace uxn
{
namespace patl
{

template <typename Container>
class super_maxrep_iterator
    : public std::iterator<
        std::forward_iterator_tag,
        typename Container::prefix>
{
    typedef typename Container::prefix prefix;
    typedef typename Container::key_type key_type;

    struct key_type_less
        : public std::binary_function<key_type, key_type, bool>
    {
        key_type_less()
        {
        }

        key_type_less(const key_type &keys)
            : keys_(keys)
        {
        }

        bool operator()(const key_type &key0, const key_type &key1) const
        {
            return
                key0 == keys_ ||
                key1 != keys_ &&
                key0[-1] < key1[-1];
        }

    private:
        key_type keys_;
    };

    typedef std::set<key_type, key_type_less> unique_type;

    void push_new_pair()
    {
        sfreq_.push_back(std::make_pair(
            1,
            unique_type(key_type_less(pref_.cont()->keys()))));
    }

    void unique_update(unique_type &us, const prefix &pref)
    {
        if (pref.is_root())
            us.clear();
        else if (pref.leaf(0))
        {
            // us is empty!
            us.insert(pref.key(0));
            if (!us.insert(pref.key(1)).second)
                us.clear();
        }
        else if (!us.empty() && !us.insert(pref.key(1)).second)
            us.clear();
    }

    void unique_update(unique_type &us, const unique_type &us1)
    {
        if (us.empty() || us1.empty())
        {
            us.clear();
            return;
        }
        unique_type isect(key_type_less(pref_.cont()->keys()));
        std::set_intersection(
            us.begin(),
            us.end(),
            us1.begin(),
            us1.end(),
            std::inserter(isect, isect.end()),
            us.key_comp());
        if (isect.empty())
            us.insert(us1.begin(), us1.end());
        else
            us.clear();
    }

    void merge()
    {
        std::pair<word_t, unique_type>
            *sfreq1 = &sfreq_.back(),
            *sfreq0 = sfreq1 - 1;
        sfreq0->first += sfreq1->first;
        unique_update(sfreq0->second, sfreq1->second);
        sfreq_.pop_back();
    }

    struct functor
    {
        void init(super_maxrep_iterator *mrit)
        {
            mrit_ = mrit;
        }

        void push()
        {
            mrit_->push_new_pair();
        }

        void incr()
        {
            ++mrit_->sfreq_.back().first;
            mrit_->unique_update(mrit_->sfreq_.back().second, mrit_->pref_);
        }

        void update()
        {
        }

        void merge()
        {
            mrit_->merge();
        }

        bool cond() const
        {
            return mrit_->sfreq_.back().second.empty();
        }

    private:
        super_maxrep_iterator *mrit_;
    };

public:
    super_maxrep_iterator(const Container *cont)
        : pref_(cont->root_prefix())
    {
        push_new_pair();
        func_.init(this);
        pref_.postorder_init(func_);
    }

    const prefix &operator*() const
    {
        return pref_;
    }

    const prefix *operator->() const
    {
        return &**this;
    }

    word_t freq() const
    {
        return sfreq_.back().first;
    }

    super_maxrep_iterator &operator++()
    {
        sfreq_.back().second.clear();
        pref_.postorder_incr(func_);
        return *this;
    }

    super_maxrep_iterator operator++(int)
    {
        super_maxrep_iterator it(*this);
        ++*this;
        return it;
    }

private:
    prefix pref_;
    functor func_;
    std::vector<std::pair<word_t, unique_type> > sfreq_;
};

} // namespace patl
} // namespace uxn

#endif
