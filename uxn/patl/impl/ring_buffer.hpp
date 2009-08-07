/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_IMPL_RING_BUFFER_HPP
#define PATL_IMPL_RING_BUFFER_HPP

namespace uxn
{
namespace patl
{
namespace impl
{

template <
    typename Type,
    typename Allocator = std::allocator<Type> >
class ring_buffer
{
    typedef ring_buffer<Type, Allocator> cont_type;

public:
    typedef Type value_type;
    typedef typename Allocator::const_pointer const_pointer;
    typedef typename Allocator::const_reference const_reference;
    typedef typename Allocator::pointer pointer;
    typedef typename Allocator::reference reference;
    typedef Allocator allocator_type;
    typedef word_t size_type;

    explicit ring_buffer(
        size_type initSize,
        const allocator_type &alloc = allocator_type())
        : alloc_(alloc)
        , size_(0)
    {
        begin_ = end_ = head_ = alloc_.allocate(initSize);
        tail_ = head_ + initSize;
    }

    ~ring_buffer()
    {
        clear();
        alloc_.deallocate(head_, tail_ - head_);
    }

    void clear()
    {
        while (!empty())
            pop_front();
    }

    bool empty() const
    {
        return !size_;
    }

    bool endpoint() const
    {
        return begin_ == end_;
    }

    size_type size() const
    {
        return size_;
    }

    size_type capacity() const
    {
        return tail_ - head_;
    }

    void push_back(const_reference value)
    {
        alloc_.construct(end_, value);
        ++size_;
        if (++end_ == tail_)
            end_ = head_;
    }

    void pop_front()
    {
        alloc_.destroy(begin_);
        --size_;
        if (++begin_ == tail_)
            begin_ = head_;
    }

    // and vice versa ;)
    void push_front(const_reference value)
    {
        if (begin_-- == head_)
            begin_ = tail_ - 1;
        alloc_.construct(begin_, value);
        ++size_;
    }

    void pop_back()
    {
        if (end_-- == head_)
            end_ = tail_ - 1;
        alloc_.destroy(end_);
        --size_;
    }

    const_pointer back() const
    {
        return end_ == head_ ? tail_ - 1 : end_ - 1;
    }
    pointer back()
    {
        return end_ == head_ ? tail_ - 1 : end_ - 1;
    }

    const_pointer front() const
    {
        return begin_;
    }
    pointer front()
    {
        return begin_;
    }

    const_pointer operator[](size_type id) const
    {
        return begin_ + id < tail_
            ? begin_ + id
            : head_ + id - (tail_ - begin_);
    }
    pointer operator[](size_type id)
    {
        return begin_ + id < tail_
            ? begin_ + id
            : head_ + id - (tail_ - begin_);
    }

    size_type index_of(const_pointer elem) const
    {
        return elem < begin_
            ? (tail_ - begin_) + (elem - head_)
            : elem - begin_;
    }

    const_pointer following(const_pointer elem) const
    {
        return ++elem == tail_ ? head_ : elem;
    }
    pointer following(pointer elem)
    {
        return ++elem == tail_ ? head_ : elem;
    }

    const_pointer preceding(const_pointer elem) const
    {
        return elem == head_ ? tail_ - 1 : --elem;
    }
    pointer preceding(pointer elem)
    {
        return elem == head_ ? tail_ - 1 : --elem;
    }

private:
    struct reserve_functor
    {
        reserve_functor(const cont_type *cont, pointer new_head)
            : cont_(cont)
            , new_head_(new_head)
        {
        }

        pointer operator()(const_pointer old_ptr) const
        {
            return new_head_ + cont_->index_of(old_ptr);
        }

    private:
        const cont_type *cont_;
        pointer new_head_;
    };

public:
    // резервировать новый размер кольцевого буфера
    void reserve(
        sword_t new_size,
        // указатель на функцию-член для корректировки
        // внутренних указателей элемента на другие элементы
        // того же кольцевого буфера
        void (Type::*mem_fun)(const reserve_functor&) = &Type::realloc)
    {
        // если новый размер кольца больше старого
        if (new_size > tail_ - head_)
        {
            // выделяем место под новый буфер
            pointer
                new_head = alloc_.allocate(new_size),
                new_end = new_head;
            // создаём функтор для корректировки внутренних указателей
            reserve_functor functor(this, new_head);
            // пробегаем по всем элементам кольца
            pointer cur = begin_;
            if (size_) do
            {
                // правим внутренние указатели каждого элемента
                (cur->*mem_fun)(functor);
                // конструируем новый элемент и инициализируем старым
                alloc_.construct(new_end++, *cur);
                // разрушаем старый
                alloc_.destroy(cur);
                // следующий элемент и заворачивание в кольцо
                if (++cur == tail_)
                    cur = head_;
            } while (cur != end_);
            // освобождаем память под старым кольцом
            alloc_.deallocate(head_, tail_ - head_);
            // инициализируем внутренние указатели новыми значениями
            begin_ = head_ = new_head;
            tail_ = head_ + new_size;
            end_ = begin_ + size_; // size_ остался прежним
        }
    }

private:
    allocator_type alloc_;
    pointer head_, tail_, begin_, end_;
    size_type size_;
};

} // namespace impl
} // namespace patl
} // namespace uxn

#endif
