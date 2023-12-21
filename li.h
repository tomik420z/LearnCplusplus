#ifndef LI_H
#define LI_H

#include <iostream>

/// 
/// особенность данного списка заключается в том, что он не имеет поля кол-ва элементов, сделано это для того, 
/// чтобы операция splise выполнялась за константное время, но теперь метод size() выполняется за линейное время  

/// @brief класс реализации шаблонного списка
/// @tparam _Ty - тип данных 
/// @tparam _Ty_Alloc - тип распределителя памяти 
template <typename _Ty, typename _Ty_Alloc = std::allocator<_Ty>>
class li
{
public:
    using value_type = _Ty;
    using size_type = size_t;

protected:
    /// @brief узел списка 
    struct Node
    {
        _Ty val; // значение 
        Node *prev; // указатель на предыдущий элемент 
        Node *next; // указатель на следующий элеменит 

    };

    //! так, как память будет выделяться не только под сам элемент списка, а под весь узел,
    //! следует создать аллокатор с типом Node
    using reb_alloc = typename std::allocator_traits<_Ty_Alloc>::template rebind_alloc<Node>;
    reb_alloc alloc; // аллокатор
    // простой юзинг для удобства 
    using alloc_traits = std::allocator_traits<reb_alloc>;

    Node *head; // указатель на голову
    Node *tail; // указатель на хвост (сырая память, для итератора)

    
    template <bool _is_const>
    class _common_iterator
    {
    public:

        using value_type = std::conditional_t<_is_const, const _Ty, _Ty>;
        using pointer = std::conditional_t<_is_const, const _Ty*, _Ty*>;        
        using reference = value_type &;
        using const_reference = const reference;

    protected:

        using node_type = std::conditional_t<_is_const, const Node*, Node*>;
        
        node_type ptr;

        // конструктор от указателя (узла) 
        _common_iterator(Node* ptr) : ptr(ptr) {}

    public:
        // конструктор копирования 
        _common_iterator(const _common_iterator &iter) : ptr(iter.ptr) {}

        // оператор копирования 
        _common_iterator &operator=(const _common_iterator &iter)
        {
            ptr = iter.ptr;
            return *this;
        }

        // префиксный инкремент 
        _common_iterator &operator++()
        {
            ptr = ptr->next;
            return *this;
        }

        // постфиксный инкремент 
        _common_iterator &operator++(int)
        {
            _common_iterator tmp = *this;
            ptr = ptr->next;
            return tmp;
        }

        // префиксный декремент 
        _common_iterator &operator--()
        {
            ptr = ptr->prev;
            return *this;
        }

        // постфиксный декремент 
        _common_iterator &operator--(int)
        {
            _common_iterator tmp = *this;
            ptr = ptr->prev;
            return tmp;
        }

        // вернуть базовый указатель  
        const Node base() noexcept { return ptr; }

        value_type& operator*() { return ptr->val; }

        bool operator==(const _common_iterator &iter) const noexcept {
            return ptr == iter.ptr;
        }

        bool operator!=(const _common_iterator &iter) const noexcept { 
            return ptr != iter.ptr; 
        }

        pointer operator->() noexcept { 
            return ptr; 
        }

        ~_common_iterator() = default;

        friend li<_Ty>;
    };

    friend  _common_iterator<true>;
    friend  _common_iterator<false>;

public:
    // конструктор по умолчанмию 
    li() 
    : alloc(), 
      head(), 
      tail(alloc_traits::allocate(alloc, 1)) {

        head = tail;
    }

    li(size_t __n, const _Ty &__val = _Ty()) {
        
        // создать фиктивную ноду 
        tail = alloc_traits::allocate(alloc, 1);
        head = tail;
        

        if (__n == 0) {
            return;
        }

        // первый элемент 
        head = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, &head->val, __val);
         
        Node* prev = head;
        // остальные элементы 
        for (size_t i = 1; i < __n; ++i) {
            Node* curr = alloc_traits::allocate(alloc, 1);
            // копирование 
            alloc_traits::construct(alloc, &curr->val, __val);
            curr->prev = prev;
            prev->next = curr; 
            prev = prev->next;
        }
        
        // хвост должен ссылаться на последний элемент для корректного декрементировавания (--end()) 
        prev->next = tail;
        tail->prev = prev;
    }

    li(std::initializer_list<_Ty> init_li)
    {
        // создать фиктивную ноду 
        tail = alloc_traits::allocate(alloc, 1);
        head = tail;
        
        construct_from_range(std::move_iterator(init_li.begin()), 
                            std::move_iterator(init_li.end()));
    }

    // конструктор копирования 
    li(const li<_Ty, _Ty_Alloc> &obj) 
    {
        tail = alloc_traits::allocate(alloc, 1);
        head = tail;

        construct_from_range(obj.begin(), obj.end());
    }

    // конструктор перемещения (реализация плохая, перемещающий конструктор не должен выделаять память)
    li(li<_Ty, _Ty_Alloc> &&obj) : head(obj.head), tail(obj.tail)
    {    
        obj.tail = alloc_traits::allocate(obj.alloc, 1);
        obj.head = obj.tail;
    }

    /// @brief проврека списка на пустооту 
    /// @return  true - если пуст, false - в противном случае 
    bool empty() const noexcept {
        return tail == head;
    }

    /// @brief размер списка 
    /// @return 
    size_t size() const noexcept {  
        int sz = 0;
        for(auto it = begin(); it != end(); ++it, ++sz) ; 
        
        return sz;
    }
    
    // очистка списка 
    void clear() {
        Node* curr = head;

        while(curr != tail) {
            Node* next = curr->next;
            alloc_traits::destroy(alloc, &curr->val);
            alloc_traits::deallocate(alloc, curr, 1);
            curr = next;
        }

        head = tail; 
    }
    // деструктор 
    ~li() {

        clear();

        alloc_traits::deallocate(alloc, tail, 1);
        
    }

    void push_front(const _Ty & _val) {
        Node* curr = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, &curr->val, _val);
    
        Node* tmp = head;
        head = curr;
        curr->next = tmp;
        tmp->prev = head;
    } 

    void push_front(_Ty && _val) {
        Node* curr = create_node(std::move(_val));

        Node* tmp = head;
        head = curr;
        curr->next = tmp;
        tmp->prev = head;
    } 

    template<typename... Args>
    void emplace_front(Args&&... args) {
        Node* curr = create_node(std::forward<Args>(args)...);

        Node* tmp = head;
        head = curr;
        curr->next = tmp;
        tmp->prev = head;
    }
    
