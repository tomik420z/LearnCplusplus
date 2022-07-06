#ifndef LI_H
#define LI_H

#include <iostream>

template <typename _Ty, typename _Ty_Alloc = std::allocator<_Ty>>
class li
{
public:
    using value_type = _Ty;
    using size_type = size_t;
    using allocator_type = _Ty_Alloc;

protected:
    struct Node
    {
        _Ty val;
        Node *prev;
        Node *next;
    };

    using reb_alloc = typename std::allocator_traits<_Ty_Alloc>::template rebind_alloc<Node>;
    reb_alloc alloc; // аллокатор
    using alloc_traits = std::allocator_traits<reb_alloc>;

    Node *head; // указатель на голову
    Node *tail; // указатель на хвост (сырая память, для итератора)
    size_t sz;

    template <bool _is_const>
    class _common_iterator
    {
    public:
        using value_type = std::conditional_t<_is_const, const Node *, Node *>;
        using pointer = value_type *;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using iterator_category = std::bidirectional_iterator_tag;

    protected:
        value_type ptr;

    public:
        _common_iterator(value_type ptr) : ptr(ptr) {}

        _common_iterator(const _common_iterator &iter) : ptr(iter.ptr) {}

        _common_iterator &operator=(const _common_iterator &iter)
        {
            if (this = &iter)
                return *this;

            ptr = iter.ptr;
            return *this;
        }

        _common_iterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }

        _common_iterator &operator++(int)
        {
            _common_iterator tmp = *this;
            ptr = ptr->next;
            return *this;
        }

        _common_iterator &operator--()
        {
            ptr = ptr->prev;
            return *this;
        }

        _common_iterator &operator--(int)
        {
            _common_iterator tmp = *this;
            ptr = ptr->prev;
            return *this;
        }

        value_type base() { return ptr; }

        std::conditional_t<_is_const, const _Ty &, _Ty &> operator*() { return ptr->val; }

        bool operator==(const _common_iterator &iter) const noexcept
        {
            return ptr == iter.ptr;
        }

        bool operator!=(const _common_iterator &iter) const noexcept { return ptr != iter.ptr; }

        std::conditional_t<_is_const, const _Ty *, _Ty *> operator->() { return ptr; }

        ~_common_iterator() {}
    };

public:
    li(const allocator_type &__alloc = allocator_type()) : sz(0), alloc(__alloc)
    {
        tail = alloc_traits::allocate(alloc, 1);
        head = tail;
    }

    li(size_t __n, const _Ty &__val = _Ty(), const allocator_type &__alloc = allocator_type()) : li(__alloc)
    {
        for (size_t i = 0; i < __n; ++i)
        {
            push_back(__val);
        }
    }

    li(std::initializer_list<_Ty> init_li, const _Ty_Alloc &__alloc = allocator_type()) : li(__alloc)
    {
        for (auto it = init_li.begin(); it != init_li.end(); ++it)
        {
            push_back(*it);
        }
    }

    li(const li<_Ty, _Ty_Alloc> &obj) : sz(0)
    {
        if (alloc_traits::propagate_on_container_copy_assignment::value &&
            alloc != obj.alloc)
        {
            alloc = obj.alloc;
        }

        tail = alloc_traits::allocate(alloc, 1);
        head = tail;

        for (li<_Ty, _Ty_Alloc>::iterator it = obj.begin(); it != obj.end(); ++it)
        {
            push_back(*it);
        }
    }

    li(li<_Ty, _Ty_Alloc> &&obj) : head(obj.head), tail(obj.tail), sz(obj.sz)
    {
        if constexpr (alloc_traits::propagate_on_container_move_assignment::value &&
            alloc != obj.alloc)
        {
            alloc = std::move(obj.alloc);
        }

        obj.tail = alloc_traits::allocate(obj.alloc, 1);
        obj.head = obj.tail;
        obj.sz = 0;
    }

    li<_Ty, _Ty_Alloc> &operator=(const li<_Ty, _Ty_Alloc> &obj)
    {
        if (this == &obj)
            return *this;

        clear();

        if (alloc_traits::propagate_on_container_copy_assignment::value &&
            alloc != obj.alloc)
        {
            alloc = obj.alloc;
        }

        for (li<_Ty, allocator_type>::iterator it = obj.begin(); it != obj.end(); ++it)
        {
            push_back(*it);
        }

        return *this;
    }

    li<_Ty, _Ty_Alloc> &operator=(li<_Ty, _Ty_Alloc> &&obj)
    {
        if (this == &obj)
            return *this;

        clear();

        Node * ptr_tail = tail;

        head = obj.head;
        tail = obj.tail;
        sz = obj.sz;
    
        obj.tail = ptr_tail;
        obj.head = obj.tail;
        obj.sz = 0;

        return *this;
    }

    ~li()
    {
        clear();

        alloc_traits::deallocate(alloc, tail, 1);
    }

    bool empty() { return head == tail; }

    void clear()
    {
        while (!empty())
        {
            pop_back();
        }
    }

    void push_back(const _Ty &__val)
    {
        if (head == tail)
        {
            head = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &head->val, __val);
            head->next = tail;
            head->prev = tail;

            tail->prev = head;
            tail->next = nullptr;
        }
        else
        {
            Node *cur = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &cur->val, __val);
            cur->next = tail;
            cur->prev = tail->prev;
            tail->prev->next = cur;
            tail->prev = cur;
        }
        ++sz;
    }

    void push_back(_Ty &&__val)
    {
        if (head == tail)
        {
            head = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &head->val, std::move(__val));
            head->next = tail;
            head->prev = tail;

            tail->prev = head;
            tail->next = nullptr;
        }
        else
        {
            Node *cur = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &cur->val, std::move(__val));
            cur->next = tail;
            cur->prev = tail->prev;
            tail->prev->next = cur;
            tail->prev = cur;
        }
        ++sz;
    }

    template <typename... Args>
    void emplace_back(Args &&...args)
    {
        Node *new_ptr = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, new_ptr, std::forward<Args>(args)...);
        if (head == tail)
        {
            new_ptr->next = tail;
            new_ptr->prev = nullptr;

            tail->prev = new_ptr;
            tail->next = nullptr;

            head = new_ptr;
        }
        else
        {
            new_ptr->next = tail;
            new_ptr->prev = tail->prev;
            tail->prev->next = new_ptr;
            tail->prev = new_ptr;
        }
        ++sz;
    }

    void pop_back()
    {
        // узел, который надо удалить
        Node *tmp = tail->prev;

        if (tmp == head)
        {
            alloc_traits::destroy(alloc, &tmp->val);
            alloc_traits::deallocate(alloc, tmp, 1);

            head = tail;
        }
        else
        {

            Node *ptr_prev = tmp->prev;

            ptr_prev->next = tail;
            tail->prev = ptr_prev;

            alloc_traits::destroy(alloc, &tmp->val);
            alloc_traits::deallocate(alloc, tmp, 1);
        }
        --sz;
    }

    size_t size() const noexcept { return sz; }

    using iterator = _common_iterator<false>;

    using const_iterator = _common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() const { return iterator(head); }

    iterator end() const { return iterator(tail); }

    const_iterator cbegin() const { return const_iterator(head); }

    const_iterator cend() const { return const_iterator(tail); }

    reverse_iterator rbegin() const { return reverse_iterator(tail); }

    reverse_iterator rend() const { return reverse_iterator(head); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(tail); }

    const_reverse_iterator crend() const { return const_reverse_iterator(head); }
};

#endif