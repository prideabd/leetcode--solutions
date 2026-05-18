#pragma once
#include <iostream>
#include <algorithm>
#include <initializer_list>
#include <new>
#include <utility>
#include <type_traits>

namespace dc {
    template <typename T>
    class MyVector {
    private:
        T* data;
        size_t _size;       // 当前元素个数
        size_t _capacity;   // 最大容量

        // 销毁[first, last)范围内所有对象
        void destroy_elements(T* first, T* last) noexcept {
            for (; first != last; ++first) {
                first->~T();
            }
        }

    public:
        // --- 构造和析构 ---

        // 无参构造函数
        MyVector() : data(nullptr), _size(0), _capacity(0) {
            // std::cout << "[Constructor] 默认构造: 内存地址 " << data << std::endl;
        }

        // 列表构造函数
        MyVector(std::initializer_list<T> list) {
            _size = list.size();
            _capacity = list.size();
            if (_capacity > 0) {
                data = static_cast<T*>(::operator new(_capacity * sizeof(T)));
            } else {
                data = nullptr;
            }
            // data = new T[_capacity];
            // std::cout << "[Constructor] 列表构造: 申请空间 " << _capacity 
                    //   << " at " << data << std::endl;

            size_t i = 0;
            for (const T& item : list) {
                new (data + i++) T(item);
                // data[i++] = item;
            }
        }

        // 移动构造函数
        MyVector(MyVector&& other) noexcept {
            data = other.data;
            _size = other._size;
            _capacity = other._capacity;
            other.data = nullptr;
            other._size = 0;
            other._capacity = 0;
        }

        // 拷贝构造函数:申请内存并就地拷贝
        MyVector(const MyVector& other) {
            _size = other._size;
            _capacity = other._capacity;
            if (_capacity > 0) {
                data = static_cast<T*>(::operator new(_capacity * sizeof(T)));
                // data = new T[_capacity];
                // std::cout << "[Constructor] 拷贝构造 (深拷贝): 申请新空间 " << data << std::endl;
                for (size_t i = 0; i < _size; ++i) {
                    new (data + i) T(other.data[i]);
                    // data[i] = other.data[i];
                }
            } else {
                data = nullptr;
            }
        }

        // 析构函数
        ~MyVector() {
            // std::cout << "[Destructor] 析构释放: 销毁地址 " << data << std::endl;
            clear(); // 析构所有对象
            ::operator delete(data); // 释放内存
            // delete[] data;
        }

        
        // --- 赋值运算符 ---
        // copy_and_swap
        void swap(MyVector& other) noexcept {
            // using std::swap; // 允许编译器在找不到自定义 swap 时使用标准库版本
            // swap(data, other.data);
            // swap(_size, other._size);
            // swap(_capacity, other._capacity);
            T* tempPtr = data;
            data = other.data;
            other.data = tempPtr;
            
            size_t tempSize = _size;
            _size = other._size;
            other._size = tempSize;

            size_t tempCap = _capacity;
            _capacity = other._capacity;
            other._capacity = tempCap;
        }

        MyVector& operator=(MyVector other) {
            this->swap(other);
            return *this;
        }

        MyVector& operator=(MyVector&& other) noexcept {
            if (this != &other) {
                clear();
                ::operator delete(data);
                data = other.data;
                _size = other._size;
                _capacity = other._capacity;
                other.data = nullptr;
                other._size = 0;
                other._capacity = 0;
            }
            return *this;
        }

        // --- 核心内存控制 ---
        // 扩容函数
        void reserve(size_t new_capacity) {
            // std::cout << "DEBUG: 正在扩容至 " << new_capacity << std::endl;
            if (new_capacity <= _capacity) {
                return;
            }
            // 1. 分配原始内存
            T* new_data = static_cast<T*>(::operator new(new_capacity * sizeof(T)));

            size_t i = 0;
            try {
                // 2. 搬运数据
                for (; i < _size; ++i) {
                    new (new_data + i) T(std::move_if_noexcept(data[i]));
                }
            }
            catch (...) {
                // 3. 异常处理，销毁新对象并释放新内存
                destroy_elements(new_data, new_data + i);
                ::operator delete(new_data);
                throw;
            }
            // 4. 销毁旧对象并释放旧内存
            destroy_elements(data, data + _size);
            ::operator delete(data);
            data = new_data;
            _capacity = new_capacity;
        }

        // 调整有效元素个数
        void resize(size_t new_size, const T& value = T()) {
            if (new_size < _size) {
                // 尺寸变小，销毁后续对象，无需释放内存
                destroy_elements(data + new_size, data + _size);
                _size = new_size;
            } else if (new_size > _size) {
                // 尺寸变大
                // 先判断是否超出最大容量，分配空间
                if (new_size > _capacity) {
                    reserve(new_size);
                }
                for (size_t i = _size; i < new_size; ++i) {
                    new (data + i) T(value);
                }
                _size = new_size;
            }
        }

