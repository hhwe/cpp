/*
 * a simple unit test for cpp
 */
#ifndef HTEST_H_
#define HTEST_H_

#include <iostream>
#include <string>
#include <vector>

namespace htest {
class TestCase {
public:
    TestCase(const char* name) :
        name_(name) {
    }
    ~TestCase();

    virtual void Run() = 0;

    const std::string name_;
    int passedPredicateNum_{0};
    int failedPredicateNum_{0};
    bool result_{true};
};

class UnitTest {
public:
    static UnitTest* GetInstance() {
        static UnitTest instance;
        return &instance;
    }

    TestCase* RegisterTestCase(TestCase* testCase) {
        testCases_.emplace_back(testCase);
        return testCase;
    }

    int Run() {
        for (auto testCase : testCases_) {
            currentTestCase_ = testCase;
            std::cout << ">>>>[Run TestCase] " << testCase->name_ << std::endl;
            testCase->Run();
            if (testCase->result_) {
                passedTestCaseNum_++;
            } else {
                failedTestCaseNum_++;
            }
            std::cout << "[TestCase Result] Passed:" << testCase->passedPredicateNum_
                      << " Failed:" << testCase->failedPredicateNum_ << std::endl;
        }
        std::cout << "\n[UnitTest Result] Passed-TestCase:" << passedTestCaseNum_ << " Failed-TestCase:" << failedTestCaseNum_ << std::endl;
        return 0;
    }

    TestCase* currentTestCase_;

private:
    std::vector<TestCase*> testCases_;
    int passedTestCaseNum_{0};
    int failedTestCaseNum_{0};
};

// Expands to the name of the class that implements the given test.
#define TESTCASE_NAME_(testcase_name) testcase_name##_Test

// Helper macro for defining tests.
#define UNIT_TEST_(testcase_name)                                                                            \
    class TESTCASE_NAME_(testcase_name) : public htest::TestCase {                                           \
    public:                                                                                                  \
        TESTCASE_NAME_(testcase_name)                                                                        \
        (const char* name) : TestCase(name) {                                                                \
        }                                                                                                    \
        virtual void Run() override;                                                                         \
                                                                                                             \
    private:                                                                                                 \
        static const TestCase* const testCase_;                                                              \
    };                                                                                                       \
                                                                                                             \
    const htest::TestCase* const TESTCASE_NAME_(testcase_name)::testCase_ =                                  \
        htest::UnitTest::GetInstance()->RegisterTestCase(new TESTCASE_NAME_(testcase_name)(#testcase_name)); \
    void TESTCASE_NAME_(testcase_name)::Run()

// Declare a class for each test suit, and user-defined function in TEST will copy to Run()
#define TEST(test_case_name) UNIT_TEST_(test_case_name)

// Use this function in main() to run all tests.
int RUN_ALL_TESTS();
inline int RUN_ALL_TESTS() {
    return htest::UnitTest::GetInstance()->Run();
}

#define EXPECT_COMPARE_HELPER_(op_name, op, val1, val2)                                 \
    do {                                                                                \
        if ((val1)op(val2)) {                                                           \
            htest::UnitTest::GetInstance()->currentTestCase_->passedPredicateNum_++;    \
        } else {                                                                        \
            htest::UnitTest::GetInstance()->currentTestCase_->failedPredicateNum_++;    \
            htest::UnitTest::GetInstance()->currentTestCase_->result_ = false;          \
            std::cout << __FILE__ << ":" << __LINE__ << " Failed!" << std::endl;        \
            std::cout << "\tExpect:" << (val1) << "\n\tActual:" << (val2) << std::endl; \
        }                                                                               \
    } while (0)

#define EXPECT_EQ(val1, val2) EXPECT_COMPARE_HELPER_(EQ, ==, (val1), (val2))
#define EXPECT_NE(val1, val2) EXPECT_COMPARE_HELPER_(NE, !=, (val1), (val2))
#define EXPECT_LE(val1, val2) EXPECT_COMPARE_HELPER_(LE, <=, (val1), (val2))
#define EXPECT_LT(val1, val2) EXPECT_COMPARE_HELPER_(LT, <, (val1), (val2))
#define EXPECT_GE(val1, val2) EXPECT_COMPARE_HELPER_(GE, >=, (val1), (val2))
#define EXPECT_GT(val1, val2) EXPECT_COMPARE_HELPER_(GT, >, (val1), (val2))

#define EXPECT_BOOL_HELPER_(op, val)                                                 \
    do {                                                                             \
        if (val) {                                                                   \
            htest::UnitTest::GetInstance()->currentTestCase_->passedPredicateNum_++; \
        } else {                                                                     \
            htest::UnitTest::GetInstance()->currentTestCase_->failedPredicateNum_++; \
            htest::UnitTest::GetInstance()->currentTestCase_->result_ = false;       \
            std::cout << __FILE__ << ":" << __LINE__ << " Failed!" << std::endl;     \
            std::cout << "\tExpect:" << (op) << "\n\tActual:" << (val) << std::endl; \
        }                                                                            \
    } while (0)

#define EXPECT_TRUE(val) EXPECT_BOOL_HELPER_(true, (val))
#define EXPECT_FALSE(val) EXPECT_BOOL_HELPER_(false, !(val))

} // namespace htest

#endif // HTEST_H_