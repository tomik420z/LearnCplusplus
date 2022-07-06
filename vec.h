#ifndef VEC_H
#define VEC_H

#include <iterator>

template <typename _Ty, typename _Ty_Alloc = std::allocator<_Ty>>
class vec
{
protected:
    _Ty *_ptr;
    size_t sz;
    size_t cap;

    using reb_alloc = typename std::allocator_traits<_Ty_Alloc>::template rebind_alloc<_Ty>;
    using alloc_traits = std::allocator_traits<reb_alloc>;
    reb_alloc alloc;

    template <bool _is_const>
    class _common_iterator
    {
    public:
        using value_type = std::conditional_t<_is_const, const _Ty *, _Ty *>;
        using pointer = value_type *;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using iterator_category = std::random_access_iterator_tag;

    protected:
        std::conditional_t<_is_const, const _Ty *, _Ty *> _pointer;

    public:
        _common_iterator(std::conditional_t<_is_const, const _Ty *, _Ty *> ptr) : _pointer(ptr) {}

        _common_iterator(const _common_iterator &iter) : _pointer(iter._pointer) {}

        std::conditional_t<_is_const, const _Ty &, _Ty &> operator*() { return *_pointer; }

        std::conditional_t<_is_const, const _Ty *, _Ty *> operator->() { return _pointer; }

        std::conditional_t<_is_const, const _Ty *, _Ty *> base() { return _pointer; }

        _common_iterator &operator=(const _common_iterator &iter)
        {
            if (this == &iter)
                return *this;
            _pointer = iter._pointer;
            return *this;
        }

        _common_iterator &operator++()
        {
            ++_pointer;
            return *this;
        }

        _common_iterator &operator++(int)
        {
            _common_iterator tmp = *this;
            ++_pointer;
            return *this;
        }

        _common_iterator &operator--()
        {
            --_pointer;
            return *this;
        }

        _common_iterator &operator--(int)
        {
            _common_iterator tmp = *this;
            --_pointer;
            return *this;
        }

        _common_iterator &operator+(size_t step)
        {
            _pointer += step;
            return *this;
        }

        _common_iterator &operator-(size_t step)
        {
            _pointer -= step;
            return *this;
        }

        bool operator==(const _common_iterator &iter) const noexcept { return _pointer == iter._pointer; }

        bool operator!=(const _common_iterator &iter) const noexcept { return _pointer != iter._pointer; }

        ~_common_iterator() {}
    };

public:
    vec(const _Ty_Alloc &alloc = _Ty_Alloc()) : sz(0), cap(0), _ptr(nullptr), alloc(alloc) {}

    vec(size_t sz, const _Ty &val = _Ty(), const _Ty_Alloc &alloc = _Ty_Alloc()) : sz(sz), cap(sz), alloc(alloc)
    {
        _ptr = alloc_traits::allocate(alloc, sz);
        for (size_t i = 0; i < sz; ++i)
        {
            alloc_traits::construct(alloc, _ptr + i, val);
        }
    }

    vec(const std::initializer_list<_Ty> &init_li, const _Ty_Alloc &alloc = _Ty_Alloc()) : vec()
    {
        reserve(init_li.size());
        for (auto it = init_li.begin(); it != init_li.end(); ++it)
        {
            push_back(*it);
        }
    }

