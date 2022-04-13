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

namespace {
// 2408. SFINAE-friendly common_type/iterator_traits is missing in C++14

template <typename T>
struct has_iterator_cat {
private:
    struct two {
        char a;
        char b;
    };
    template <typename U>
    static two test(...);
    template <typename U>
    static char test(typename U::iterator_category* = 0);

public:
    static const bool value = sizeof(test<T>(0)) == sizeof(char);
};

template <typename Iterator, bool>
struct iterator_traits_impl {};

template <typename Iterator>
struct iterator_traits_impl<Iterator, true> {
    using iterator_category = typename Iterator::Category;
    using value_type = typename Iterator::Tp;
    using difference_type = typename Iterator::Distance;
    using pointer = typename Iterator::Pointer;
    using reference = typename Iterator::Reference;
};

template <typename Iterator, bool>
struct iterator_traits_helper {};

template <typename Iterator>
struct iterator_traits_helper<Iterator, true>
    : public iterator_traits_impl<Iterator,
                                  std::is_convertible<
                                      typename Iterator::iterator_category, input_iterator_tag>::value
                                      || std::is_convertible<
                                          typename Iterator::iterator_category, output_iterator_tag>::value> {
};
} // namespace

template <typename Iterator>
struct iterator_traits : public iterator_traits_helper<Iterator, has_iterator_cat<Iterator>::value> {
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

// 萃取迭代器的类型
template <typename Iterator>
inline typename iterator_traits<Iterator>::iterator_category
iterator_category(const Iterator&) {
    using category = typename iterator_traits<Iterator>::iterator_category;
    return category();
}

// 萃取迭代器的元素类型
template <typename Iterator>
inline typename iterator_traits<Iterator>::value_type*
value_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::value_type*>(0);
}

// 萃取迭代器的两个元素间距
template <typename Iterator>
inline typename iterator_traits<Iterator>::difference_type*
distance_type(const Iterator&) {
    return static_cast<typename iterator_traits<Iterator>::difference_type*>(0);
}

// ====================================================================================

// 反向迭代器
template <typename Iterator>
class reverse_iterator : public iterator_traits<Iterator> {
public: // member types
    using iterator_type = Iterator;
    using self = reverse_iterator<Iterator>;

    using iterator_category = typename iterator_traits<Iterator>::iterator_category;
    using value_type = typename iterator_traits<Iterator>::value_type;
    using difference_type = typename iterator_traits<Iterator>::difference_type;
    using pointer = typename iterator_traits<Iterator>::pointer;
    using reference = typename iterator_traits<Iterator>::reference;

public:
    reverse_iterator() {
    }
    explicit reverse_iterator(iterator_type x) :
        current_(x) {
    }

    iterator_type base() const {
        return current_;
    }

    reference operator*() const {
        iterator_type tmp = current_;
        return *--tmp;
    }

    pointer operator->() {
        return &(*this);
    }

    self& operator++() {
        --current_;
        return *this;
    }

    self& operator++(int) {
        self tmp = *this;
        --current_;
        return tmp;
    }

    self& operator--() {
        ++current_;
        return *this;
    }

    self& operator--(int) {
        self tmp = *this;
        ++current_;
        return tmp;
    }

private:
    iterator_type current_;
};

template <typename Iterator>
inline bool operator==(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return x.base() == y.base();
}

template <typename Iterator>
inline bool operator!=(const reverse_iterator<Iterator>& x, const reverse_iterator<Iterator>& y) {
    return !(x == y);
}

template <typename InputIterator>
using RequireInputIterator = typename std::enable_if<std::is_convertible<
    typename iterator_traits<InputIterator>::iterator_category,
    input_iterator_tag>::value>::type;
} // namespace mystl

#endif // MYSTL_ITERATOR_H_
