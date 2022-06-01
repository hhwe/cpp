#ifndef MYSTL_ALGORITHM_H_
#define MYSTL_ALGORITHM_H_

#include <type_traits>
#include <cstring>
#include "utility.h"
#include "heap.h"

namespace mystl {

template <class ForwardIterator>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last) {
    if (first != last) {
        ForwardIterator next = first;
        ++next;
        while (next != last) {
            if (*first == *next)
                return first;
            ++first;
            ++next;
        }
    }
    return last;
}

template <class ForwardIterator, class BinaryPredicate>
ForwardIterator adjacent_find(ForwardIterator first, ForwardIterator last,
                              BinaryPredicate pred) {
    if (first != last) {
        ForwardIterator next = first;
        ++next;
        while (next != last) {
            if (pred(*first, *next))
                return first;
            ++first;
            ++next;
        }
    }
    return last;
}

template <class InputIterator, class UnaryPredicate>
bool all_of(InputIterator first, InputIterator last, UnaryPredicate pred) {
    while (first != last) {
        if (!pred(*first)) return false;
        ++first;
    }
    return true;
}

template <class InputIterator, class UnaryPredicate>
bool any_of(InputIterator first, InputIterator last, UnaryPredicate pred) {
    while (first != last) {
        if (pred(*first)) return true;
        ++first;
    }
    return false;
}

template <class InputIterator1, class InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    while (first1 != last1) {
        if (!(*first1 == *first2))
            return false;
        ++first1;
        ++first2;
    }
    return true;
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1,
           InputIterator2 first2, BinaryPredicate pred) {
    while (first1 != last1) {
        if (!pred(*first1, *first2))
            return false;
        ++first1;
        ++first2;
    }
    return true;
}

template <class ForwardIterator, class T>
void fill(ForwardIterator first, ForwardIterator last, const T& val) {
    while (first != last) {
        *first = val;
        ++first;
    }
}

template <class OutputIterator, class Size, class T>
OutputIterator fill_n(OutputIterator first, Size n, const T& val) {
    while (n > 0) {
        *first = val;
        ++first;
        --n;
    }
    return first; // since C++11
}

template <class ForwardIterator1, class ForwardIterator2>
void iter_swap(ForwardIterator1 a, ForwardIterator2 b) {
    swap(*a, *b);
}

template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1) {
        if (first2 == last2 || *first2 < *first1)
            return false;
        else if (*first1 < *first2)
            return true;
        ++first1;
        ++first2;
    }
    return (first2 != last2);
}

template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             Compare comp) {
    while (first1 != last1) {
        if (first2 == last2 || comp(*first2, *first1))
            return false;
        else if (comp(*first1, *first2))
            return true;
        ++first1;
        ++first2;
    }
    return (first2 != last2);
}

template <class T>
const T& max(const T& a, const T& b) {
    return (a < b) ? b : a;
}

template <class T, typename Compare>
const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? b : a;
}

template <class T>
const T& min(const T& a, const T& b) {
    return (a > b) ? b : a;
}

template <class T, typename Compare>
const T& min(const T& a, const T& b, Compare comp) {
    return comp(b, a) ? b : a;
}

template <class InputIterator1, class InputIterator2>
pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    while ((first1 != last1) && (*first1 == *first2)) {
        ++first1;
        ++first2;
    }
    return make_pair(first1, first2);
}

