#include <stdio.h>
#include <unistd.h>
#include <time.h>

int wait(int ms) {
	usleep(ms);
	return 0;
}

int printnum(int i) {
	return printf("%d\n", i);
}

int usclock() {
	return (double)clock() * 1000000 / CLOCKS_PER_SEC;
}
