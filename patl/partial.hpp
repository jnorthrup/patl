#ifndef PATL_PARTIAL_HPP
#define PATL_PARTIAL_HPP

#include "impl/partial_base.hpp"

namespace uxn
{
namespace patl
{

/// ��������� �������: ����� ���������� � ���� ���������� ���� ����� � �����
/// (��� �������-����������� ����� ����� ������ false)
template <typename Container, bool SameLength = false>
class partial_match
    : public impl::partial_base<Container, SameLength>
{
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    partial_match(
        const Container &cont,              // ��������� ����������
        const key_type &mask,               // ����� � ��������� ���� '?'
        word_t mask_len = ~word_t(0),       // ����� ����� � �������� (��� ����������� �����)
        const char_type &joker = '?',       // ������ �������
        const char_type &terminator = '\0') // ������ ��������� ������
        : super(cont, mask, mask_len, terminator)
        , joker_(joker)
    {
    }

    void init() const
    {
    }

    bool operator()(word_t i) const
    {
        return i <= super::mask_len_;
    }

    // �������� �� ����������� ����������� ������� ch � ������� i �������
    bool operator()(word_t i, const char_type &ch) const
    {
        return i < super::mask_len_
            ? ch != super::terminator_ && (super::mask_[i] == joker_ || super::mask_[i] == ch)
            : !SameLength || i == super::mask_len_ && ch == super::terminator_;
    }

private:
    char_type joker_;
};

/// Hamming distance decision functor
template <typename Container, bool SameLength = false>
class hamming_distance
    : public impl::partial_base<Container, SameLength>
{
    typedef impl::partial_base<Container, SameLength> super;
    typedef typename Container::key_type key_type;
    typedef typename Container::bit_compare bit_compare;
    typedef typename bit_compare::char_type char_type;

public:
    hamming_distance(
        const Container &cont,
        word_t dist,
        const key_type &mask,
        word_t mask_len = ~word_t(0),
        const char_type &terminator = '\0') // ������ ��������� ������
        : super(cont, mask, mask_len, terminator)
        , dist_(dist)
    {
    }

    word_t distance() const
    {
        return diff_.size();
    }

    void init()
    {
        diff_.clear();
    }

    bool operator()(word_t i) const
    {
        return i <= super::mask_len_;
    }

    // �������� �� ����������� ����������� ������� ch � ������� i �������
    bool operator()(word_t i, const char_type &ch)
    {
        while (!diff_.empty() && i <= diff_.back())
            diff_.pop_back();
        if (i < super::mask_len_ && ch != super::terminator_)
        {
            if (super::mask_[i] == ch)
                return true;
            if (diff_.size() == dist_)
                return false;
            diff_.push_back(i);
            return true;
        }
        return
            !SameLength && i >= super::mask_len_ ||
            i == super::mask_len_ && ch == super::terminator_;
    }

private:
    word_t dist_;
    std::vector<word_t> diff_;
};

} // namespace patl
} // namespace uxn

#endif
