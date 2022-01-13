#ifndef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

#include <type_traits>

namespace mystl {

template <typename T1>
inline void construct(T1* ptr) {
    ::new (static_cast<void*>(ptr)) T1();
}

template <typename T1, typename T2>
inline void construct(T1* ptr, const T2& value) {
    ::new (static_cast<void*>(ptr)) T1(value);
}

template <typename T1, typename... Args>
inline void construct(T1* ptr, Args&&... args) {
    ::new (static_cast<void*>(ptr)) T1(std::forward<Args>(args)...);
}

template <typename ForwardIterator>
inline void __destroy(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (; first < last; ++first) {
        destroy(&*first);
    }
}

template <typename ForwardIterator>
inline void __destroy(ForwardIterator first, ForwardIterator last, std::true_type) {
}

template <typename T>
inline void destroy(T* ptr) {
    ptr->~T();
}

template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy(first, last, std::false_type); // TODO: type triats
}

} // namespace mystl

#endif // MYSTL_CONSTRUCT_H_
