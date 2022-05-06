#ifndef MYSTL_ARRAY_TEST_H_
#define MYSTL_ARRAY_TEST_H_

#include "array.h"
#include "htest.h"
#include "htest_utils.h"

#include <list>
#include <string>
#include <array>
#include <random>

namespace mystl {
namespace test {
namespace array_test {
TEST(array) {
    {
        std::array<int, 10> a{0};
    }
}
}
}
} // namespace mystl::test::array_test
#endif // MYSTL_ARRAY_TEST_H_
