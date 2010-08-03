/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
 |
 | External string mergesort using trie and C++ generators
-*/
#include <list>
#include <uxn/patl/impl/generator.hpp>
#include <uxn/patl/aux_/file_utils.hpp>
#include <uxn/patl/aux_/file_mapping_io.hpp>
#include <uxn/patl/aux_/buffered_io.hpp>
#include <uxn/patl/aux_/perf_timer.hpp>
#include <uxn/patl/trie_set.hpp>
#include <uxn/patl/trie_map.hpp>

#pragma warning(disable: 4503) // warning C4503: decorated name length exceeded, name was truncated
#pragma warning(disable: 4127) // warning C4127: conditional expression is constant

namespace patl = uxn::patl;

//#define SORT_LIN_DEBUG

template <typename T>
class piece
{
public:
    piece()
        : begin_(0)
        , end_(0)
    { }

    piece(const T *begin, const T *end)
        : begin_(begin)
        , end_(end)
    { }

    bool empty() const
    { return !begin_ && !end_; }

    word_t length() const
    { return end_ - begin_; }

    const T &operator[](sword_t off) const
    { return begin_[off]; }
    T &operator[](sword_t off)
    { return begin_[off]; }

    const T *begin() const
    { return begin_; }

    const T *end() const
    { return end_; }

private:
    const T *begin_, *end_;
};

namespace uxn
{
namespace patl
{

/// bit-comparator for piece<T>
template <typename T>
class bit_comparator<piece<T> >
    : public bit_comparator<T>
{
    typedef bit_comparator<T> super;
    typedef piece<T> value_type;

public:
    typedef T char_type;

    static const word_t bit_size = 8 * sizeof(T);

    word_t bit_length(const value_type &s) const
    {
        return bit_size * s.length();
    }

    word_t get_bit(const value_type &s, word_t id) const
    {
        return super::get_bit(s[id / bit_size], id % bit_size);
    }

    word_t bit_mismatch(const value_type &a, const value_type &b) const
    {
        if (&a == &b)
            return ~word_t(0);
        return b.length() < a.length()
            ? mismatch_intern(b, a)
            : mismatch_intern(a, b);
    }

private:
    word_t mismatch_intern(const value_type &a, const value_type &b) const
    {
        word_t i = 0;
        for (; i != a.length(); ++i)
        {
            const word_t m = super::bit_mismatch(a[i], b[i]);
            if (m != ~word_t(0))
                return i * bit_size + m;
        }
        return ~word_t(0);
    }
};

} // namespace patl
} // namespace uxn

typedef piece<unsigned char> line_t;
typedef patl::trie_set<line_t> trie_t;
typedef std::list<std::string> temps_t;
class strings_from_file_generator;
typedef std::list<std::pair<std::string, strings_from_file_generator*> > merge_src_t;
typedef patl::trie_map<line_t, merge_src_t::iterator> trie_map_t;

