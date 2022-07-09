

template<typename T, typename...Args>
struct is_constructible {
    private:
    template<typename...>   
    static int f(...);

    template<typename U, typename... Kwargs>
    static decltype(U(std::declval<Kwargs>()...)) f(const U&, Kwargs&&...);

    public:
    static constexpr bool value = std::is_same_v<T, 
        decltype(f(std::declval<T>(), std::declval<Args>()...))>;
}; 


int main() {
    
}


