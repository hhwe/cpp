#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

#include <cstddef>

namespace MyStl {

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
    using iterator_category = Iterator::Category;
    using value_type = Iterator::Tp;
    using difference_type = Iterator::Distance;
    using pointer = Iterator::Pointer;
    using reference = Iterator::Reference;
};
} // namespace MyStl

#endif // MYSTL_ITERATOR_H_
