#ifndef MYSTL_UNINITIALIZED_H_
#define MYSTL_UNINITIALIZED_H_

#include <type_traits>
#include "algorithm.h"

namespace mystl {

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    // TODO typename __type_traits<T>::is_POD_type;
    return copy(first, last, result);
}

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_fill(InputIterator first, InputIterator last,
                                   ForwardIterator result) {
    // TODO typename __type_traits<T>::is_POD_type;
    return fill(first, last, result);
}

template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, std::true_type) {
    return mystl::fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, std::false_type) {
    ForwardIterator cur = first;
    try {
        for (; n--; ++cur) {
            mystl::construct(&*cur, x);
        }
    } catch (...) {
        mystl::destroy(first, cur);
    }
    return cur;
}

template <class ForwardIterator, class Size, class T>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
    return uninitialized_fill_n_aux(first, n, x, std::is_pod<T>());
}
} // namespace mystl

#endif // MYSTL_UNINITIALIZED_H_
