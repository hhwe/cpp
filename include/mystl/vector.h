#ifndef MYSTL_VECTOE_H_
#define MYSTL_VECTOE_H_

#include "allocator.h"
#include "construct.h"

namespace mystl {

template <typename T, typename Alloc = mystl::allocator<T>>
class vector {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = value_type*;
    using const_iterator = const value_type*;
    using data_allocator = Alloc;

    // Iterators
    iterator begin() {
        return begin_;
    }

    iterator end() {
        return end_;
    }

    // Capacity
    size_type size() const {
        return size_type(end_ - begin_);
    }

    size_type capacity() const {
        return size_type(capacity_ - begin_)
    }

    bool empty() const {
        return begin_ == end_;
    }

    // Element access
    reference operator[](size_type n) {
        return *(begin_ + n);
    }

    reference at(size_type n) {
        return *(begin_ + n);
    }

    reference front() {
        return *(begin_);
    }

    reference back() {
        return *(end_);
    }

    // Modifiers
    void realloc_insert(iterator pos, const_reference val) {
        const size_type len = size() != 0 ? 2 * size() : 1;
        iterator new_begin = Alloc::allocate(len);
        iterator new_end = new_begin;
        try {
            new_end = uninitialized_copy(begin_, pos, new_begin); // TODO: use move
            construct(new_end, val);
            ++new_end;
            // new_end = uninitialized_copy(pos, end_, new_end);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            destroy(new_begin, new_end);
            Alloc::deallocate(new_begin, len);
            throw;
        }
        destroy(begin_, end_);
        deallocate();

        begin_ = new_begin;
        end_ = new_end;
        capacity_ = new_begin + len;
    }

    void push_back(const value_type& val) {
        if (end_ != capacity_) {
            construct(end_, val);
            ++end_;
        } else {
            realloc_insert(end_, val);
        }
    }

    void pop_back() {
        --end_;
        destroy(end_);
    }

    iterator erase(iterator first, iterator last) {
        iterator i = copy(last, end_, first);
    }

    iterator erase(iterator pos) {
        if (pos + 1 != end_) {
            copy(pos + 1, end_, pos);
        }
        --end_;
        destroy(end_);
        return pos;
    }

    void clear() {
        erase(begin_, end_);
    }

    // Constructor and Destructor
    void fill_initialize(size_type n, const_reference val) {
        begin_ = Alloc::allocate(n);
        std::uninitialized_fill_n(result, n, x);
        end_ = begin_ + n;
        capacity_ = end_;
    }

    vector() :
        begin_(0), end_(0), capacity_(0) {
    }

    vector(size_type n, const_reference val) {
        fill_initialize(n, val);
    }

    explicit vector(size_type n) {
        fill_initialize(n, T());
    }

    ~vector() {
        Alloc::destroy(begin_, end_);
        Alloc::deallocate(begin_, end_);
    }

protected:
    iterator begin_;
    iterator end_;
    iterator capacity_;
};

} // namespace mystl

#endif // MYSTL_VECTOE_H_
