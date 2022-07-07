#include <iostream>
#include <type_traits>
#include "pool_alloc.h"
template<typename AllocatorClass>
struct is_class {
private:

    template <typename C>
    static int f(int C::*p) noexcept { return 0; }
    
    template <typename...>
    static char f(...) noexcept { return 0; }

public:
    static const bool value = (sizeof(f<AllocatorClass>(0)) == sizeof(int));
};


template<typename T>
static const bool is_class_v = is_class<T>::value; 

int main() {

}