/*

    li<_Ty, _Ty_Alloc> &operator=(const li<_Ty, _Ty_Alloc> &obj)
    {
        if (this == &obj)
            return *this;

        clear();

        if (alloc_traits::propagate_on_container_copy_assignment::value &&
            alloc != obj.alloc)
        {
            alloc = obj.alloc;
        }

        for (li<_Ty, allocator_type>::iterator it = obj.begin(); it != obj.end(); ++it)
        {
            push_back(*it);
        }

        return *this;
    }

    li<_Ty, _Ty_Alloc> &operator=(li<_Ty, _Ty_Alloc> &&obj)
    {
        if (this == &obj)
            return *this;

        clear();

        Node * ptr_tail = tail;

        head = obj.head;
        tail = obj.tail;
        sz = obj.sz;
    
        obj.tail = ptr_tail;
        obj.head = obj.tail;
        obj.sz = 0;

        return *this;
    }

    void push_back(const _Ty &__val)
    {
        if (head == tail)
        {
            head = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &head->val, __val);
            head->next = tail;
            head->prev = tail;

            tail->prev = head;
            tail->next = nullptr;
        }
        else
        {
            Node *cur = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &cur->val, __val);
            cur->next = tail;
            cur->prev = tail->prev;
            tail->prev->next = cur;
            tail->prev = cur;
        }
        ++sz;
    }

    void push_back(_Ty &&__val)
    {
        if (head == tail)
        {
            head = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &head->val, std::move(__val));
            head->next = tail;
            head->prev = tail;

            tail->prev = head;
            tail->next = nullptr;
        }
        else
        {
            Node *cur = alloc_traits::allocate(alloc, 1);
            alloc_traits::construct(alloc, &cur->val, std::move(__val));
            cur->next = tail;
            cur->prev = tail->prev;
            tail->prev->next = cur;
            tail->prev = cur;
        }
        ++sz;
    }

    template <typename... Args>
    void emplace_back(Args &&...args)
    {
        Node *new_ptr = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, new_ptr, std::forward<Args>(args)...);
        if (head == tail)
        {
            new_ptr->next = tail;
            new_ptr->prev = nullptr;

            tail->prev = new_ptr;
            tail->next = nullptr;

            head = new_ptr;
        }
        else
        {
            new_ptr->next = tail;
            new_ptr->prev = tail->prev;
            tail->prev->next = new_ptr;
            tail->prev = new_ptr;
        }
        ++sz;
    }

    void pop_back()
    {
        // узел, который надо удалить
        Node *tmp = tail->prev;

        if (tmp == head)
        {
            alloc_traits::destroy(alloc, &tmp->val);
            alloc_traits::deallocate(alloc, tmp, 1);

            head = tail;
        }
        else
        {

            Node *ptr_prev = tmp->prev;

            ptr_prev->next = tail;
            tail->prev = ptr_prev;

            alloc_traits::destroy(alloc, &tmp->val);
            alloc_traits::deallocate(alloc, tmp, 1);
        }
        --sz;
    }
*/

    using iterator = _common_iterator<false>;

    using const_iterator = _common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;

    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    const_iterator begin() const noexcept  {return const_iterator(head); }

    const_iterator end() const noexcept { return const_iterator(tail); }

    iterator begin() noexcept { return iterator(head); }

    iterator end() noexcept { return iterator(tail); }

    const_iterator cbegin() const noexcept { return const_iterator(head); }

    const_iterator cend() const noexcept { return const_iterator(tail); }

    reverse_iterator rbegin() const { return reverse_iterator(tail); }

    reverse_iterator rend() const { return reverse_iterator(head); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(tail); }

    const_reverse_iterator crend() const { return const_reverse_iterator(head); }
private:
    template<typename Iter> 
    void construct_from_range(Iter _first, Iter _last) {
        
        // если диапазаон пуст 
        if (_first == _last) {
            return;
        }

        // первый элемент 
        head = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, &head->val, *_first);
        ++_first;
         
        Node* prev = head;
        // остальные элементы 
        for (; _first != _last; ++_first) {
            Node* curr = alloc_traits::allocate(alloc, 1);
            // копирование 
            alloc_traits::construct(alloc, &curr->val, *_first);

            curr->prev = prev;
            prev->next = curr; 
            prev = prev->next;
        }
        
        // хвост должен ссылаться на последний элемент для корректного декрементировавания (--end()) 
        prev->next = tail;
        tail->prev = prev;
    }

    template<typename...Args> 
    Node* create_node(Args&&...args) {
        Node* new_node = alloc_traits::allocate(alloc, 1);
        alloc_traits::construct(alloc, &new_node->val, std::forward<Args>(args)...);
        return new_node;
    }
};

#endif