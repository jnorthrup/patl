/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_MAXREP_ITERATOR_HPP
#define PATL_MAXREP_ITERATOR_HPP

#include <vector>
#include "config.hpp"
#include "impl/prefix.hpp"

namespace uxn
{
namespace patl
{

template <typename Container>
class maxrep_iterator
    : public std::iterator<
        std::forward_iterator_tag,
        typename Container::prefix>
{
    typedef typename Container::prefix prefix;
    typedef typename Container::key_type key_type;

    bool left_diff() const
    {
        if (pref_.is_root())
            return true;
        const key_type
            key0 = pref_.key(0),
            key1 = pref_.key(1);
        return
            key0 == pref_.cont()->keys() ||
            key1 == pref_.cont()->keys() ||
            key0[-1] != key1[-1];
    }

    void bit31_update()
    {
        if (!impl::get_highest_bit(sfreq_.back()) && left_diff())
            sfreq_.back() |= highest_bit;
    }

    void merge()
    {
        const word_t sfreq1 = sfreq_.back();
        sfreq_.pop_back();
        sfreq_.back() =
            (sfreq_.back() | sfreq1) & highest_bit |
            (impl::bits_but_highest(sfreq_.back()) + impl::bits_but_highest(sfreq1));
    }

    struct functor
    {
        void init(maxrep_iterator *mrit)
        {
            mrit_ = mrit;
        }

        void push()
        {
            mrit_->sfreq_.push_back(1);
        }

        void incr()
        {
            ++mrit_->sfreq_.back();
            update();
        }

        void update()
        {
            mrit_->bit31_update();
        }

        void merge()
        {
            mrit_->merge();
        }

        bool cond() const
        {
            return !impl::get_highest_bit(mrit_->sfreq_.back());
        }

    private:
        maxrep_iterator *mrit_;
    };

public:
    maxrep_iterator(const Container *cont)
        : pref_(cont->root_prefix())
        , sfreq_(1, 1)
    {
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
        return impl::bits_but_highest(sfreq_.back());
    }

    maxrep_iterator &operator++()
    {
        pref_.postorder_incr(func_);
        return *this;
    }

    maxrep_iterator operator++(int)
    {
        maxrep_iterator it(*this);
        ++*this;
        return it;
    }

private:
    prefix pref_;
    functor func_;
    std::vector<word_t> sfreq_;
};

} // namespace patl
} // namespace uxn

#endif
