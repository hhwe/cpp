#ifndef MYSTL_CONSTRUCT_H_
#define MYSTL_CONSTRUCT_H_

#include <cstddef>

namespace mystl {

template <typename T1>
inline void construct(T1* p) {
    ::new (static_cast<void*>(p)) T1();
}
template <typename T1, typename T2>
inline void construct(T1* p, const T2& value) {
    ::new (static_cast<void*>(p)) T1(value);
}
template <typename T1, typename... Args>
inline void construct(T1* p, Args&&... args) {
    ::new (static_cast<void*>(p)) T1(std::forward<Args>(args)...);
}

template <typename T>
inline void destroy(T* p) {
    p->~T();
}

} // namespace mystl

#endif // MYSTL_CONSTRUCT_H_
