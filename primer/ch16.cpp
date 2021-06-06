#include "common.h"

// string s1("hi!");
// string s2;

// s2 = std::move(string("bye!"));
// s2 = std::move(s1);

int main(int argc, char const *argv[])
{
    string s1("hi!");
    string s2;

    s2 = std::move(string("bye!"));
    s2 = std::move(s1);
    return 0;
}
