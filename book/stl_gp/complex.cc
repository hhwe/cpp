#include "complex.h"

#include <iostream>

using namespace std;
int main(int argc, char const *argv[])
{
    Complex c1(2, 1);
    cout << c1.real() << endl;
    cout << c1.imag() << endl;

    const Complex c2(2, 1);
    cout << c2.real() << endl; // error if real is not const
    cout << c2.imag() << endl; // error if imag is not const

    cout << c1.func(c2) << endl;
    return 0;
}
