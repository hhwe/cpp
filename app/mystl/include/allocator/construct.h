#ifndef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

#include <type_traits>

namespace MyStl {

// placement new, call T1::T1()
template <typename T1>
inline void construct(T1* ptr) {
    ::new (static_cast<void*>(ptr)) T1();
}

// placement new, call T1::T1(value)
template <typename T1, typename T2>
inline void construct(T1* ptr, const T2& value) {
    ::new (static_cast<void*>(ptr)) T1(value);
}

// placement new, call T1::T1(value)
template <typename T1, typename... Args>
inline void construct(T1* ptr, Args&&... args) {
    ::new (static_cast<void*>(ptr)) T1(std::forward<Args>(args)...);
}

// destroy, call T::~T()
template <typename T>
inline void destroy(T* ptr) {
    ptr->~T();
}

// type is not trivial
template <typename ForwardIterator>
inline void destroy_aux(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (; first < last; ++first) {
        destroy(&*first);
    }
}

// if type is trivial
template <typename ForwardIterator>
inline void destroy_aux(ForwardIterator first, ForwardIterator last, std::true_type) {
}

// circular dtor to destroy
// trivial classes支持静态初始化（static initizlization），如果一个类包含以下特点，那么它就是trivial
// 所有的拷贝构造函数都是trivial
// 所有的移动构造函数都是trivial
// 所有鹅赋值操作符都是trivial
// 所有的移动赋值操作符都是trivial
// 默认构造函数和析构函数是trivial
// https://docs.microsoft.com/en-us/cpp/cpp/trivial-standard-layout-and-pod-types
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    destroy_aux(first, last, std::is_trivial<ForwardIterator>::type);
}

} // namespace MyStl

#endif // MYSTL_CONSTRUCT_H_
