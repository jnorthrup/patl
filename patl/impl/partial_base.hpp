#ifndef PATL_PARTIAL_BASE_HPP
#define PATL_PARTIAL_BASE_HPP

#include <vector>
#include <map>

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename Container, bool SameLength>
class partial_base
{
protected:
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    partial_base(
        const Container &cont,              // ��������� ����������
        const key_type &mask,               // ����� ������ (�������)
        word_t mask_len = ~word_t(0),       // ����� ����� � �������� (��� ����������� �����)
        const char_type &terminator = '\0') // ������ ��������� ������
        : mask_(mask)
        , mask_len_(get_min(
            mask_len,
            cont.bit_comp().bit_length(mask) / bit_compare::bit_size - 1))
        , terminator_(terminator)
    {
    }

    static const bool accept_subtree = !SameLength;

    //void init();

    bool operator()(word_t i) const
    {
        return i <= mask_len_;
    }

    //bool operator()(word_t i, const char_type &ch);

protected:
    key_type mask_;
    word_t mask_len_;
    char_type terminator_;
};

template <typename This, typename Container, bool SameLength>
class levenshtein_generic
    : public partial_base<Container, SameLength>
{
    typedef This this_t;
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

    typedef std::vector<bool> bit_vector;
    typedef std::map<char_type, bit_vector> char_bits_map;
    typedef std::vector<std::pair<word_t, word_t> > states_vector;
    typedef std::vector<states_vector> states_sequence;

public:
    levenshtein_generic(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // ������ ��������� ������
        : super(cont, mask, mask_len, terminator)
        , dist_(dist)
        , zero_bits_(super::mask_len_) // all false init
        , states_seq_(1, states_vector(1, std::make_pair(0, 0)))
    {
        for (word_t i = 0; i != super::mask_len_; ++i)
        {
            const char_type ch = super::mask_[i];
            if (char_map_.find(ch) == char_map_.end())
            {
                const std::pair<typename char_bits_map::iterator, bool> ins_pair =
                    char_map_.insert(std::make_pair(ch, bit_vector(super::mask_len_)));
                bit_vector &hi = ins_pair.first->second;
                for (word_t i = 0; i != super::mask_len_; ++i)
                    hi[i] =  ch == super::mask_[i];
            }
        }
    }

    word_t distance() const
    {
        return cur_dist_;
    }

    void init()
    {
        states_seq_.clear();
        states_seq_.push_back(states_vector(1, std::make_pair(0, 0)));
    }

    bool operator()(word_t i)
    {
        while (states_seq_.size() > i + 1)
            states_seq_.pop_back();
        return !states_seq_.back().empty();
    }

    bool operator()(word_t, const char_type &ch)
    {
        states_seq_.push_back(states_vector());
        const states_vector &current = states_seq_[states_seq_.size() - 2];
        if (ch == super::terminator_)
        {
            cur_dist_ = ~word_t(0);
            for (states_vector::const_iterator it = current.begin()
                ; it != current.end()
                ; ++it)
            {
                if (impl::bits_but_highest(it->first) == super::mask_len_ && it->second < cur_dist_)
                    cur_dist_ = it->second;
            }
            return cur_dist_ != ~word_t(0);
        }
        states_vector &next = states_seq_.back();
        const typename char_bits_map::const_iterator hi_it = char_map_.find(ch);
        const bit_vector &hi = hi_it != char_map_.end()
            ? hi_it->second
            : zero_bits_;
        for (states_vector::const_iterator it = current.begin()
            ; it != current.end()
            ; ++it)
        {
            const word_t
                i = it->first,
                e = it->second;
// C4127: conditional expression is constant
#pragma warning(push)
#pragma warning(disable : 4127)
            if (impl::bits_but_highest(i) == super::mask_len_ && !SameLength)
#pragma warning(pop)
            {
                next.clear();
                cur_dist_ = e;
                for (++it; it != current.end(); ++it)
                {
                    if (impl::bits_but_highest(it->first) == super::mask_len_ &&
                        it->second < cur_dist_)
                        cur_dist_ = it->second;
                }
                return true;
            }
            static_cast<this_t*>(this)->transitions(i, e, hi, next);
        }
        return !next.empty();
    }

protected:
    word_t dist_;

private:
    char_bits_map char_map_;
    bit_vector zero_bits_;
    states_sequence states_seq_;
    word_t cur_dist_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
