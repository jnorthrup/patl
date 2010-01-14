/*-
 | This source code is part of PATL (Practical Algorithm Template Library)
 | Released under the BSD License (http://www.opensource.org/licenses/bsd-license.php)
 | Copyright (c) 2005, 2007..2009, Roman S. Klyujkov (uxnuxn AT gmail.com)
-*/
#ifndef PATL_AUX_STATIC_ALLOC_HPP
#define PATL_AUX_STATIC_ALLOC_HPP

namespace uxn
{
namespace patl
{
namespace aux
{

class static_alloc_state
{
public:
    static void init(unsigned char *next, word_t size)
    {
        next_ = next;
        limit_ = next + size;
    }

    template <typename T>
    static word_t max_size()
    {
        return next_ < limit_ ? (limit_ - next_) / sizeof(T) : 0;
    }

    template <typename T>
    static T *allocate(word_t num)
    {
        T *ret = reinterpret_cast<T*>(next_);
        next_ += num * sizeof(T);
        return ret;
    }

private:
    static unsigned char *next_, *limit_;
};

unsigned char *static_alloc_state::next_ = 0;
unsigned char *static_alloc_state::limit_ = 0;

template <typename T>
class static_alloc
{
public:
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template <typename U>
    struct rebind
    {
        typedef static_alloc<U> other;
    };

    pointer address(reference value) const
    {
        return &value;
    }
    const_pointer address(const_reference value) const
    {
        return &value;
    }

    static_alloc()
    {
    }
    static_alloc(const static_alloc&)
    {
    }
    template <typename U>
    static_alloc(const static_alloc<U>&)
    {
    }

    ~static_alloc()
    {
    }

    template <typename U>
    bool operator==(const static_alloc<U>&)
    {
        return true;
    }

    template <typename U>
    bool operator!=(const static_alloc<U>&)
    {
        return false;
    }

    size_type max_size() const
    {
        return static_alloc_state::max_size<T>();
    }

    pointer allocate(size_type num, const void* = 0)
    {
        return static_alloc_state::allocate<T>(num);
    }

    void construct(pointer p, const T &value)
    {
        new(static_cast<void*>(p))T(value);
    }

    void destroy(pointer p)
    {
        p->~T();
    }

    void deallocate(pointer, size_type)
    {
    }
};

} // namespace aux
} // namespace patl
} // namespace uxn

#endif
