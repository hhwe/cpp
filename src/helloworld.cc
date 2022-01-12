#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <deque>
#include <queue>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <hashtable>
#include <rbtree>
using namespace std;

// class A
// {
// public:
//     A() {}
//     void destory() { delete this; }

// private:
//     ~A() {}
// };

// class B
// {
// private:
//     void* operator new(size_t t){}     // 注意函数的第一个参数和返回值都是固定的
//     void operator delete(void* ptr){} // 重载了new就需要重载delete
// public:
//     B(){}
//     ~B(){}
// };

int main()
{
    vector<string> msg{"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};

    for (const string &word : msg)
    {
        cout << word << " ";
    }
    cout << endl;
    list<int> a;
    list<int>::iterator at = a.begin();
    at++;
    cout << sizeof(a) << endl;
    vector<int> b;
    vector<int>::iterator bt = b.begin();
    cout << sizeof(b) << endl;

    deque<int> d;
    queue<int> q;

    set<int> s;
    map<int, string> m;

    unordered_map<int, string> um;

    cout << vector<int>::iterator().base() << endl;
    

    // A a;
    // A *a  = new A;
    // B b;
    // B *b = new B;
}
