#include <iostream>
#include <vector>
#include <string>

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



    // A a;
    // A *a  = new A;
    // B b;
    // B *b = new B;
}
