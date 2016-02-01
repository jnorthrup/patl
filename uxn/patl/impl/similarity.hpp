#ifndef PATL_IMPL_SIMILARITY_HPP
#define PATL_IMPL_SIMILARITY_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

template <typename T, typename PreorderIter, typename Decision>
class similarity
{
    typedef T this_t;

public:
    typedef PreorderIter const_preorder_iterator;
    typedef typename const_preorder_iterator::const_vertex const_vertex;
    typedef typename const_vertex::bit_compare bit_compare;
    typedef typename const_vertex::key_type key_type;
    typedef typename bit_compare::char_type char_type;

    explicit similarity(
        const_vertex vtx,
        const Decision &decis,
        word_t skipped = 0,
        const char_type &sentinel = char_type())
        : vtx_(vtx)
        , pit_(vtx.preorder_end()), end_(pit_)
        , decis_(decis)
        , skipped_(skipped)
        , sentinel_(sentinel)
    {
    }

    void init()
    {
        pit_ = vtx_.preorder_begin(); end_ = vtx_.preorder_end();
        decis_.init();
        static_cast<this_t*>(this)->next_fit();
    }

    bool the_end() const { return pit_ == end_; }

    const Decision &decis() const { return decis_; }

    const_vertex vertex() const { return *pit_; }

    void next()
    {
        pit_.next_subtree();
        static_cast<this_t*>(this)->next_fit();
    }

    word_t matched() const { return matched_; }

protected:
    const_vertex vtx_;
    const_preorder_iterator pit_, end_;
    Decision decis_;
    word_t skipped_, matched_;
    char_type sentinel_;
};

template <typename PreorderIter, typename Decision>
class whole_similarity // сопоставлять целиком, итерация по листьям
    : public similarity<
        whole_similarity<PreorderIter, Decision>,
        PreorderIter,
        Decision>
{
    friend similarity;

public:
    whole_similarity(
        const_vertex vtx,
        const Decision &decis,
        word_t skipped = 0,
        const char_type &sentinel = char_type())
        : similarity(vtx, decis, skipped, sentinel)
    {
    }

private:
    void next_fit()
    {
        while (pit_ != end_)
        {
            const word_t skip0 = max0(static_cast<sword_t>(pit_->skip()));
            // if current bit is illegal in pattern - skip whole subtree
            if (!decis_.bit_level(skip0, pit_->get_qid()))
            {
                pit_.next_subtree();
                continue;
            }
            const key_type &key = pit_->key();
            const word_t bit_len = pit_->bit_comp().bit_length(key);
            word_t i;
            if (!(bit_len + 1) && pit_->get_qtag())
            {
                for (i = skip0 / bit_compare::bit_size; i < skipped_; ++i)
                    if (key[i] == sentinel_)
                        break;
                if (i < skipped_)
                {
                    pit_.next_subtree();
                    continue;
                }
            }
            else
                i = get_max(skipped_, skip0 / bit_compare::bit_size);
            decis_.trunc(i - skipped_); // сообщить функтору, с какого по счету символа начинать сопоставление
            const word_t limit = pit_->get_qtag()
                ? bit_len           / bit_compare::bit_size - 1 // leaf
                : pit_->next_skip() / bit_compare::bit_size;    // branch
            if (i > limit)
            {
                ++pit_;
                continue;
            }
            for (; i != limit; ++i)
            {
                if (!(bit_len + 1) && pit_->get_qtag() && key[i] == sentinel_)
                {
                    if (decis_.accepted())
                    {
                        matched_ = i - skipped_;
                        return;
                    }
                    else
                        break;
                }
                if (!decis_.next_char(key[i])) // сопоставить символ key[i] в позиции i
                    break;
            }
            if (i != limit)
            {
                pit_.next_subtree();
                continue;
            }
            else if (pit_->get_qtag() && decis_.accepted())
            {
                matched_ = i - skipped_;
                return;
            }
            ++pit_;
        }
    }
};

template <typename PreorderIter, typename Decision>
class greedy_similarity // сопоставлять как можно более длинный префикс, итерация по поддеревьям
    : public similarity<
        greedy_similarity<PreorderIter, Decision>,
        PreorderIter,
        Decision>
{
    friend similarity;

public:
    greedy_similarity(
        const_vertex vtx,
        const Decision &decis,
        word_t skipped = 0,
        const char_type &sentinel = char_type())
        : similarity(vtx, decis, skipped, sentinel)
    {
    }

private:
    void next_fit()
    {
        while (pit_ != end_)
        {
            const word_t skip0 = max0(static_cast<sword_t>(pit_->skip()));
            // if current bit is illegal in pattern - skip whole subtree
            if (!decis_.bit_level(skip0, pit_->get_qid()))
            {
                pit_.next_subtree();
                continue;
            }
            const key_type &key = pit_->key();
            const word_t bit_len = pit_->bit_comp().bit_length(key);
            word_t i;
            if (!(bit_len + 1) && pit_->get_qtag())
            {
                for (i = skip0 / bit_compare::bit_size; i < skipped_; ++i)
                    if (key[i] == sentinel_)
                        break;
                if (i < skipped_)
                {
                    pit_.next_subtree();
                    continue;
                }
            }
            else
                i = get_max(skipped_, skip0 / bit_compare::bit_size);
            decis_.trunc(i - skipped_); // сообщить функтору, с какого по счету символа начинать сопоставление
            const word_t limit = pit_->get_qtag()
                ? bit_len           / bit_compare::bit_size - 1 // leaf
                : pit_->next_skip() / bit_compare::bit_size;    // branch
            if (i > limit)
            {
                ++pit_;
                continue;
            }
            for (; i != limit; ++i)
            {
                /*if (!(bit_len + 1) && pit_->get_qtag() && key[i] == sentinel_)
                {
                    if (decis_.accepted())
                    {
                        matched_ = i - skipped_;
                        return;
                    }
                    else
                        break;
                }*/
                if (!decis_.next_char(key[i])) // сопоставить символ key[i] в позиции i
                    break;
                if (decis_.accepted()) // завершилось ли сопоставление
                {
                    matched_ = i + 1 - skipped_;
                    return;
                }
            }
            if (i != limit)
            {
                pit_.next_subtree();
                continue;
            }
            else if (pit_->get_qtag() && decis_())
            {
                matched_ = i - skipped_;
                return;
            }
            ++pit_;
        }
    }
};

