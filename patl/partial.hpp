#ifndef PATL_PARTIAL_HPP
#define PATL_PARTIAL_HPP

#include <string>
#include <vector>

namespace uxn
{
namespace patl
{

/// ��������� �������: ����� ���������� � ���� ���������� ���� ����� � �����
/// (��� �������-����������� ����� ����� ������ false)
template <typename Container, bool SameLength = false>
class partial_match
{
    typedef Container cont_type;
    typedef typename cont_type::key_type key_type;
    typedef typename cont_type::bit_compare bit_compare;
    typedef typename bit_compare::scalar_t scalar_t;

public:
    static const word_t bit_size = bit_compare::bit_size;

    partial_match(
        const cont_type &cont,          // ��������� ����������
        const key_type &mask,           // ����� � ��������� ���� '?'
        const scalar_t &joker = '?',    // ������ �������
        word_t maskLen = ~word_t(0))    // ����� ����� � ����� (��� ����������� �����)
        : bit_comp_(cont.bit_comp())
        , mask_(mask)
        , joker_(joker)
    {
        mask_len_ = impl::get_min(maskLen, bit_comp_.bit_length(mask_) - bit_size);
    }

    /// �������� �� ����������� ����������� ���� �� ��������� id � ������� skip
    bool operator()(word_t skip, word_t id) const
    {
        return skip < mask_len_ + (SameLength ? bit_size : 0)
            ? mask_[skip / bit_size] == joker_ || bit_comp_.get_bit(mask_, skip) == id
            : true;
    }

    /// ����� ������������ ����� � ������
    /// ������ ��������� ����� ������� �������������� ����, �� � ����� ������
    /// ��� �� ����� ������, ������� ��������� ��� �������
    bool operator()(const key_type &str) const
    {
        const word_t strLen = bit_comp_.bit_length(str) - bit_size;
        for (word_t i = 0
            ; i != std::min(mask_len_, strLen) / bit_size + (SameLength ? 1 : 0)
            ; ++i)
        {
            if (mask_[i] != joker_ && mask_[i] != str[i])
                return false;
        }
        return strLen >= mask_len_;
    }

private:
    bit_compare bit_comp_;
    key_type mask_;
    scalar_t joker_;
    word_t mask_len_;
};

/// ����� �� ��, ��� � � ���������� ��������, ������ ��������������
/// ������, ���������� ������ ������������� ����� ����� � �����
/// �� ���� �������� � ������� ������
template <typename cont_type, bool SameLength = false>
class hamming_distance
{
    typedef typename cont_type::key_type key_type;
    typedef typename cont_type::bit_compare bit_compare;

    static const word_t bit_size = bit_compare::bit_size;

public:
    hamming_distance(
        const cont_type &cont,
        const key_type &mask,
        word_t dist,
        word_t maskLen = ~word_t(0))
        : bit_comp_(cont.bit_comp())
        , mask_(mask)
        , dist_(dist)
        , diff_elem_(0)
    {
        mask_len_ = impl::get_min(maskLen, bit_comp_.bit_length(mask_) - bit_size);
    }

    bool operator()(word_t skip, word_t id)
    {
        while (!diff_.empty() && skip <= diff_.back())
        {
            const word_t prevByte = diff_.back() / bit_size;
            diff_.pop_back();
            if (prevByte != (diff_.empty() ? ~word_t(0) : diff_.back() / bit_size))
                --diff_elem_;
        }
        if (skip < mask_len_ + (SameLength ? bit_size : 0) &&
            bit_comp_.get_bit(mask_, skip) != id)
        {
            const word_t prevByte =
                diff_.empty() ? ~word_t(0) : diff_.back() / bit_size;
            diff_.push_back(skip);
            if (prevByte != skip / bit_size)
                ++diff_elem_;
            return diff_elem_ <= dist_;
        }
        else
            return true;
    }

    bool operator()(const key_type &str) const
    {
        const word_t strLen = bit_comp_.bit_length(str) - bit_size;
        for (word_t i = 0, diff = 0
            ; i != impl::get_min(mask_len_, strLen) / bit_size + (SameLength ? 1 : 0)
            ; ++i)
        {
            if (mask_[i] != str[i] && ++diff > dist_)
                return false;
        }
        return strLen >= mask_len_;
    }

private:
    bit_compare bit_comp_;
    key_type mask_;
    word_t dist_;
    word_t mask_len_;
    std::vector<word_t> diff_;
    word_t diff_elem_;
};

} // namespace patl
} // namespace uxn

#endif
