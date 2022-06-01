#ifndef MYSTL_QUEUE_TEST_H_
#define MYSTL_QUEUE_TEST_H_

#include "queue.h"
#include "htest.h"
#include "htest_utils.h"

#include <list>
#include <string>
#include <array>
#include <random>
#include <queue>

namespace mystl {
namespace test {
namespace queue_test {
TEST(queue) {
    {
        std::queue<int> q1;
        mystl::queue<int> q2;

        for (auto i = 0; i != 10; ++i) {
            q1.push(i);
            q2.push(i);
        }
        for (auto i = 0; i != 10; ++i) {
            EXPECT_TRUE(q1.front() == q2.front());
            q1.pop();
            q2.pop();
        }
    }

    {
        mystl::queue<int> q1;
        for (auto i = 0; i != 10; ++i)
            q1.push(i);
        auto q2(q1);
        EXPECT_TRUE(q1 == q2);
        EXPECT_TRUE(!(q1 != q2));
    }

    {
        mystl::queue<int> q;
        EXPECT_TRUE(q.empty());
        EXPECT_TRUE(q.size() == 0);

        q.push(10);
        q.push(11);
        EXPECT_TRUE(!q.empty());
        EXPECT_TRUE(q.size() == 2);
    }

    {
        mystl::queue<std::string> q;
        q.push("front");
        q.push("back");

        EXPECT_TRUE(q.front() == "front");
        EXPECT_TRUE(q.back() == "back");
    }

    {
        mystl::queue<int> q1, q2;

        q1.push(1);
        q1.push(2);
        q1.push(3);
        q2.push(1);
        q2.push(2);

        EXPECT_TRUE(q1.size() == 3 && q2.size() == 2);
        q1.swap(q2);
        EXPECT_TRUE(q1.size() == 2 && q2.size() == 3);
        mystl::swap(q1, q2);
        EXPECT_TRUE(q1.size() == 3 && q2.size() == 2);
    }
}

TEST(priority_queue) {
    {
        int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, -1, -2, -3};
        std::priority_queue<int> pq1(std::begin(arr), std::end(arr));
        mystl::priority_queue<int> pq2(std::begin(arr), std::end(arr));

        while (!pq1.empty() && !pq2.empty()) {
            EXPECT_EQ(pq1.top(), pq2.top());
            pq1.pop();
            pq2.pop();
        }
        EXPECT_TRUE(pq1.empty() && pq2.empty());
    }

    {
        mystl::priority_queue<std::string> pq;
        EXPECT_TRUE(pq.empty());

        pq.push("zxh");
        EXPECT_TRUE(!pq.empty());
    }

    {
        mystl::priority_queue<int> pq;
        auto i = 1;
        for (; i != 10; ++i) {
            pq.push(i);
            EXPECT_EQ(pq.size(), i);
        }
        for (i = pq.size(); i != 0; --i) {
            pq.pop();
            EXPECT_EQ(pq.size(), (i - 1));
        }
    }

    {
        std::priority_queue<int> pq1;
        mystl::priority_queue<int> pq2;

        pq1.push(30);
        pq1.push(100);
        pq1.push(25);
        pq1.push(40);

        pq2.push(30);
        pq2.push(100);
        pq2.push(25);
        pq2.push(40);

        while (!pq1.empty() && !pq2.empty()) {
            EXPECT_EQ(pq1.top(), pq2.top());
            pq1.pop();
            pq2.pop();
        }
    }

    {
        mystl::priority_queue<int> foo, bar;
        foo.push(15);
        foo.push(30);
        foo.push(10);
        bar.push(101);
        bar.push(202);

        EXPECT_TRUE(foo.size() == 3 && bar.size() == 2);
        foo.swap(bar);
        EXPECT_TRUE(foo.size() == 2 && bar.size() == 3);

        mystl::swap(foo, bar);
        EXPECT_TRUE(foo.size() == 3 && bar.size() == 2);
    }
}
}
}
} // namespace mystl::test::queue_test
#endif // MYSTL_QUEUE_TEST_H_
