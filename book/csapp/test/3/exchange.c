/*************************************************************************
    > File Name: exchange.c
    > Author: 冷倾殇
    > Mail: 1500428751@qq.com 
    > Created Time: Mon Sep 19 23:26:24 2022
 ************************************************************************/
#include <stdio.h>

long exchange(long *xp, long y)
{
	long x = *xp;
	*xp = y;
	return x;
}


int mian()
{
	long a = 4;
	long b = exchange(&a, 3);
	printf("a = %ld, b = %ld\n", a, b);
	return 0;
}
