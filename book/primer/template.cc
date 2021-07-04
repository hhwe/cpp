#include "common.h"

template <typename T>
int compare(const T &v1, const T &v2)
{
    if (v1 < v2)
        return -1;
    // if (v1 > v2) // 
    if (v2 < v1)
        return 1;
    return 0;
}

template <typename T, class U>
calc(const T &, const &U);

// 非类型模板参数，模板实参必须是常量表达式
template <unsigned N, unsigned M>
int compare(const char (&p1)[n], const char (&p2)[M])
{
    return strcmp(p1, p2);
}
compare("hi", "mom");



