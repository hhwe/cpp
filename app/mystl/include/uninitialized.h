#ifndef MYSTL_UNINITIALIZED_H_
#define MYSTL_UNINITIALIZED_H_

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

template <typename InputIterator, typename ForwardIterator>
ForwardIterator uninitialized_fill_n(InputIterator first, std::size_t n,
                                     ForwardIterator result) {
    // TODO typename __type_traits<T>::is_POD_type;
    return fill_n(first, n, result);
}

} // namespace mystl

#endif // MYSTL_UNINITIALIZED_H_
