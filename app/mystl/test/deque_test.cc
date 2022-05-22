#ifndef MYSTL_DEQUE_TEST_H_
#define MYSTL_DEQUE_TEST_H_

#include "deque.h"
#include "htest.h"
#include "htest_utils.h"

#include <list>
#include <string>
#include <array>
#include <random>
#include <deque>

namespace mystl {
namespace test {
namespace deque_test {
TEST(deque) {
    {
        std::deque<int> dq1(10, 0);
        mystl::deque<int> dq2(10, 0);
        EXPECT_TRUE(htest::ContainerEqual(dq1, dq2));

        int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::deque<int> dq3(std::begin(arr), std::end(arr));
        mystl::deque<int> dq4(std::begin(arr), std::end(arr));
        EXPECT_TRUE(htest::ContainerEqual(dq3, dq4));

        auto dq5(dq1);
        auto dq6(dq2);
        EXPECT_TRUE(htest::ContainerEqual(dq5, dq6));

        auto dq7 = dq3;
        auto dq8 = dq4;
        EXPECT_TRUE(htest::ContainerEqual(dq7, dq8));

        auto dq9 = std::move(dq7);
        auto dq10 = std::move(dq8);
        EXPECT_TRUE(htest::ContainerEqual(dq9, dq10));
    }

    {
        mystl::deque<int> dq1;
        assert(dq1.empty());
        assert(dq1.size() == 0);

        mystl::deque<int> dq2(10, 0);
        assert(!dq2.empty());
        assert(dq2.size() == 10);
    }

    {
        std::deque<std::string> dq1(10, "10");
        mystl::deque<std::string> dq2(10, "10");

        dq1[0] = "0";
        dq1[9] = "9";
        dq2[0] = "0";
        dq2[9] = "9";

        assert(dq1.front() == dq2.front());
        assert(dq1.back() == dq2.back());
    }

    {
        std::deque<int> dq1;
        mystl::deque<int> dq2;

        for (auto i = 0; i != 1000; ++i) {
            dq1.push_back(i);
            dq2.push_back(i);
        }
        EXPECT_TRUE(htest::ContainerEqual(dq1, dq2));

        for (auto i = 1000; i != 2000; ++i) {
            dq1.push_front(i);
            dq2.push_front(i);
        }
        EXPECT_TRUE(htest::ContainerEqual(dq1, dq2));

        for (auto i = 0; i != 1000; ++i) {
            dq1.pop_back();
            dq2.pop_back();
        }
        EXPECT_TRUE(htest::ContainerEqual(dq1, dq2));

        for (auto i = 0; i != 1000; ++i) {
            dq1.pop_front();
            dq2.pop_front();
        }
        EXPECT_TRUE(htest::ContainerEqual(dq1, dq2));
    }

    {
        int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        mystl::deque<int> foo(arr, arr + 3), bar(arr + 3, arr + 10);

        assert(foo.size() == 3 && bar.size() == 7);
        foo.swap(bar);
        assert(foo.size() == 7 && bar.size() == 3);
        mystl::swap(foo, bar);
        assert(foo.size() == 3 && bar.size() == 7);
    }

    {
        int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        mystl::deque<int> foo1(arr, arr + 3), bar(arr + 3, arr + 10);

        assert(foo1 != bar);
        auto foo2 = bar;
        assert(foo2 == bar);
    }
}
}
}
} // namespace mystl::test::deque_test
#endif // MYSTL_DEQUE_TEST_H_
