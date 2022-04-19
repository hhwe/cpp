#ifndef MYSTL_FUNCTEXCEPT_H_
#define MYSTL_FUNCTEXCEPT_H_

#include <stdexcept>
#include <cassert>

namespace mystl {

#define MYSTL_DEBUG(expr) assert(expr)

#define THROW_LENGTH_ERROR_IF(expr, what) \
    do {                                  \
        if (expr) {                       \
            throw std::length_error(what) \
        }                                 \
    } while (0U);

#define THROW_OUT_OF_RANGE_IF(expr, what) \
    do {                                  \
        if (expr) {                       \
            throw std::out_of_range(what) \
        }                                 \
    } while (0U);

#define THROW_RUNTIME_ERROR_IF(expr, what) \
    do {                                   \
        if (expr) {                        \
            throw std::runtime_error(what) \
        }                                  \
    } while (0U);

} // namespace mystl

#endif // MYSTL_FUNCTEXCEPT_H_
