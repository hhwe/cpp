#include <iostream>
#include <vector>

#ifndef COMPLEX_H
#define COMPLEX_H

class Complex
{
public:
    Complex(double r = 0, double i = 0) : re(r), im(i) {}

    Complex &operator+=(const Complex &); // return by reference

    double real() const
    {
        return re;
    }
    double imag() const
    {
        return im;
    }

    std::vector<int> vt;

    int func(const Complex &param) { return param.re + param.im; } // 相同class的各个objects互为friends（友元）

private:
    double re, im;
    friend Complex &__doapl(Complex *, const Complex &);
};

inline Complex &__doapl(Complex *ths, const Complex &r)
{
    ths->re += r.re;
    ths->im += r.im;

    return *ths;
}

#endif // COMPLEX_H