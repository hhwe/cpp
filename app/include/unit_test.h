/*
 * a simple unit test for cpp
 */
#ifndef UNIT_TEST_H_
#define UNIT_TEST_H_

#include <iostream>
#include <string>
#include <vector>

namespace unit_test {
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
            std::cout << "[Run TestCase] " << testCase->name_ << std::endl;
            testCase->Run();
            if (testCase->result_) {
                passedTestCaseNum_++;
            } else {
                failedTestCaseNum_++;
            }
        }
        std::cout << "[UnitTest]\tPassed:" << passedTestCaseNum_ << "\tFailed:" << failedTestCaseNum_ << std::endl;
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
#define UNIT_TEST_(testcase_name)                                                                     \
    class TESTCASE_NAME_(testcase_name) : public TestCase {                                           \
    public:                                                                                           \
        TESTCASE_NAME_(testcase_name)                                                                 \
        (const char* name) : TestCase(name) {                                                         \
        }                                                                                             \
        virtual void Run() override;                                                                  \
                                                                                                      \
    private:                                                                                          \
        static const TestCase* const testCase_;                                                       \
    };                                                                                                \
                                                                                                      \
    const TestCase* const TESTCASE_NAME_(testcase_name)::testCase_ =                                  \
        UnitTest::GetInstance()->RegisterTestCase(new TESTCASE_NAME_(testcase_name)(#testcase_name)); \
    void TESTCASE_NAME_(testcase_name)::Run()

// Declare a class for each test suit, and user-defined function in TEST will copy to Run()
#define TEST(test_case_name) UNIT_TEST_(test_case_name)

// Use this function in main() to run all tests.
int RUN_ALL_TESTS();
inline int RUN_ALL_TESTS() {
    return UnitTest::GetInstance()->Run();
}

#define EXPECT_HELPER_(op_name, op, val1, val2)                                     \
    do {                                                                            \
        if (val1 op val2) {                                                         \
            UnitTest::GetInstance()->currentTestCase_->passedPredicateNum_++;       \
        } else {                                                                    \
            UnitTest::GetInstance()->currentTestCase_->failedPredicateNum_++;       \
            UnitTest::GetInstance()->currentTestCase_->result_ = false;             \
            std::cout << __FILE__ << ":" << __LINE__ << " Failed!" << std::endl;    \
            std::cout << "\tExpect:" << val1 << "\n\tActual:" << val2 << std::endl; \
        }                                                                           \
    } while (0)

#define EXPECT_EQ(val1, val2) EXPECT_HELPER_(EQ, ==, val1, val2)
#define EXPECT_NE(val1, val2) EXPECT_HELPER_(NE, !=, val1, val2)
#define EXPECT_LE(val1, val2) EXPECT_HELPER_(LE, <=, val1, val2)
#define EXPECT_LT(val1, val2) EXPECT_HELPER_(LT, <, val1, val2)
#define EXPECT_GE(val1, val2) EXPECT_HELPER_(GE, >=, val1, val2)
#define EXPECT_GT(val1, val2) EXPECT_HELPER_(GT, >, val1, val2)

} // namespace unit_test

#endif // UNIT_TEST_H_