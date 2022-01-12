#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include <cstddef>

namespace mystl {
template <typename T>
class allocator {
private:
    /* data */
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static pointer allocate(size_type n) {
        return ::operator new(n * sizeof(T));
    }

    static void deallocate(pointer p) {
        return ::operator delete(p);
    }

    static void construct(pointer p, const T& value) {
        ::new ((void*)p) p->T(value);
    }

    static void destroy(pointer p) {
        p->~T();
    }
};
} // namespace mystl

#endif // MYSTL_ALLOCATOR_H_
