#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

#define INF 0x7fffffffffffffff
#define MAX 0x7ff0000000000000
#define POS_INFINITY (double)INF
#define NEG_INFINITY -INF
#define NEG_ZERO (-1.0 / INF)

typedef unsigned char* byte_pointer;

void show_bytes(byte_pointer start, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++) {
		printf(" %.2x", start[i]);
	}
	printf("\n");
}

void show_int(int x) 
{
	show_bytes((byte_pointer)&x, sizeof(int));
}

void show_float(float x) 
{
	show_bytes((byte_pointer)&x, sizeof(float));
}

void show_pointer(void* x)
{
	show_bytes((byte_pointer)&x, sizeof(void *));
}

void test_show_bytes(int val)
{
	int ival = val;
	float fval = (float)ival;
	int *pval = &ival;
	show_int(ival);
	show_float(fval);
	show_pointer(pval);
}

void int_float(void)
{
	long long ll = INF;
	double db = INF;

	printf("%lld, %lf\n", ll, db);

	long long *p1 = (long long*)&ll;
	double *p2 = (double *)&ll;
	
	printf("%lld, %lf\n", *p1, *p2);
}

int main(int argc, char** argv) 
{
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}


	int val = atoi(argv[1]);
	test_show_bytes(val);

	short x = 12345;
	short mx = -x;
	show_bytes((byte_pointer)&x, sizeof(short));
	show_bytes((byte_pointer)&mx, sizeof(short));

    printf("isnan(0.0)      : %d\n", isnan(0.0));
    printf("isnan(1.0/0.0)      : %d\n", isnan(1.0/0.0));
    printf("isnan(-1.0/0.0)      : %d\n", isnan(1.0/0.0));

    printf("nan:%lf, -nan:%lf, -0:%lf\n", POS_INFINITY, NEG_INFINITY, NEG_ZERO);
    int_float();
    return 0;
}

