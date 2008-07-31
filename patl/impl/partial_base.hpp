#ifndef PATL_PARTIAL_BASE_HPP
#define PATL_PARTIAL_BASE_HPP

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

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
