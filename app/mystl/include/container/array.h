#ifndef MYSTL_AEEAY_H_
#define MYSTL_AEEAY_H_

#include "allocator.h"
#include "iterator.h"
#include "construct.h"
#include "uninitialized.h"
#include "functional.h"
#include "functexcept.h"

namespace mystl {
template <typename T, std::size_t N>
struct array_traits {
    using Type = T[N];

    static constexpr T& ref(const Type& t, std::size_t n) noexcept {
        return const_cast<T&>(t[n]);
    }

    static constexpr T* ptr(const Type& t) noexcept {
        return const_cast<T*>(t);
    }
};

template <typename T>
struct array_traits<T, 0> {
    struct Type {};

    static constexpr T& ref(const Type&, std::size_t) noexcept {
        return *static_cast<T*>(nullptr);
    }

    static constexpr T* ptr(const Type&) noexcept {
        return nullptr;
    }
};

template <typename T, std::size_t N>
class array {
public: // member types
    using value_type = T;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    using AT_Type = array_traits<T, N>;
    typename AT_Type::Type elems;

public: // member functions
    /*
     * @brief Iterators
     */
    iterator begin() noexcept {
        return iterator(data());
    }
    const_iterator begin() const noexcept {
        return const_iterator(data());
    }

    iterator end() noexcept {
        return iterator(data() + N);
    }
    const_iterator end() const noexcept {
        return const_iterator(data() + N);
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }
    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(begin());
    }

    const_iterator cend() const noexcept {
        return const_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(rbegin());
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(rend());
    }

    /*
     * @brief Capacity
     */
    size_type size() const noexcept {
        return N;
    }

    size_type max_size() const noexcept {
        return N;
    }

    bool empty() const noexcept {
        return size() == 0U;
    }

    /*
     * @brief Element access
     */
    reference operator[](size_type n) noexcept {
        return AT_Type::ref(elems, n);
    }
    constexpr const_reference operator[](size_type n) const noexcept {
        return AT_Type::ref(elems, n);
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(n >= N, "out of range");
        return AT_Type::ref(elems, n);
    }

    constexpr const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(n >= N, "out of range");
        return AT_Type::ref(elems, n);
    }

    reference front() {
        return *begin();
    }
    const_reference front() const {
        return front();
    }

    reference back() {
        return N ? *(end() - 1) : *end();
    }
    const_reference back() const {
        return back();
    }

    value_type* data() noexcept {
        return AT_Type::ptr(elems);
    }
    const value_type* data() const noexcept {
        return data();
    }

    /*
     * @brief Modifiers
     */
    void fill(const value_type& val) {
        mystl::fill_n(begin(), size(), val);
    }

    void swap(array& x) noexcept(noexcept(mystl::swap(std::declval<value_type&>(), std::declval<value_type&>()))) {
        // mystl::swap_ranges(begin(), end(), x.begin());
    }
};
} // namespace mystl

#endif // MYSTL_AEEAY_H_
