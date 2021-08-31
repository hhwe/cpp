#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;

constexpr int ASIZE = 500000;

namespace testStl
{

    long get_a_target_long()
    {
        long target = 0;
        cout << "target (0~" << RAND_MAX << "): ";
        cin >> target;
        return target;
    }

    string get_a_target_string()
    {
        string target = 0;
        char buf[10];
        cout << "target (0~" << RAND_MAX << "): ";
        cin >> target;
        snprintf(buf, 10, "%s", target.c_str());
        return string(buf);
    }

    int CompareLongs(const void *a, const void *b)
    {
        return (*(long *)a) - (*(long *)b);
    }

    int CompareStrings(const void *a, const void *b)
    {
        return (*(string *)a) > (*(string *)b);
    }

    void test_array()
    {
        cout << "\ntest_array()... ... \n";
        array<long, ASIZE> arr;
        clock_t timeStart = clock();
        for (long i = 0; i < ASIZE; i++)
        {
            arr[i] = rand();
        }
        cout << "milli-seconds: " << (clock() - timeStart) << endl;
        cout << "array.size(): " << arr.size() << endl;
        cout << "array.front(): " << arr.front() << endl;
        cout << "array.back(): " << arr.back() << endl;
        cout << "array.data(): " << arr.data() << endl;

        long target = get_a_target_long();
        timeStart = clock();
        qsort(arr.data(), ASIZE, sizeof(long), CompareLongs);
        long *pItem = (long *)bsearch(&target, (arr.data()), ASIZE, sizeof(long), CompareLongs);
        cout << "qsort() + bsearch*(), milli-senconds: " << (clock() - timeStart) << endl;
        if (pItem != NULL)
        {
            cout << "found, " << *pItem << endl;
        }
        else
        {
            cout << "not found, " << endl;
        }
    }

} // namespace testStl

int main(int argc, char const *argv[])
{
    int ia[6] = {27, 1, 4, 454, 6, 443};
    vector<int, allocator<int>> vi(ia, ia + 6);

    cout << count_if(vi.begin(), vi.end(),
                     not1(bind2nd(less<int>(), 50)));

    testStl::test_array();

    return 0;
}