class strings_from_file_generator
    : public patl::impl::generator_base<strings_from_file_generator, line_t>
{
public:
    strings_from_file_generator(
        const char *infname,
        word_t block_size,
        word_t max_string_size)
        : fmi_(infname)
        , block_size_(block_size)
        , max_string_size_(max_string_size)
        , lin_num_(0)
        , skip_yield_(false)
        , viewed_off_beg_(0)
        , viewed_off_cur_(0)
    {
    }

    PATL_EMIT
    {
        while (fmi_.residue_length())
        {
            fmv_ = fmi_.create_view(block_size_);
            text_ = static_cast<unsigned char*>(fmv_->get_view());
            text_end_ = text_ + fmv_->size();
            lin_beg_ = text_ + viewed_off_beg_;
            lin_cur_ = text_ + viewed_off_cur_;
            viewed_off_cur_ = viewed_off_beg_ = 0;
            for (; lin_cur_ != text_end_; ++lin_cur_)
            {
                // проход по тексту, ищем '\n'
                if (*lin_cur_ == '\n')
                {
                    ++lin_cur_; // указывает на следующий после '\n' символ
                    ++lin_num_;
                    // yield, если длина строки не больше допустимой
                    if (static_cast<word_t>(lin_cur_ - lin_beg_) <= max_string_size_ &&
                        !skip_yield_)
                        PATL_YIELD(line_t(lin_beg_, lin_cur_));
                    else
                        printf("ignore too big line number %d\n", lin_num_);
                    skip_yield_ = false;
                    // находим начало следующей строки, пропускаем блоки '\n'
                    for (lin_beg_ = lin_cur_
                        ; lin_beg_ != text_end_ && *lin_beg_ == '\n'
                        ; ++lin_beg_) ;
                    lin_cur_ = lin_beg_;
                    if (lin_beg_ == text_end_)
                        break;
                }
            }
            if (lin_beg_ != text_end_ && !fmi_.residue_length() &&
                static_cast<word_t>(lin_cur_ - lin_beg_) <= max_string_size_ &&
                !skip_yield_) // yield последней строки
            {
                ++lin_num_;
                PATL_YIELD(line_t(lin_beg_, lin_cur_));
            }
            PATL_YIELD(line_t()); // условный сигнал - окно закрывается
            if (!fmi_.residue_length())
                break;
            // если строка не завершена - откатываем окно назад, до начала строки
            if (lin_beg_ != text_end_)
            {
                if (static_cast<word_t>(lin_cur_ - lin_beg_) > max_string_size_)
                    skip_yield_ = true; // строка слишком большая, пропустить остаток
                else
                {
                    viewed_off_cur_ = fmi_.drop_back(lin_cur_ - lin_beg_);
                    viewed_off_beg_ = viewed_off_cur_ - (lin_cur_ - lin_beg_);
                }
            }
        }
    }
    PATL_STOP_EMIT

    word_t get_lin_num() const
    { return lin_num_; }

    bool empty() const
    { return !generator_line; }

    long long get_file_length() const
    { return fmi_.get_file_length(); }

    long long residue_length() const
    { return fmi_.residue_length(); }

private:
    patl::aux::file_mapping_input fmi_;
    std::auto_ptr<patl::aux::file_mapping_view> fmv_;
    word_t block_size_, max_string_size_, lin_num_;
    bool skip_yield_;
    const unsigned char *text_, *text_end_;
    word_t viewed_off_beg_, viewed_off_cur_;
    const unsigned char *lin_beg_, *lin_cur_;
};

class tmp_sorted_files_generator
    : public patl::impl::generator_base<tmp_sorted_files_generator, std::string>
{
public:
    tmp_sorted_files_generator(
        const char *infname,
        word_t N,
        word_t max_string_size)
        : sff_gen_(infname, N << 18, max_string_size)
        , max_nodes_((N << 18) / 24) // (четверть RAM) / sizeof(node)
    {
    }

    PATL_EMIT
    {
        while (sff_gen_.next())
        {
            value_ = sff_gen_.value();
            // скидываем принудительно, перед окончанием действия окна
            if (value_.empty())
                PATL_YIELD(flush_to_temp());
            else
            {
                if (trie_.size() == max_nodes_)
                    PATL_YIELD(flush_to_temp());
                trie_.insert(value_);
            }
        }
    }
    PATL_STOP_EMIT

    long long get_file_length() const
    { return sff_gen_.get_file_length(); }

    long long residue_length() const
    { return sff_gen_.residue_length(); }

private:
    std::string flush_to_temp()
    {
        const std::string tmpfname(patl::aux::get_temp_file_name());
        patl::aux::buffered_output outbuf(tmpfname.c_str());
        for (trie_t::const_iterator it = trie_.begin()
            ; it != trie_.end()
            ; ++it)
            outbuf.write_bytes(it->begin(), it->length());
        trie_.clear();
        return tmpfname;
    }

    strings_from_file_generator sff_gen_;
    line_t value_;
    word_t max_nodes_;
    trie_t trie_;
};

