/*************************************************************************
    > File Name: main.c
    > Author: 冷倾殇
    > Mail: 1500428751@qq.com 
    > Created Time: Sun Sep 18 22:17:38 2022
 ************************************************************************/

#include <stdio.h>

void multstore(long, long, long*);

int main()
{
	long d;
	multstore(2, 3, &d);
	printf("2 * 3 --> %ld\n", d);
	return 0;
}
long mult2(long a, long b) {
	long s = a * b;
	return s;
}

long vframe(long n, long idx, long *q) {
	long i;
	long *p[n];
	p[0];
	for (i = 0; i < n; i++) {
		p[i] = q;
	}
	return *p[idx];
}