template <class InputIterator1, class InputIterator2, class BinaryPredicate>
pair<InputIterator1, InputIterator2>
mismatch(InputIterator1 first1, InputIterator1 last1,
         InputIterator2 first2, BinaryPredicate pred) {
    while ((first1 != last1) && pred(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return make_pair(first1, first2);
}

template <class InputIterator, class OutputIterator>
OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
    while (first != last) {
        *result = *first;
        ++result;
        ++first;
    }
    return result;
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator __copy_d(RandomAccessIterator first, RandomAccessIterator last, OutputIterator result, Distance*) {
    Distance n = last - first;
    while (n > 0) {
        *result = *first;
        ++result;
        ++first;
        --n;
    }
    return result;
}

template <class InputIterator, class OutputIterator>
OutputIterator __copy(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
    return __copy_d(first, last, result, distance_type(first));
}

template <class InputIterator, class OutputIterator>
OutputIterator __copy_dispatch(InputIterator first, InputIterator last, OutputIterator result) {
    return __copy<InputIterator, OutputIterator>(first, last, result, iterator_category(first));
}

template <class T>
T* __copy_t(T* first, T* last, T* result, std::true_type) {
    std::memmove(result, first, (last - first) * sizeof(T));
    return result;
}

template <class T>
T* __copy_t(T* first, T* last, T* result, std::false_type) {
    return __copy_d(first, last, result, (ptrdiff_t*)0);
}

template <class T>
T* __copy_dispatch(T* first, T* last, T* result) {
    return __copy_t(first, last, result, std::is_trivial<T>::type);
}

template <class T>
T* __copy_dispatch(const T* first, const T* last, T* result) {
    return __copy_t(first, last, result, std::is_trivial<T>::type);
}

template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result) {
    return __copy_dispatch<InputIterator, OutputIterator>(first, last, result);
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
                                     BidirectionalIterator1 last,
                                     BidirectionalIterator2 result) {
    while (last != first) { *(--result) = *(--last); }
    return result;
}

/*
 *@bug multiple definition

// 针对 char* 的特化版本
template <>
char* copy(const char* first, const char* last, char* result) {
    std::memmove(result, first, last - first);
    return result;
}

// 针对 wchar_t* 的特化版本
template <>
wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result) {
    std::memmove(result, first, (last - first) * sizeof(wchar_t));
    return result;
}

*/

template <class InputIterator, class OutputIterator>
OutputIterator move(InputIterator first, InputIterator last, OutputIterator result) {
    while (first != last) {
        *result = mystl::move(*first);
        ++result;
        ++first;
    }
    return result;
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 move_backward(BidirectionalIterator1 first,
                                     BidirectionalIterator1 last,
                                     BidirectionalIterator2 result) {
    while (last != first) *(--result) = std::move(*(--last));
    return result;
}

/*
 * set
 */
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++result;
            ++first1;
        } else if (*first2 < *first1)
            ++first2;
        else {
            ++first1;
            ++first2;
        }
    }
    return copy(first1, last1, result);
}

template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
OutputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                              InputIterator2 first2, InputIterator2 last2,
                              OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++result;
            ++first1;
        } else if (comp(*first2, *first1))
            ++first2;
        else {
            ++first1;
            ++first2;
        }
    }
    return copy(first1, last1, result);
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else {
            *result = *first1;
            ++result;
            ++first1;
            ++first2;
        }
    }
    return result;
}

template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
OutputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                                InputIterator2 first2, InputIterator2 last2,
                                OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2))
            ++first1;
        else if (comp(*first2, *first1))
            ++first2;
        else {
            *result = *first1;
            ++result;
            ++first1;
            ++first2;
        }
    }
    return result;
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++result;
            ++first1;
        } else if (*first2 < *first1) {
            *result = *first2;
            ++result;
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
OutputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                                        InputIterator2 first2, InputIterator2 last2,
                                        OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++result;
            ++first1;
        } else if (*first2 < *first1) {
            *result = *first2;
            ++result;
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            *result = *first1;
            ++first1;
        } else if (*first2 < *first1) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIterator1, class InputIterator2,
          class OutputIterator, class Compare>
OutputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                         InputIterator2 first2, InputIterator2 last2,
                         OutputIterator result, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        } else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

/*
 * search
 */
template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last,
                   const T& val) {
    if (first == last) return false;
    ForwardIterator mid = (last + first) / 2U;
    if (*mid == val) return true;
    return binary_search(first, mid) || binary_search(mid, last);
}

template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last,
                   const T& val, Compare comp);

} // namespace mystl

#endif // MYSTL_ALGORITHM_H_
