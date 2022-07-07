#include <iostream>
#include "pool_alloc.h" // my allocator 
// метафункция, которая проверяет содержит ли аллокатор метод:
//void allocator<T>::construct(T* ptr, Args&&... args);
template <typename T, typename AllocatorClass, typename Pointer, typename... Args>
struct has_method_construct {
private:
        static int f(...);

        template<typename U,typename K, typename... Kwargs> 
        static decltype(std::declval<U>().construct(std::declval<K>() ,std::declval<Kwargs>()...))  
            f(const U&, const K& , Kwargs&&...);
public:
        static constexpr bool value =  std::is_same_v<void, decltype(f(std::declval<AllocatorClass>(), std::declval<Pointer>(), std::declval<Args>()...))> 
            && std::is_constructible_v<T, Args...>;
};

//main call!
// type, container, Args...
template <typename T, template <typename U> class allocator_container, typename... Args>
static const bool has_method_construct_alloc_v =  has_method_construct<T, allocator_container<T>, T*, Args...>::value;

int main() {
    // exapmle
    std::cout << (has_method_construct_alloc_v<std::string, pool_allocator, std::string>) << std::endl; 
    return 0;    
}