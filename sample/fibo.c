#include <stdio.h>
#include <time.h>
#include <unistd.h>

int fiiboo(int x) {
	if (x == 0) {
		return 0;
	}else if (x == 1) {
		return 1;
	}else{
		return fiiboo(x - 1) + fiiboo(x - 2);
	}
}

int main(void) {
	printf("%d\n", getpid());
	int start = (double)clock() * 1000000 / CLOCKS_PER_SEC;

	printf("%d\n", fiiboo(45));

	int end = (double)clock() * 1000000 / CLOCKS_PER_SEC;
	printf("start: %d, end: %d, result: %d\n", start, end, end - start);
	return 0;
}



