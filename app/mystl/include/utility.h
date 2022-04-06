#ifndef MYSTL_UTILITY_H_
#define MYSTL_UTILITY_H_

#include <type_traits>

namespace mystl {

template <class T1, class T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;
};

template <class T1, class T2>
pair<T1, T2> make_pair(T1&& x, T2&& y) {
    return (pair<T1, T2>(forward<T1>(x), forward<T2>(y)));
}

// Forward an lvalue.
template <typename T>
constexpr T&& forward(typename std::remove_reference<T>::type& arg) noexcept {
    return static_cast<T&&>(arg);
}

// Forward an rvalue.
template <typename T>
constexpr T&& forward(typename std::remove_reference<T>::type&& arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "template argument"
                                                       " substituting T is an lvalue reference type");
    return static_cast<T&&>(arg);
}

template <class T>
constexpr typename std::remove_reference<T>::type&& move(T&& arg) noexcept {
    return static_cast<std::remove_reference<T>::type&&>(arg);
}

template <class T>
void swap(T& a, T& b) {
    T c(std::move(a));
    a = std::move(b);
    b = std::move(c);
}

// template <class T, std::size_t N>
// void swap(T& a[N], T& b[N]) {
//     for (std::size_t i = 0; i < N; ++i) swap(a[i], b[i]);
// }

} // namespace mystl

#endif // MYSTL_UTILITY_H_