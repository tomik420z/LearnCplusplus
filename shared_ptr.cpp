#include <iostream>
#include <memory>
#include "pool_alloc.h"

namespace proj {

    template <typename T>
    class my_alloc{
        using value_type = T;
        using pointer = T*;
        pointer ptr;
    public:
        my_alloc() : ptr(nullptr) {}
        T * allocate(size_t n) {
            return reinterpret_cast<pointer>(::operator new(n));
        }

        void deallocate(pointer ptr, size_t n) {
            ::operator delete(ptr);
        }


        ~my_alloc() {}
    };

    template<typename T> 
    class shared_ptr
    {
    private:
        struct base_control_block {
            size_t count_of_shared;
            size_t count_of_weak;
            
            base_control_block(size_t cs, size_t cw) : count_of_shared(cs), count_of_weak(cw) {}
            ~base_control_block() = default;
        };
        
        template <typename _Alloc>
        struct control_block_make_shared : base_control_block {
            T object;
            using reb_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<
                                                                control_block_make_shared<_Alloc>>;
            using alloc_traits = std::allocator_traits<reb_alloc>;

            reb_alloc alloc;

            control_block_make_shared(size_t cs, size_t cw, const T& obj, reb_alloc && alloc) : base_control_block(cs, cw),
                object(obj), alloc(std::move(alloc)) {}

            void destroy() {
                alloc_traits::destroy(alloc, &object);
            }
/*
            ~control_block_make_shared() {
                destroy();
                alloc_traits::deallocate(alloc, this, 1);            
            }
            */
        };

        base_control_block * ptr;
        
        T* value;   
        template<typename _Alloc>
        shared_ptr(control_block_make_shared<_Alloc>  *);

    public:
        template <typename U, typename... Args>
        friend shared_ptr<U> make_shared(Args&&... args);

        template <typename U, typename Alloc,  typename... Args>
        friend shared_ptr<U> allocate_shared(Alloc & alloc, Args&&... args);

        shared_ptr(const shared_ptr &);
        

        template<typename _Alloc, typename... Args> 
        shared_ptr(const _Alloc& alloc, Args&&...args) {
            using reb_alloc = typename std::allocator_traits<_Alloc>::template rebind_alloc<
                                                                control_block_make_shared<_Alloc>>;
            using alloc_traits = std::allocator_traits<reb_alloc>;
            reb_alloc allocator = alloc;

            auto* tmp_ptr = alloc_traits::allocate(allocator, 1);
            alloc_traits::construct(allocator, tmp_ptr, control_block_make_shared<_Alloc>(
                1, 0, T(std::forward<Args>(args)...), std::move(allocator)
            ));
            ptr = tmp_ptr;
            value = &tmp_ptr->object;
        }

        shared_ptr();

        size_t get_count() {
            return ptr->count_of_shared;
        }

        T& operator*() {
            return *value;
        }

        ~shared_ptr();
    };
    
    template<typename T>
    shared_ptr<T>::shared_ptr(const shared_ptr<T> & obj) : ptr(obj.ptr), value(obj.value) {
        ++ptr->count_of_shared;
    }
    template<typename T>
    template<typename _Alloc>
    shared_ptr<T>::shared_ptr(control_block_make_shared<_Alloc> * ptr) : ptr(ptr), value(&ptr->value) { } 

    template<typename T>
    shared_ptr<T>::shared_ptr() : ptr(nullptr), value(nullptr) {}
    
    template<typename T>
    shared_ptr<T>::~shared_ptr() {
        
    }
       
    template <typename T, typename... Args>
    shared_ptr<T> make_shared(Args&&... args) {
        return shared_ptr<T>(std::allocator<int>(), std::forward<Args>(args)...);
    }    

    template <typename T, typename Alloc,  typename... Args>
    shared_ptr<T> allocate_shared(Alloc & alloc, Args&&... args) {
        return ptr(alloc, std::forward<Args>(args)...);
    
    }
};

using namespace proj;

int main() {

    shared_ptr<int> ptr1 = make_shared<int>(10);
    shared_ptr<int> ptr2 = make_shared<int>(15);

    //shared_ptr<int> ptr3 = allocate_shared<int, decltype(all)>(all, 17);
    std::cout << *ptr1 << std::endl;
    std::cout << *ptr2 << std::endl;
    //std::cout << *ptr3 << std::endl;
    return 0;   
}