        // 手动释放内存
        void shrink_to_fit() {
            if (_size >= _capacity) {
                return;
            }
            if (_size == 0) {
                ::operator delete(data);
                data = nullptr;
                _capacity = 0;
                return;
            }
            // 1.申请新内存，大小为_size * sizeof(T)
            T* new_data = static_cast<T*>(::operator new(_size * sizeof(T)));
            // 2.移动旧数据的同时销毁旧对象
            for (size_t i = 0; i < _size; ++i) {
                new (new_data + i) T(std::move_if_noexcept(data[i]));
                data[i].~T();
            }
            // 3.释放旧内存
            ::operator delete(data);
            data = new_data;
            _capacity = _size;
        }

        // --- 修改器 ---

        // emplace_back 的核心思想是：我不接收对象，我接收用来构造对象的参数！
        template <typename... Args>
        void emplace_back(Args&&... args) {
            // 1. 检查容量
            if (_size >= _capacity) {
                reserve(_capacity == 0 ? 1 : _capacity * 2);
            }
            // 2. 就地构造，不去调用任何构造函数
            new (data + _size) T(std::forward<Args>(args)...);
            _size++;
        }

        template <typename... Args>
        iterator emplace(const_iterator pos, Args&&... args) {
            // 1. 安全检查，确保pos属于当前范围
            if (pos < begin() || pos > end()) {
                throw std::out_of_range("MyVector::emplace position out of range");
            }
            // 计算插入点下标
            size_t index = static_cast<size_t>(pos - begin());

            // 2. 性能与安全性判断
            constexpr bool safe_inplace = std::is_nothrow_move_assignable_v<T> && std::is_nothrow_constructible_v<T, Args...>;

            // A：需要扩容或者操作可能抛异常，选择在新空间重构
            if (_size + 1 > _capacity || !safe_inplace) {
                size_t new_capcity = (_capacity == 0 ? 1 : _capacity * 2);
                // 为了避免扩容后仍然不够，确保新容量至少能容纳_size + 1个元素
                if (new_capcity < _size + 1) {
                    new_capcity = _size + 1;
                }
                T* new_data = static_cast<T*>(::operator new(new_capcity * sizeof(T)));
                size_t constructed = 0; // 记录已构造对象数量，用于异常回滚
                try {
                    // 搬运插入位置之前的元素
                    for (size_t i = 0; i < index; ++i) {
                        new (new_data + i) T(std::move_if_noexcept(data[i]));
                        ++constructed;
                    }
                    // 构造新元素
                    new (new_data + index) T(std::forward<Args>(args)...);
                    ++constructed;
                    // 搬运之后的元素
                    for (size_t j = index; j < _size; ++j) {
                        new (new_data + j + 1) T(std::move_if_noexcept(data[j]));
                        ++constructed;
                    }
                } catch (...) {
                    // 强异常安全保证：如果中途失败，销毁新构造对象并释放内存
                    destroy_elements(new_data, new_data + constructed);
                    ::operator delete(new_data);
                    throw;
                }
                // 未出现异常，清理旧对象以及内存，更新指针和容量
                destroy_elements(data, data + _size);
                ::operator delete(data);
                data = new_data;
                _capacity = new_capcity;
                ++_size;
                return data + index;
            }

            // B: 容量足够
            T* insert_pos = data + index;
            if (index == _size) {
                // 末尾插入，直接构造
                new (data + _size) T(std::forward<Args>(args)...);
                ++_size;
                return insert_pos;
            }

            // 解决自引用风险：先构造临时对象，防止 args 引用的是容器内部即将被平移的元素
            T tmp(std::forward<Args>(args)...);

            // ① 在末尾空白处构造最后一个元素的副本
            new (data + _size) T(std::move_if_noexcept(data[_size - 1]));
            // ② 逆向平移
            for (size_t i = _size - 1; i > index; --i) {
                data[i] = std::move(data[i - 1]);
            }
            // ③ 将临时对象移动赋值到目标位置（此时 data[index] 是已移走状态，赋值是安全的）
            data[index] = std::move(tmp);

            ++_size;
            return insert_pos;
        }

        // insert函数
        iterator insert(const_iterator pos, const T& value) {
            return emplace(pos, value);
        }
        iterator insert(const_iterator pos, T&& value) {
            return emplace(pos, std::move(value));
        }


