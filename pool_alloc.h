#ifndef POOL_ALLOC
#define POOL_ALLOC

#include <type_traits>
#include <iostream>
#include <memory>
/* пул аллокатор (или стек аллокатор)
    при создании выделяет память на заданное кол-во элеметов (пул)
    гаранитруется, что этот лимит не будет превышен
    из пула могут "черпать память" сразу несколько аллокаторов
    память не очищается с вызовом deallocate (т.е)
    очистка пула происхоидит при вызове деструктора аллокатора, который последний пользовался пулом.
*/

template <typename _Tp>
class pool_allocator
{
    static_assert(!std::is_same_v<_Tp, void>, "Type of the allocator can not be void");

public:
    using allocator_type = pool_allocator<_Tp>;
    using value_type = _Tp;
    using pointer = _Tp *;
    using const_pointer = const _Tp *;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_copy_assignment = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;

protected:
    static size_t max_size;          // максимальное кол-во элементов
    static _Tp *cur;                 // указатель на первый незанятый элемент
    static size_t __count_connected; // кол-во аллокаторов ссылающихся на пулл
    static _Tp *head;                // указатель на начало выделенной памяти

    void alloc_memory(size_t sz)
    {
        head = reinterpret_cast<_Tp *>(::operator new(sz));
        void * p = head;
        std::align(alignof(_Tp), sizeof(_Tp), 
                    p, max_size); 
            
        max_size -= sizeof(_Tp);
        head = reinterpret_cast<_Tp*>(p);
        cur = head;    
    }

public:
    pool_allocator()
    {
        if (__count_connected == 0)
        {
            std::cout << "alloc_memory\n";
            alloc_memory(sizeof(_Tp) * max_size);
        }
        ++__count_connected;
    }

    pool_allocator(const pool_allocator<_Tp> &alloc) : pool_allocator() {}

    template <typename U>
    pool_allocator(const pool_allocator<U> &__alloc) = delete;

    template<typename U>
    pool_allocator(pool_allocator<U> && alloc) = delete;

    pool_allocator(pool_allocator && alloc) : pool_allocator() {}

    pool_allocator<_Tp> &operator=(const pool_allocator<_Tp> &alloc)
    {
        ++__count_connected;
        return *this;
    }

    template <typename U>
    pool_allocator<_Tp> &operator=(const pool_allocator<U> &alloc) noexcept
    {
        ++__count_connected;
        return *this;
    }

    template <typename U>
    pool_allocator<_Tp> &operator=(pool_allocator<U> &&alloc) = delete;

    pool_allocator &operator=(pool_allocator &&alloc) { return *this; }


    _Tp *allocate(size_t n) { return cur++; }

    void deallocate(_Tp *ptr, size_t n) {}

    template <typename... Args>
    void construct(_Tp *ptr, Args &&...args)
    {
        std::cout << "construct\n";
        new (ptr) _Tp(std::forward<Args>(args)...);
    }

    ~pool_allocator()
    {
        if (__count_connected == 1)
        {
            std::cout << "delete pool\n";
            ::operator delete(reinterpret_cast<void *>(head));
        }
        --__count_connected;
    }
};

template <typename T, typename U>
bool operator==(const pool_allocator<T> &__alloc1, const pool_allocator<U> &__alloc2) { return std::is_same_v<U, T>; }

template <typename T, typename U>
bool operator!=(const pool_allocator<T> &__alloc1, const pool_allocator<U> &__alloc2) { return !(__alloc1 == __alloc2); }

template <typename _Tp>
size_t pool_allocator<_Tp>::__count_connected = 0;

template <typename _Tp>
_Tp *pool_allocator<_Tp>::head = nullptr;

template <typename _Tp>
_Tp *pool_allocator<_Tp>::cur = nullptr;

template <typename _Tp>
size_t pool_allocator<_Tp>::max_size = 10'000'000;

#endif