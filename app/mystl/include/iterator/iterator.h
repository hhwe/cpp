#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

#include <cstddef>

namespace mystl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename Category, typename Tp, typename Distance = ptrdiff_t,
          typename Pointer = Tp*, typename Reference = Tp&>
struct iterator {
    using iterator_category = Category;
    using value_type = Tp;
    using difference_type = Distance;
    using pointer = Pointer;
    using reference = Reference;
};

template <typename Iterator>
struct iterator_traits {
    using iterator_category = typename Iterator::Category;
    using value_type = typename Iterator::Tp;
    using difference_type = typename Iterator::Distance;
    using pointer = typename Iterator::Pointer;
    using reference = typename Iterator::Reference;
};

// 针对 pointer 的偏特化版本
template <typename Iterator>
struct iterator_traits<Iterator*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = Iterator;
    using difference_type = ptrdiff_t;
    using pointer = Iterator*;
    using reference = Iterator&;
};

// 针对 pointer-to-const 的偏特化版本
template <typename Iterator>
struct iterator_traits<const Iterator*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = Iterator;
    using difference_type = ptrdiff_t;
    using pointer = const Iterator*;
    using reference = const Iterator&;
};

template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    using category = typename iterator_traits<Iterator>::iterator_category;
    return category();
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

} // namespace mystl

#endif // MYSTL_ITERATOR_H_
