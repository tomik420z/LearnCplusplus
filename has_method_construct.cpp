#include <iostream> 
#include <type_traits>

template<typename T>
T&& declval();

template <typename T, typename... Args>
auto hmc_helper(int) -> std::integral_constant<
                            decltype(declval<T>().construct(
                                declval<Args>()...), bool()),
                                true 
                            >;

template<typename T, typename...Args> 
auto hmc_helper(...) -> std::false_type;   

template <typename T, typename... Args>
struct has_method_construct : decltype(hmc_helper<T, Args...>(0)) {};

template<typename T, typename... Args>
const bool has_method_construct_v = has_method_construct<T, Args... >::value;

struct S {
    S()  = delete;
    ~S() = delete;

    void construct(int);
    void construct(int, int);
};

int main() {
    std::cout << (has_method_construct_v<S, int, int, int>) << std::endl;
    return 0;
}