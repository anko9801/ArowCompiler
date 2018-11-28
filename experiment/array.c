#include<stdio.h>

int main() {
	int a[3] = {12,232,3434};
	int b = 0;
	a[1] = 34;
	printf("%d", a[1]);
	a[2] = 2;
	return a[2];
}
