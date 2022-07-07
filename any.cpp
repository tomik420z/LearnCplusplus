#include <iostream>
#include <vector>
#include <string>
//type erasure idiom C++
// for exaple any class 

class any {
private:
    struct base_helper;

    template<typename T>
    struct derived_helper;

    struct base_helper{
        virtual ~base_helper() = default; 
        virtual const std::type_info & get_type() const = 0; 
    };

    template <typename T>
    struct derived_helper : base_helper {
        T value;

        derived_helper(const T& value): value(value) {}

        derived_helper(T&& value) : value(std::move(value)) {}

        const std::type_info & get_type() const {
            return typeid(value);
        }
    };

 

    base_helper * object;
public:
    
    template <typename T> 
    friend T any_cast(const any& obj_any);

    any() : object(nullptr) {}

    template <typename T>
    any(const T& object) : object(new derived_helper<T>(object)) {}

    template<typename T>
    any(T&& object) : object(new derived_helper<T>(std::move(object))) {}    

    template <typename T>
    any & operator=(const T & __object) {
        if (object != nullptr) {
            delete object; 
        }
        object = new derived_helper<T>(__object);
        return *this;
    }

    template <typename T>
    any & operator=(T&& __object) {
        if (object != nullptr) {
            delete object; 
        }
        object = new derived_helper<T>(std::move(__object));
        return *this;
        
    }

    ~any() { if (object != nullptr) delete object; }
};

template <typename T> 
T any_cast(const any & obj_any) {
    auto ptr = dynamic_cast<any::derived_helper<T>*>(obj_any.object); 
    if(ptr->get_type() != typeid(T)) {
        throw std::exception(); // bad_any_cast
    } else {
        return ptr->value;
    }
}




int main() {
    std::string str = "abbbd";
    any obj = std::move(str);
    std::cout << any_cast<std::string>(obj) << std::endl;
    obj = 234; 
    std::cout << any_cast<int>(obj) << std::endl;
    obj = std::vector<int>({0,4,6,3,6,7});
    std::cout << any_cast<std::vector<int>>(obj)[0] << std::endl;
    return 0;
}