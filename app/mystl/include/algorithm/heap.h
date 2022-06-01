#ifndef MYSTL_HEAP_H_
#define MYSTL_HEAP_H_

#include "functional.h"

namespace mystl {
template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void push_heap_aux(RandomAccessIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;
    while (holeIndex > topIndex && comp(*(first + parent), value)) {
        *(first + holeIndex) = mystl::move(*(first + parent));
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = mystl::move(value);
}

template <typename RandomAccessIterator>
void push_heap(RandomAccessIterator first, RandomAccessIterator last) {
    using ValueType = typename iterator_traits<RandomAccessIterator>::value_type;
    using DistanceType = typename iterator_traits<RandomAccessIterator>::difference_type;

    ValueType value = mystl::move(*(last - 1));
    push_heap_aux(first, DistanceType((last - first) - 1), DistanceType(0), mystl::move(value), mystl::less<ValueType>());
}

template <typename RandomAccessIterator, typename Compare>
void push_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    using ValueType = typename iterator_traits<RandomAccessIterator>::value_type;
    using DistanceType = typename iterator_traits<RandomAccessIterator>::difference_type;

    ValueType value = mystl::move(*(last - 1));
    push_heap_aux(first, DistanceType((last - first) - 1), DistanceType(0), mystl::move(value), comp);
}

template <typename RandomAccessIterator, typename Distance, typename T, typename Compare>
void adjust_heap(RandomAccessIterator first, Distance holeIndex, Distance len, T value, Compare comp) {
    Distance topIndex = holeIndex;
    Distance secondChild = holeIndex;
    while (secondChild < (len - 1) / 2) {
        secondChild = 2 * (secondChild + 1);
        if (comp(*(first + secondChild), *(first + (secondChild - 1)))) {
            secondChild--;
        }
        *(first + holeIndex) = mystl::move(*(first + secondChild));
        holeIndex = secondChild;
    }
    if (secondChild == len) {
        *(first + holeIndex) = mystl::move(*(first + (secondChild - 1)));
        holeIndex = secondChild - 1;
    }
    push_heap_aux(first, holeIndex, topIndex, mystl::move(value), comp);
}

template <typename RandomAccessIterator>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last) {
    using ValueType = typename iterator_traits<RandomAccessIterator>::value_type;
    using DistanceType = typename iterator_traits<RandomAccessIterator>::difference_type;
    if (last - first > 1) {
        ValueType value = mystl::move(*(last - 1));
        adjust_heap(first, DistanceType(0), DistanceType(last - first), mystl::move(value), mystl::less<ValueType>());
    }
}

template <typename RandomAccessIterator, typename Compare>
void pop_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    using ValueType = typename iterator_traits<RandomAccessIterator>::value_type;
    using DistanceType = typename iterator_traits<RandomAccessIterator>::difference_type;
    if (last - first > 1) {
        ValueType value = mystl::move(*(last - 1));
        adjust_heap(first, DistanceType(0), DistanceType(last - first), mystl::move(value), comp);
    }
}

template <typename RandomAccessIterator>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last) {
    while (last - first > 1) {
        pop_heap(first, last--);
    }
}

template <typename RandomAccessIterator, typename Compare>
void sort_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    while (last - first > 1) {
        pop_heap(first, last--, comp);
    }
}

template <typename RandomAccessIterator, typename Compare>
void make_heap_aux(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    using ValueType = typename iterator_traits<RandomAccessIterator>::value_type;
    using DistanceType = typename iterator_traits<RandomAccessIterator>::difference_type;

    if (last - first < 2) return;

    const DistanceType len = last - first;
    DistanceType parent = (len - 2) / 2;

    while (true) {
        ValueType value = mystl::move(*(first + parent));
        mystl::adjust_heap(first, parent, len, value, comp);
        if (parent == 0) {
            return;
        }
        parent--;
    }
}

template <typename RandomAccessIterator>
void make_heap(RandomAccessIterator first, RandomAccessIterator last) {
    using ValueType = typename iterator_traits<RandomAccessIterator>::value_type;

    make_heap_aux(first, last, mystl::less<ValueType>());
}

template <typename RandomAccessIterator, typename Compare>
void make_heap(RandomAccessIterator first, RandomAccessIterator last, Compare comp) {
    make_heap_aux(first, last, comp);
}
} // namespace mystl

#endif // MYSTL_HEAP_H_
