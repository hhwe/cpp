#ifndef MYSTL_NUMERIC_H_
#define MYSTL_NUMERIC_H_

namespace mystl {

template <typename InputIterator, typename T>
T accumulate(InputIterator first, InputIterator last, T init) {
    while (first != last) {
        init = init + *first;
        ++first;
    }
    return init;
}

template <typename InputIterator, typename T, typename BinaryOperation>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOperation binary_op) {
    while (first != last) {
        init = binary_op(init, *first);
        ++first;
    }
    return init;
}

template <typename InputIterator, typename OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result) {
    if (first != last) {
        typename iterator_traits<InputIterator>::value_type val, prev;
        *result = prev = *first;
        while (++first != last) {
            val = *first;
            *++result = val - prev;
            prev = val;
        }
        ++result;
    }
    return result;
}

template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
OutputIterator adjacent_difference(InputIterator first, InputIterator last,
                                   OutputIterator result, BinaryOperation binary_op) {
    if (first != last) {
        typename iterator_traits<InputIterator>::value_type val, prev;
        *result = prev = *first;
        while (++first != last) {
            val = *first;
            *++result = binary_op(val, prev);
            prev = val;
        }
        ++result;
    }
    return result;
}

template <typename ForwardIterator, typename T>
void iota(ForwardIterator first, ForwardIterator last, T val) {
    while (first != last) {
        *first = val;
        ++first;
        ++val;
    }
}

template <typename InputIterator, typename OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result) {
    if (first != last) {
        typename iterator_traits<InputIterator>::value_type val = *first;
        *result = val;
        while (++first != last) {
            val = val + *first;
            *++result = val;
        }
        ++result;
    }
    return result;
}

template <typename InputIterator, typename OutputIterator, typename BinaryOperation>
OutputIterator partial_sum(InputIterator first, InputIterator last,
                           OutputIterator result, BinaryOperation binary_op) {
    if (first != last) {
        typename iterator_traits<InputIterator>::value_type val = *first;
        *result = val;
        while (++first != last) {
            val = binary_op(val, *first);
            *++result = val;
        }
        ++result;
    }
    return result;
}

template <typename InputIterator1, typename InputIterator2, typename T>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init) {
    while (first1 != last1) {
        init = init + (*first1) * (*first2);
        ++first1;
        ++first2;
    }
    return init;
}

template <typename InputIterator1, typename InputIterator2, typename T,
          typename BinaryOperation1, typename BinaryOperation2>
T inner_product(InputIterator1 first1, InputIterator1 last1,
                InputIterator2 first2, T init,
                BinaryOperation1 binary_op1,
                BinaryOperation2 binary_op2) {
    while (first1 != last1) {
        init = binary_op1(init, binary_op2(*first1, *first2));
        ++first1;
        ++first2;
    }
    return init;
}

} // namespace mystl

#endif // MYSTL_NUMERIC_H_
