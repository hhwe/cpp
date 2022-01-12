#ifndef MYSTL_ITERATOR_H_
#define MYSTL_ITERATOR_H_

#include <cstddef>

namespace mystl {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

template <typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
          typename _Pointer = _Tp*, typename _Reference = _Tp&>
struct iterator {
    using iterator_category = _Category;
    using value_type = _Tp;
    using difference_type = _Distance;
    using pointer = _Pointer;
    using reference = _Reference;
};

} // namespace mystl

#endif // MYSTL_ITERATOR_H_
