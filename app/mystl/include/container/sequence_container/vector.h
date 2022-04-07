#ifndef MYSTL_VECTOE_H_
#define MYSTL_VECTOE_H_

#include <iostream>
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"

namespace mystl {

template <typename T, typename Alloc = mystl::allocator<T>>
class vector {
public: // member types
    using allocator_type = Alloc;
    using data_allocator = Alloc;

    using value_type = typename allocator_type::value_type;
    using pointer = typename allocator_type::pointer;
    using const_pointer = typename allocator_type::const_pointer;
    using reference = typename allocator_type::reference;
    using const_reference = typename allocator_type::const_reference;
    using size_type = typename allocator_type::size_type;
    using difference_type = typename allocator_type::difference_type;

    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

public: // member functions
    /*
     * Constructor and Destructor
     */
    vector() :
        start_(0), finish_(0), capacity_(0) {
    }

    vector(size_type n, const_reference val) {
        fill_initialize(n, val);
    }

    vector(size_type n) {
        fill_initialize(n, T());
    }

    ~vector() {
        data_allocator::destroy(start_, finish_);
        data_allocator::deallocate(start_, capacity_ - finish_);
    }

    /*
     * Iterators
     */
    iterator begin() noexcept {
        return start_;
    }

    iterator end() noexcept {
        return finish_;
    }

    const_iterator begin() const noexcept {
        return start_;
    }

    const_iterator end() const noexcept {
        return finish_;
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept {
        return end();
    }

    const_reverse_iterator rend() const noexcept {
        return begin();
    }

    const_iterator cbegin() const noexcept {
        return begin();
    }

    const_iterator cend() const noexcept {
        return end();
    }

    const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept {
        return rend();
    }
    /*
     * Capacity
     */
    size_type size() const noexcept {
        return static_cast<size_type>(-1) / sizeof(value_type);
    }

    size_type max_size() const noexcept {
        return static_cast<size_type>(capacity_ - start_);
    }

    void resize(size_type n) {
        if (n < size()) {
            data_allocator::destroy(start_ + n, finish_);
            finish_ = start_ + n;
        } else if (n > size() && n <= capacity()) {
            // TODO:
        }
    }

    void resize(size_type n, const value_type& val) {
    }

    size_type capacity() const noexcept {
        return size_type(capacity_ - start_);
    }

    bool empty() const noexcept {
        return start_ == finish_;
    }

    void reserve(size_type n) {
        // TODO:
    }

    void shrink_to_fit();

    /*
     * Element access
     */
    reference operator[](size_type n) {
        return *(start_ + n);
    }

    const_reference operator[](size_type n) const {
        return *(start_ + n);
    }

    reference at(size_type n) {
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        return (*this)[n];
    }

    reference front() {
        return *(start_);
    }

    const_reference front() const {
        return *(start_);
    }

    reference back() {
        return *(finish_ - 1);
    }

    const_reference back() const {
        return *(finish_ - 1);
    }

    pointer data() noexcept {
        return start_;
    }

    const_pointer data() const noexcept {
        return start_;
    }

    /*
     * Modifiers
     */
    template <class InputIterator>
    void assign(InputIterator first, InputIterator last);

    void assign(size_type n, const value_type& val);

    // void assign(initializer_list<value_type> il);

    void push_back(const value_type& val) {
        insert(finish_, val);
    }

    void pop_back() {
        --finish_;
        data_allocator::destroy(finish_);
    }

    iterator insert(const_iterator position, const value_type& val);
    iterator insert(const_iterator position, size_type n, const value_type& val);
    template <class InputIterator>
    iterator insert(const_iterator position, InputIterator first, InputIterator last);
    iterator insert(const_iterator position, value_type&& val);
    // iterator insert(const_iterator position, initializer_list<value_type> il);

    iterator erase(iterator first, iterator last);

    iterator erase(iterator pos);

    void swap(vector& x);
    void clear() {
        erase(start_, finish_);
    }

    template <class... Args>
    iterator emplace(const_iterator position, Args&&... args);
    template <class... Args>
    void emplace_back(Args&&... args);

private:
    void fill_initialize(size_type n, const_reference val) {
        start_ = data_allocator::allocate(n);
        mystl::uninitialized_fill_n(start_, n, val);
        finish_ = start_ + n;
        capacity_ = finish_;
    }

    void realloc_insert(iterator pos, const_reference val) {
        const size_type len = size() != 0 ? 2 * size() : 1;
        iterator new_begin = Alloc::allocate(len);
        iterator new_end = new_begin;
        try {
            new_end = uninitialized_copy(start_, pos, new_begin); // TODO: use move
            construct(new_end, val);
            ++new_end;
            // new_end = uninitialized_copy(pos, finish_, new_end);
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
            destroy(new_begin, new_end);
            Alloc::deallocate(new_begin, len);
            throw;
        }
        destroy(start_, finish_);
        // deallocate(); TODO：

        start_ = new_begin;
        finish_ = new_end;
        capacity_ = new_begin + len;
    }

protected:
    iterator start_;
    iterator finish_;
    iterator capacity_;
};

} // namespace mystl

#endif // MYSTL_VECTOE_H_
