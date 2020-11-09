#include "common.h"

class Numbered
{
private:
public:
    int mysn;
    static int unique;

    Numbered();
    Numbered(const Numbered &);
    Numbered &operator=(const Numbered &) = delete;

    ~Numbered() = default;
};

int Numbered::unique = 10;

Numbered::Numbered()
{
    cout << "init constructor" << endl;
    mysn = unique++;
}

Numbered::Numbered(const Numbered &)
{
    cout << "systhesized copy constructor" << endl;
    mysn = unique++;
}

// Numbered &Numbered::operator=(const Numbered &rhs)
// {
//     cout << "systhesized copy-asignment operator" << endl;
//     mysn = rhs.mysn;
//     return *this;
// }

void f(Numbered s)
{
    cout << s.mysn << endl;
}

void f1(Numbered &s)
{
    cout << s.mysn << endl;
}

class Employee
{
private:
    string name_;
    int id_;
    static int unique;

public:
    Employee() = default;
    Employee(string name);
    ~Employee();
};

int Employee::unique = 50;

Employee::Employee(string name)
{
    name_ = name;
    id_ = unique++;
}

Employee::~Employee()
{
}

int main(int argc, char const *argv[])
{
    Numbered a, b = a, c = a;
    f1(a);
    f1(b);
    f1(c);
    f(a);
    f(b);
    f(c);
    f1(a);
    f1(b);
    f1(c);


    return 0;
}