template <typename PreorderIter, typename Decision>
class narrow_similarity // сопоставлять до первого прихода в позицию приема, итерация по поддеревьям
    : public similarity<
        narrow_similarity<PreorderIter, Decision>,
        PreorderIter,
        Decision>
{
    friend similarity;

public:
    narrow_similarity(
        const_vertex vtx,
        const Decision &decis,
        word_t skipped = 0,
        const char_type &sentinel = char_type())
        : similarity(vtx, decis, skipped, sentinel)
    {
    }

private:
    void next_fit()
    {
        while (pit_ != end_)
        {
            const word_t skip0 = max0(static_cast<sword_t>(pit_->skip()));
            // if current bit is illegal in pattern - skip whole subtree
            if (!decis_.bit_level(skip0, pit_->get_qid()))
            {
                pit_.next_subtree();
                continue;
            }
            const key_type &key = pit_->key();
            const word_t bit_len = pit_->bit_comp().bit_length(key);
            word_t i;
            if (!(bit_len + 1) && pit_->get_qtag())
            {
                for (i = skip0 / bit_compare::bit_size; i < skipped_; ++i)
                    if (key[i] == sentinel_)
                        break;
                if (i < skipped_)
                {
                    pit_.next_subtree();
                    continue;
                }
            }
            else
                i = get_max(skipped_, skip0 / bit_compare::bit_size);
            decis_.trunc(i - skipped_); // сообщить функтору, с какого по счету символа начинать сопоставление
            const word_t limit = pit_->get_qtag()
                ? bit_len           / bit_compare::bit_size - 1 // leaf
                : pit_->next_skip() / bit_compare::bit_size;    // branch
            if (i > limit)
            {
                ++pit_;
                continue;
            }
            if (decis_.accepted()) // завершилось ли сопоставление
            {
                matched_ = i + 1 - skipped_;
                return;
            }
            for (; i != limit; ++i)
            {
                if (!decis_.next_char(key[i])) // сопоставить символ key[i] в позиции i
                    break;
                if (decis_.accepted()) // завершилось ли сопоставление
                {
                    matched_ = i + 1 - skipped_;
                    return;
                }
            }
            if (i != limit)
            {
                pit_.next_subtree();
                continue;
            }
            ++pit_;
        }
    }
};

template <typename PreorderIter, typename Decision>
class prefix_similarity // сопоставлять минимум N символов префикса, итерация по поддеревьям
    : public similarity<
        prefix_similarity<PreorderIter, Decision>,
        PreorderIter,
        Decision>
{
    friend similarity;

public:
    prefix_similarity(
        const_vertex vtx,
        const Decision &decis,
        word_t prefix,
        word_t skipped = 0,
        const char_type &sentinel = char_type())
        : similarity(vtx, decis, skipped, sentinel)
        , prefix_(prefix)
    {
    }

private:
    void next_fit()
    {
        while (pit_ != end_)
        {
            const word_t skip0 = max0(static_cast<sword_t>(pit_->skip()));
            // if current bit is illegal in pattern - skip whole subtree
            if (!decis_.bit_level(skip0, pit_->get_qid()))
            {
                pit_.next_subtree();
                continue;
            }
            const key_type &key = pit_->key();
            const word_t bit_len = pit_->bit_comp().bit_length(key);
            word_t i;
            if (!(bit_len + 1) && pit_->get_qtag())
            {
                for (i = skip0 / bit_compare::bit_size; i < skipped_; ++i)
                    if (key[i] == sentinel_)
                        break;
                if (i < skipped_)
                {
                    pit_.next_subtree();
                    continue;
                }
            }
            else
                i = get_max(skipped_, skip0 / bit_compare::bit_size);
            decis_.trunc(i - skipped_); // сообщить функтору, с какого по счету символа начинать сопоставление
            const word_t limit = pit_->get_qtag()
                ? bit_len           / bit_compare::bit_size - 1 // leaf
                : pit_->next_skip() / bit_compare::bit_size;    // branch
            if (i > limit)
            {
                ++pit_;
                continue;
            }
            for (; i != limit; ++i)
            {
                if (!decis_.next_char(key[i])) // сопоставить символ key[i] в позиции i
                    break;
            }
            if (i != limit)
            {
                pit_.next_subtree();
                continue;
            }
            else if (i == skipped_ + prefix_ && decis_())
            {
                matched_ = i - skipped_;
                return;
            }
            ++pit_;
        }
    }

    word_t prefix_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
