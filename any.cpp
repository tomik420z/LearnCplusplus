#include <iostream>
//type erasure idiom C++
// for exaple any class 

namespace proj{
    
class any {
private:
    struct base_helper{
        virtual ~base_helper() = default; 
        virtual const std::type_info & get_type() const = 0; 
    };

    template <typename T>
    struct derived_helper : base_helper {
        T value;

        derived_helper(const T& value): value(value) {}

        const std::type_info & get_type() const {
            return typeid(value);
        }
    };

    base_helper * object;
public:
    
    template <typename T> 
    friend T any_cast(any& obj_any);

    any() : object(nullptr) {}

    template <typename T>
    any(const T& object) : object(new derived_helper<T>(object)) {}
    
    ~any() { delete object; }
};

template <typename T> 
T any_cast(any & obj_any) {
    if(dynamic_cast<any::derived_helper<T>*>(obj_any.object)->get_type() != typeid(T)) {
        throw std::exception();
    } else {
        return dynamic_cast<any::derived_helper<T>*>(obj_any.object)->value;
    }
}

};

using namespace proj;

int main() {
    any obj = std::string("abbbd");
    std::cout << any_cast<int>(obj) << std::endl;
    return 0;
}