class strings_from_merged_files_generator
    : public patl::impl::generator_base<strings_from_merged_files_generator, line_t>
{
    strings_from_merged_files_generator &operator=(const strings_from_merged_files_generator&);

public:
    strings_from_merged_files_generator(
        const char *infname,
        word_t N,
        word_t D,
        word_t max_string_size)
        : tsf_gen_(infname, N, max_string_size)
        , input_continued_(true)
        , first_run_(true)
        , D_(D)
        , max_string_size_(max_string_size)
        , amount_per100_(0)
        , levels_completed_(1)
    {
    }

    ~strings_from_merged_files_generator()
    {
        merge_src_t::iterator it = merge_src_.begin();
        while (it != merge_src_.end())
        {
            delete it->second;
            it->second = 0;
            it = merge_src_.erase(it);
        }
    }

    PATL_EMIT
    {
        printf("\r  0%% =>   0%% of sort completed");
        while (!merge_src_.empty() || input_continued_)
        {
            if (!first_run_)
                PATL_YIELD(line_t()); // конец очередного слияния
            printf("\r% 3u%% => 100%% of sort completed", amount_per100_);
            if (input_continued_)
            {
                level_cur_ = std::find(levels_completed_.rbegin(), levels_completed_.rend(), D_).base() - levels_completed_.begin() - 1;
                if (level_cur_ == ~word_t(0))
                {
                    level_cur_ = 0;
                    const word_t init_size = merge_src_.size();
                    while (merge_src_.size() != init_size + D_ && input_continued_)
                    {
                        input_continued_ = tsf_gen_.next();
                        if (input_continued_)
                            merge_src_.push_front(merge_src_t::value_type(
                                tsf_gen_.value(),
                                reinterpret_cast<strings_from_file_generator*>(0)));
                    }
                }
                else
                {
                    if (level_cur_ + 1 == levels_completed_.size())
                        levels_completed_.push_back(0);
                    levels_completed_[level_cur_++] = 0;
                }
            }
            {
                const long long length = tsf_gen_.get_file_length();
                printf("\r% 3u%% =>   0%% of sort completed",
                    amount_per100_ = static_cast<word_t>(100 * (length - tsf_gen_.residue_length()) / length));
            }
            if (merge_src_.size() == 1) // все поместилось в один файл
                break;
            else if (first_run_)
                first_run_ = false;
            // заполняем первые D строк, насколько хватит входных временных файлов
            for (it_ = merge_src_.begin()
                ; it_ != merge_src_.end() && trie_.size() != D_
                ; )
            {
                it_->second = new strings_from_file_generator(
                    it_->first.c_str(),
                    max_string_size_,
                    max_string_size_);
#ifdef SORT_LIN_DEBUG
                printf("\nuse temp file: '%s', %5.3g Mb\n",
                    it_->first.c_str(),
                    static_cast<double>(patl::aux::get_long_file_length(it_->first.c_str())) / 1048576.0);
#endif
                inserted_ = false;
                do
                {
                    strings_continued_ = it_->second->next();
                    if (strings_continued_)
                        inserted_ =
                            !it_->second->value().empty() &&
                            trie_.insert(std::make_pair(it_->second->value(), it_)).second;
                } while (!inserted_ && strings_continued_);
                // нельзя удалять генератор строк из файла до окончания использования этих строк
                if (!strings_continued_ && !inserted_)
                    delete_src();
                else
                    ++it_;
            }
            while (trie_.size() > 1)
            {
                itt_ = trie_.begin();
                PATL_YIELD(itt_->first);
                it_ = itt_->second;
                trie_.erase(itt_);
                if (!(it_->second->get_lin_num() & 0xFFFF))
                {
                    const long long length = it_->second->get_file_length();
                    printf("\r% 3u%% => % 3u%% of sort completed",
                        amount_per100_,
                        static_cast<word_t>(100 * (length - it_->second->residue_length()) / length));
                }
                inserted_ = false;
                strings_continued_ = !it_->second->empty();
                do
                {
                    strings_continued_ = strings_continued_ && it_->second->next();
                    if (strings_continued_)
                        inserted_ =
                            !it_->second->value().empty() &&
                            trie_.insert(std::make_pair(it_->second->value(), it_)).second;
                } while (!inserted_ && strings_continued_);
                if (!strings_continued_ && !inserted_)
                    delete_src();
            }
            // остался только один источник, берем строки сразу из него
            if (trie_.size() == 1)
            {
                itt_ = trie_.begin();
                PATL_YIELD(itt_->first);
                it_ = itt_->second;
                trie_.clear();
                for (strings_continued_ = !it_->second->empty()
                    ; strings_continued_ && it_->second->next()
                    ; )
                    if (!it_->second->value().empty())
                        PATL_YIELD(it_->second->value());
                delete_src();
            }
        }
        printf("\r100%% => 100%% of sort completed\n");
    }
    PATL_STOP_EMIT

    void push_front(const merge_src_t::value_type &value)
    {
        merge_src_.push_front(value);
        if (input_continued_)
            ++levels_completed_[level_cur_];
    }

    bool sole_tmpfname() const
    { return merge_src_.size() == 1; }

    std::string first_tmpfname() const
    { return merge_src_.begin()->first; }

private:
    void delete_src()
    {
        // строки в файле закончились и не используются
        delete it_->second;
        it_->second = 0;
        patl::aux::delete_file(it_->first.c_str());
        it_ = merge_src_.erase(it_);
    }

    tmp_sorted_files_generator tsf_gen_;
    bool input_continued_, first_run_;
    merge_src_t merge_src_;
    word_t D_, max_string_size_;
    trie_map_t trie_;
    bool inserted_, strings_continued_;
    merge_src_t::iterator it_;
    trie_map_t::iterator itt_;
    word_t amount_per100_;
    std::vector<word_t> levels_completed_;
    word_t level_cur_;
};

