#include <stdio.h>
#include <unistd.h>

void loop(int n) {
    int m = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            usleep(1);
            m++;
        }
    }
}

void fun2() {
    return;
}

void fun1() {
    fun2();
}

int main() {
    loop(1000);

    // fun1callfun2
    fun1();

    return 0;
}