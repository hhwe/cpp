/*
 * a simple unit test for cpp
 */
#ifndef UNIT_TEST_H_
#define UNIT_TEST_H_

#include <string>
#include <vector>

namespace unit_test {
class TestCase {
public:
    TestCase(const char* name) :
        caseName_(name) {
    }
    ~TestCase();

private:
    const char* caseName_;
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

    void Run() {
        for (auto)
        {
            /* code */
        }
        
    }

private:
    std::vector<TestCase*> testCases_;
};

} // namespace unit_test

#endif // UNIT_TEST_H_