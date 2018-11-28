#include <stdio.h>
#include <time.h>

int main() {
	int i = 0;
	clock_t start = clock();
	while (i < 1000000) {
		i = i + 1;
		printf("%d\n", i);
	}
	printf("%f\n", (double)(clock()-start) * 1000000 / CLOCKS_PER_SEC);
	return 0;
}

