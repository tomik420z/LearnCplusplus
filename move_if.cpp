#include <iostream>
#include <vector>

class point2D {
protected:
    int* x;
    int* y;
    size_t ind;
    static int index;
public:
    point2D(int x, int y) : x(new int(x)), y(new int(y)), ind(index) { index += 1; }
    
    // move noexept
    point2D(point2D && obj) noexcept {
        x = obj.x;
        y = obj.y;
        obj.x = nullptr;
        obj.y = nullptr;
    }

    point2D(const point2D & obj): x(new int(*obj.x)), y(new int(*obj.y))  { index+=1; }

    int get_index() noexcept { return ind; }

    ~point2D() {
        delete x, y;
    }
};

int point2D::index = 0;

int main() {
    std::vector<point2D> vec_point;
    vec_point.reserve(3); // вектор с зарезервированной памятью на  3 элемента типа point2D
    for(int i = 0; i < 10; ++i) {
        // когда наступает переполнение, память реалоцируется, но копия объектов не происходит, а происходит перемещенеие! 
        vec_point.emplace_back(i, -i);
    } 
    std::cout << vec_point.back().get_index()  << std::endl;
    return 0;
}