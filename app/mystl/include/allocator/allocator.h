#ifndef MYSTL_ALLOCATOR_H_
#define MYSTL_ALLOCATOR_H_

#include "construct.h"
#include "my_allocator.h"

#include <cstddef>
#include <memory>

namespace mystl {
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

    allocator() = default;
    allocator(const allocator&) = default;
    // 模板构造函数
    template <class U>
    allocator(const allocator<U>&){};
    ~allocator() = default;

    // 用于容器内部节点的内存分配
    template <class U>
    struct rebind {
        typedef allocator<U> other;
    };

    static pointer allocate(size_type n) {
        return reinterpret_cast<pointer>(mystl::MyAllocator::GetInstance()->Allocate(n * sizeof(value_type)));
    }

    static void deallocate(pointer ptr) {
        mystl::MyAllocator::GetInstance()->Deallocate(reinterpret_cast<unsigned char*>(ptr));
    }

    static void deallocate(pointer ptr, size_type /*size*/) {
        deallocate(ptr);
    }

    static void construct(pointer ptr, const T& value) {
        mystl::construct<T>(ptr, value);
    }

    template <typename U, typename... Args>
    static void construct(U* ptr, Args&&... args) {
        mystl::construct(ptr, std::forward<Args>(args)...);
    }

    static void destroy(pointer ptr) {
        mystl::destroy(ptr);
    }

    static void destroy(pointer first, pointer last) {
        mystl::destroy(first, last);
    }
};
} // namespace mystl

#endif // MYSTL_ALLOCATOR_H_
