#include <iostream>
#include "pool_alloc.h"
template<typename T>
struct is_class {
private:
    template <typename C>
    static int f(int C::*p) noexcept { return 0; }
    
    template <typename...>
    static char f(...) noexcept { return 0; }

public:
    static const bool value = (sizeof(f<T>(0)) == sizeof(int));
};


template<typename T>
static const bool is_class_v = is_class<T>::value; 


template <typename T>
struct has_foo {
private:
    static int f(...);

    template<typename C> static decltype(std::declval<C>().foo(1)) f(const C&);

public:
    static constexpr bool value = std::is_same_v<void, decltype(f(std::declval<T>()))>;
};

template <typename T, typename... Args>
struct has_method_construct {
private:
        static int f(...);

        template<typename U, typename... Kwargs> 
        static decltype(std::declval<U>().construct(std::declval<Kwargs>()...))  f(const U&, Kwargs&&...);
public:
        static constexpr bool value = std::is_same_v<void, decltype(f(std::declval<T>(), std::declval<Args>()...))>;
};



struct alloc {
    template <typename...Args>
    void construct(Args&&...args) {
        return;
    }
};


int main() {
    std::cout << is_class<int>::value << std::endl;
    std::cout << (is_class_v<std::string>) << std::endl;
    std::cout << has_method_construct<pool_allocator<int>, int>::value << std::endl;
        std::cout << has_method_construct<std::string, char>::value << std::endl;

}
