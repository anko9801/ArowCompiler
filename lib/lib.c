#include <stdio.h>
#include <unistd.h>

int msleep(int ms) {
	usleep(10);
	return 0;
}

int printnum(int i){
    return printf("%d\n",i);
}