void sort_huge_text_file(const char *infname, const char *outfname, word_t N)
{
    const word_t D = 4; // block's amount
    const word_t max_string_size = patl::impl::align_up<1 << 16>((N << 19) / D);
    std::string tmpfname(patl::aux::get_temp_file_name());
    std::auto_ptr<patl::aux::buffered_output> outbuf(
        new patl::aux::buffered_output(tmpfname.c_str()));
    strings_from_merged_files_generator sfmf_gen(infname, N, D, max_string_size);
    while (sfmf_gen.next())
    {
        const line_t value(sfmf_gen.value());
        if (value.empty())
        {
            sfmf_gen.push_front(merge_src_t::value_type(
                tmpfname,
                reinterpret_cast<strings_from_file_generator*>(0)));
#ifdef SORT_LIN_DEBUG
            outbuf->flush_buffer();
            printf("\nmerged to file: '%s', %5.3g Mb ***\n",
                tmpfname.c_str(),
                static_cast<double>(outbuf->get_file_length()) / 1048576.0);
#endif
            tmpfname = patl::aux::get_temp_file_name();
            outbuf.reset(new patl::aux::buffered_output(tmpfname.c_str()));
        }
        else
            outbuf->write_bytes(value.begin(), value.length());
    }
    outbuf.reset();
    if (sfmf_gen.sole_tmpfname())
    {
        patl::aux::delete_file(tmpfname.c_str());
        patl::aux::move_file(sfmf_gen.first_tmpfname().c_str(), outfname);
    }
    else
        patl::aux::move_file(tmpfname.c_str(), outfname);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("SORT_LIN <infile> <outfile> [N = 128]\n");
        return 0;
    }
#ifndef PATL_DEBUG
    try
#endif
    {
        word_t N = 128;
        if (argc > 3 && sscanf(argv[3], "%d", &N) != 1)
            printf("Illegal parameter: %s. Use default value.\n", argv[3]);
        patl::aux::performance_timer tim;
        sort_huge_text_file(argv[1], argv[2], N);
        tim.finish();
        printf("time: %.2f sec.\n", tim.get_seconds());
    }
#ifndef PATL_DEBUG
    catch (const std::exception &exc)
    {
        printf("Error: %s\n", exc.what());
    }
#endif
}
