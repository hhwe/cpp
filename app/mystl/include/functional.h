#ifndef MYSTL_FUNCTIONAL_H_
#define MYSTL_FUNCTIONAL_H_

#include <type_traits>

namespace mystl {

template <typename Arg, typename Result>
struct unary_function {
    /// @c argument_type is the type of the argument
    typedef Arg argument_type;

    /// @c result_type is the return type
    typedef Result result_type;
};

template <typename Arg1, typename Arg2, typename Result>
struct binary_function {
    /// @c first_argument_type is the type of the first argument
    typedef Arg1 first_argument_type;

    /// @c second_argument_type is the type of the second argument
    typedef Arg2 second_argument_type;

    /// @c result_type is the return type
    typedef Result result_type;
};

/*
 * Operator classes
 */
template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x > y;
    }
};

template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x < y;
    }
};
} // namespace mystl

#endif // MYSTL_FUNCTIONAL_H_
