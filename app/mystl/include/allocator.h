#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include "alloc.h"
#include "construct.h"
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
        return static_cast<pointer>(::operator new(n * sizeof(T)));
    }

    static void deallocate(pointer ptr) {
        ::operator delete(ptr);
    }

    static void deallocate(pointer ptr, size_type /*size*/) {
        ::operator delete(ptr);
    }

    static void construct(pointer ptr, const T& value) {
        mystl::construct<T>(ptr, value);
    }

    static void destroy(pointer ptr) {
        mystl::destroy<T>(ptr);
    }

    static void destroy(pointer first, pointer last) {
        mystl::destroy<T>(first, last);
    }
};
} // namespace mystl

#endif // MYSTL_ALLOCATOR_H_
