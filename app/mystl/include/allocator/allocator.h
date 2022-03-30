#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include "construct.h"
#include "my_allocator.h"

#include <cstddef>
#include <memory>

namespace MyStl {
template <typename T>
class allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    static pointer allocate(size_type n) {
        return reinterpret_cast<pointer>(MyStl::MyAllocator::GetInstance()->Allocate(n));
    }

    static void deallocate(pointer ptr) {
        MyStl::MyAllocator::GetInstance()->Deallocate(reinterpret_cast<unsigned char*>(ptr));
    }

    static void deallocate(pointer ptr, size_type /*size*/) {
        deallocate(ptr);
    }

    static void construct(pointer ptr, const T& value) {
        MyStl::construct<T>(ptr, value);
    }

    static void destroy(pointer ptr) {
        MyStl::destroy<T>(ptr);
    }

    static void destroy(pointer first, pointer last) {
        MyStl::destroy<T>(first, last);
    }
};
} // namespace MyStl

#endif // MYSTL_ALLOCATOR_H_
