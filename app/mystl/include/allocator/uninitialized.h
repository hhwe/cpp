#ifndef MYSTL_UNINITIALIZED_H_
#define MYSTL_UNINITIALIZED_H_

#include <type_traits>
#include "algorithm.h"

namespace mystl {

namespace {
template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy_aux(InputIterator first, InputIterator last,
                                       ForwardIterator result, std::true_type) {
    return mystl::copy(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy_aux(InputIterator first, InputIterator last,
                                       ForwardIterator result, std::false_type) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            mystl::construct(&*cur, *first);
        }
    } catch (...) {
        mystl::destroy(result, cur);
        throw;
    }
    return cur;
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_move_aux(InputIterator first, InputIterator last,
                                       ForwardIterator result, std::true_type) {
    return mystl::move(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_move_aux(InputIterator first, InputIterator last,
                                       ForwardIterator result, std::false_type) {
    ForwardIterator cur = result;
    try {
        for (; first != last; ++first, ++cur) {
            mystl::construct(&*cur, mystl::move(*first));
        }
    } catch (...) {
        mystl::destroy(result, cur);
        throw;
    }
    return cur;
}

template <typename ForwardIterator, typename T>
void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& val, std::true_type) {
    return mystl::fill(first, last, val);
}

template <typename ForwardIterator, typename T>
void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& val, std::false_type) {
    ForwardIterator cur = first;
    try {
        for (; cur != last; ++cur) {
            mystl::construct(&*cur, val);
        }
    } catch (...) {
        mystl::destroy(first, cur);
        throw;
    }
    return;
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& val, std::true_type) {
    return mystl::fill_n(first, n, val);
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& val, std::false_type) {
    ForwardIterator cur = first;
    try {
        for (; n--; ++cur) {
            mystl::construct(&*cur, val);
        }
    } catch (...) {
        mystl::destroy(first, cur);
        throw;
    }
    return cur;
}
} // namespace

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    return uninitialized_copy_aux(
        first, last, result, std::is_pod<typename iterator_traits<ForwardIterator>::value_type>());
}

template <typename ForwardIterator, typename T>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& val) {
    return uninitialized_fill_aux(
        first, last, val, std::is_pod<typename iterator_traits<ForwardIterator>::value_type>());
}

template <typename ForwardIterator, typename Size, typename T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& val) {
    return uninitialized_fill_n_aux(first, n, val, std::is_pod<T>());
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    return uninitialized_move_aux(
        first, last, result, std::is_pod<typename iterator_traits<ForwardIterator>::value_type>());
}

} // namespace mystl

#endif // MYSTL_UNINITIALIZED_H_
