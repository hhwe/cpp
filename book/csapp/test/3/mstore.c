/*************************************************************************
    > File Name: mstore.c
    > Author: 冷倾殇
    > Mail: 1500428751@qq.com 
    > Created Time: Sun Sep 18 21:35:44 2022
 ************************************************************************/
#include <stdio.h>


long mult2(long, long);

void multstore(long x, long y, long *dest)
{
	long t = mult2(x, y);
	*dest = t;
}

