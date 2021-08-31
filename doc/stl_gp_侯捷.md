## 一、各种测试

## 二、源码分析

### 10、技术基础

泛化：模板实现 
特化：模板参数指定具体类型
偏特化：个数或者范围偏特化，局部模板参数特化或者特定模板参数带修饰（const/*/&等)


``` c++
// 泛化
template <typename T, typename Allocator_T>
class MyVector {
public:
    MyVector() {
        std::cout << "Normal version." << std::endl;
    }
};
// 特化
template <>
class MyVector<int, DefaultAllocator> {
public:
    MyVector() {
        std::cout << "Partial version." << std::endl;
    }
};
// 偏特化
template <typename T>
class MyVector<T, DefaultAllocator> {
public:
    MyVector() {
        std::cout << "Partial version." << std::endl;
    }
};
// 偏特化
template <typename T, typename Allocator_T>
class MyVector<T*, const _T*> {
public:
    MyVector() {
        std::cout << "Partial version." << std::endl;
    }
};

MyVector<int, MyAnotherAllocator> v1;
MyVector<> v2;
MyVector<int, DefaultAllocator> v3;
MyVector<int*, const DefaultAllocator*> v3;
```

### 11、分配器-allocator

allocator.allocate -> ::operator new() -> malloc;
allocator.deallocate -> ::operator delete();

gcc 9 取消了
__pool_alloc：可以实现多个元素依次分配，节约malloc分配的管理内存

### 12、

### 13、 list

list只保留了一份指针

iterator操作中pre++和post++不一样，且返回类型不一样
self operator++(_Tp) // 后置++，返回对象，无法对原有对象进行操作
self& operator++()  // 前置++，返回引用，可以继续进行++操作

### 15、iterator

algrothm >> iterator >> container
算法提问，iterator回答

iterator一般包含5种特性
1. pointer
2. reference
3. value_type
4. difference_type
5. iterator_category

还需要萃取器来辅助区分class iterators和non-class iterators（比如指针，一种退化iterator），

这个就是iterator traits，在STL中可以使用偏特化特性来实现


### 16、

### 18、19、deque/queue/stack

deque内部实际分段连续
insert()会判断当前要插入元素离头端比较近还是尾端比较近，来选择插入

queue\stack 默认基于deque实现，也可以使用其他容器来实现_Sequence，可以理解为container adapter


### 20、RB tree

RB-Tree高度平衡二分查找树，提供两个函数insert_unique()和insert_equal()


### 21、set/multiset，map/multimap

set/multiset元素的key和value合一：value就是key

无法时使用iterator修改key和value修改，内部使用const_iterator禁止修改key、value，也是一种container adapter，内部都是使用rb_tree实现

map/multimap中，key和value不合一，无法使用iterator修改key但是可以修改data，内部自动将key_type设为const便可禁止修改key