    vec<_Ty, _Ty_Alloc> &operator=(const vec<_Ty, _Ty_Alloc> &obj)
    {
        if (this == &obj)
            return *this;

        for (size_t i = 0; i < sz; ++i)
        {
            alloc_traits::destroy(alloc, _ptr + i);
        }

        alloc_traits::deallocate(alloc, _ptr, cap);

        if (alloc_traits::propagate_on_container_copy_assignment::value &&
            obj.alloc != alloc)
        {
            alloc = obj.alloc;
        }

        _ptr = alloc_traits::allocate(alloc, obj.cap);

        size_t i = 0;
        try
        {
            for (; i < obj.sz; ++i)
            {
                alloc_traits::construct(alloc, _ptr + i, *(obj._ptr + i));
            }
        }
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                alloc_traits::destroy(alloc, _ptr + i);
            }
            throw "bad_copy";
        }

        sz = obj.sz;

        return *this;
    }

    vec(const vec<_Ty> &obj) : vec()
    {
        reserve(obj.cap);
        size_t i = 0;
        try
        {
            for (; i < obj.sz; ++i)
            {
                alloc_traits::construct(alloc, _ptr + i, *(obj._ptr + i));
            }
        }
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                alloc_traits::destroy(alloc, _ptr + i);
            }
            throw "bad_copy_construct";
        }

        sz = obj.sz;
    }

    _Ty &operator[](size_t index)
    {
        return _ptr[index];
    }

    const _Ty &operator[](size_t index) const
    {
        return _ptr[index];
    }

    _Ty &at(size_t index)
    {
        if (index >= sz)
            throw "out of range";

        return _ptr[index];
    }

    const _Ty &at(size_t index) const
    {
        if (index >= sz)
            throw "out of range";

        return _ptr[index];
    }

    void reserve(size_t n)
    {
        if (n == 0)
            ++n;

        if (n <= cap)
            return;

        _Ty *tmp = alloc_traits::allocate(alloc, n);

        size_t i = 0;
        try
        {
            for (; i < sz; ++i)
            {
                alloc_traits::construct(alloc, tmp + i, *(_ptr + i));
            }
        }
        catch (...)
        {
            for (size_t j = 0; j < i; ++j)
            {
                alloc_traits::destroy(alloc, tmp + j);
            }
            alloc_traits::deallocate(alloc, tmp, n);

            throw "bad_copy";
        }

        if (_ptr != nullptr)
        {
            alloc_traits::deallocate(alloc, _ptr, cap);
        }
        _ptr = tmp;
        cap = n;
    }

    void resize(size_t new_size, const _Ty &_val = _Ty())
    {
        if (new_size <= sz)
            return;

        reserve(new_size); // резервируем память по надобности

        size_t i = sz;
        try
        {
            for (; i < new_size; ++i)
            {
                alloc_traits::construct(alloc, _ptr + i, _val);
            }
        }
        catch (...)
        {
            for (size_t j = sz; j < i; ++j)
            {
                alloc_traits::destroy(alloc, _ptr + i);
            }
        }
        sz = new_size;
        cap = new_size;
    }

    void clear()
    {
        if (empty())
        {
            return;
        }

        size_t i = 0;

        for (; i < sz; ++i)
        {
            alloc_traits::destroy(alloc, _ptr + i);
        }

        sz = 0;
    }

    void push_back(const _Ty &_val)
    {
        if (sz >= cap)
        {
            reserve(cap * 2);
        }
        try
        {
            alloc_traits::construct(alloc, _ptr + sz, _val);
        }
        catch (...)
        {
            alloc_traits::destroy(alloc, _ptr + sz);
        }

        ++sz;
    }

    void pop_back()
    {
        alloc_traits::destroy(alloc, _ptr + sz - 1);
        --sz;
    }

    size_t size() const noexcept { return sz; }

    size_t capacity() const noexcept { return cap; }

    _Ty *data() noexcept { return _ptr; }

    const _Ty *data() const noexcept { return _ptr; }

    const _Ty &back() const { return _ptr[sz - 1]; }

    _Ty &back() { return _ptr[sz - 1]; }

    const _Ty &front() const { return _ptr[0]; }

    _Ty &front() { return _ptr[0]; }

    using iterator = _common_iterator<false>;
    using const_iterator = _common_iterator<true>;
    using reverse_iterator = typename std::reverse_iterator<iterator>;
    using const_reverse_iterator = typename std::reverse_iterator<const_iterator>;

    bool empty() const noexcept { return (bool)_ptr; }

    iterator begin() const { return iterator(_ptr); }

    iterator end() const { return iterator((_ptr + sz)); }

    const_iterator cbegin() const { return const_iterator(_ptr); }

    const_iterator cend() const { return const_iterator((_ptr + sz)); }

    reverse_iterator rbegin() const { return reverse_iterator(_ptr + sz); }

    reverse_iterator rend() const { return reverse_iterator(_ptr); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(_ptr + sz); }

    const_reverse_iterator crend() const { return const_reverse_iterator(_ptr); }

    void erase(iterator &__pos)
    {
        auto cur = __pos;
        for (auto next = __pos + 1; next != end(); ++next)
        {
            *(cur) = std::move(*(next));
            cur = next;
        }

        alloc_traits::destroy(alloc, _ptr + sz - 1);
        --sz;
    }

    ~vec()
    {
        clear();
        alloc_traits::deallocate(alloc, _ptr, cap);
    }
};

#endif