        // 添加元素到末尾
        void push_back(const T& value) {
            // if (_size > _capacity) { 
            //     std::cerr << "警告: 发生严重错误, size 已经超过了 capacity!" << std::endl; 
            // }
            if (_size >= _capacity) {
                reserve(_capacity == 0 ? 1 : _capacity * 2);
            }
            new (data + _size) T(value);
            _size++;
            // data[_size++] = value;
        }

        // 增加右值引用版本(xxx.push_back(std::move(str)))
        void push_back(T&& value) {
            if (_size >= _capacity) {
                reserve(_capacity == 0 ? 1 : _capacity * 2);
            }
            new (data + _size) T(std::move(value));
            _size++;
        }

         // 删除末尾元素
        void pop_back() {
            if (_size > 0) {
                // 显示调用析构进行销毁
                data[_size - 1].~T();
                _size--;
            }
        }

        // 清空元素
        void clear() {
            destroy_elements(data, data + _size);
            _size = 0;
        }

        // 通过指针范围删除元素(迭代器)
        // 返回被删元素的下一个有效元素
        iterator erase(iterator first, iterator last) {
            if (first < begin() || last > end()) return end();
            if (first >= last) return first;

            size_t num_to_remove = last - first;
            T* end_ptr = data + _size;

            if constexpr (std::is_move_assignable_v<T>) {
                T* write = first;
                T* read = last;
                for (; read != end_ptr; ++write, ++read) {
                    *write = std::move(*read);
                }
                destroy_elements(data + _size - num_to_remove, data + _size);
            } else {
                // 对于不可赋值类型，先销毁被移除的槽，再就地移动构造后销毁源对象
                for (T* p = first; p != last; ++p) {
                    p->~T();
                }
                for (T* p = last; p != end_ptr; ++p) {
                    new (p - num_to_remove) T(std::move(*p));
                    p->~T();
                }
            }

            _size -= num_to_remove;
            return first;
        }
        // 通过下标删除元素
        void erase(size_t index) {
            if (index >= _size) {
                return;
            }
            // 复用迭代器版
            erase(data + index, data + index + 1);
        }

        // --- 访问器 ---

        // 为什么要同时拥有[]和at()？
        // []：用于你确定索引合法的地方（比如 for(int i=0; i<v.size(); ++i)）。
        // at()：用于索引来自外部输入（比如用户输入、网络包数据）或者你不确定逻辑是否严密的地方。
        // 重载[]运算符，非const版本：允许修改
        T& operator[](size_t index) {
            return data[index];
        }
        // 重载[]运算符，const版本：只读访问，用于const MyVector对象
        const T& operator[](size_t index) const {
            return data[index];
        }

        // 拥有和[]相同的功能，但进行边界检查，非const
        T& at(size_t index) {
            if (index >= _size) {
                throw std::out_of_range("MyVector::at() index out of range");
            }
            return data[index];
        }
        // 拥有和[]相同的功能，但进行边界检查，const
        const T& at(size_t index) const {
            if (index >= _size) {
                throw std::out_of_range("MyVector::at() index out of range");
            }
            return data[index];
        }

        // 返回大小
        size_t size() const { return _size; }
        // 返回最大容量
        size_t capacity() const { return _capacity; } 
        // 是否为空
        bool empty() const { return _size == 0; }

        // 简单的正向迭代器，非const版本: 允许修改
        
        using const_iterator = const T*;
        iterator begin() { return data; }
        iterator end() { return data + _size; }
        // const版本：只读
        const_iterator begin() const { return data; }
        const_iterator end() const { return data + _size; }
        // 在类中补充这两个显式只读接口
        const_iterator cbegin() const noexcept { return data; }
        const_iterator cend() const noexcept { return data + _size; }
        
        // 反向迭代器：实现的话，需要定义一个类
        // 类内对operator--和operator++进行重载，来实现反向迭代
        // 目前直接调用std::reverse_iterator
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        // 非const版本
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        // const版本
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        // 显式只读接口
        const_reverse_iterator crbegin() const noexcept {return const_reverse_iterator(end()); }
        const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }
        // 获取第一个元素
        T& front() { return data[0]; }
        const T& front() const { return data[0]; }

        // 获取最后一个元素
        T& back() { return data[_size - 1]; }
        const T& back() const { return data[_size - 1]; }

        // 提供简单sort接口
        // 目前直接使用std::sort
        void sort() { std::sort(this->begin(), this->end()); }
        // 支持自定义排序规则
        template <typename Compare>
        void sort(Compare comp) {
            std::sort(this->begin(), this->end(), comp);
        }
    };

    template <typename T>
    std::ostream& operator<<(std::ostream& os, const MyVector<T>& v) {
        os << "[";
        for (size_t i = 0; i < v.size(); ++i) {
            os << v[i];
            if (i < v.size() - 1) {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }
}