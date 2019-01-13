#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define BLOCK_SIZE      4 * 1024
#define PERIPHERAL_BASE 0x3F000000
#define GPIO_BASE       PERIPHERAL_BASE + 0x00200000

struct gpioInfo {
	unsigned long         gpio_base;
	int                   memory_fd;
	void                  *map;
	volatile unsigned int *addr;
};

struct gpioInfo g;

void confirm() {
	g.gpio_base = GPIO_BASE;
	g.memory_fd = open("/dev/mem", O_RDWR|O_SYNC);

	if (g.memory_fd < 0) {
		perror("Failed to open /dev/mem, try change permission.");
		return ;
	}

	g.map = mmap(
			NULL,
			BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			g.memory_fd,
			g.gpio_base
			);

	if (g.map == MAP_FAILED) {
		perror("mmap");
		return ;
	}

	g.addr = (volatile unsigned int *)g.map;
}

void Close() {
	munmap(g.map, BLOCK_SIZE);
	close(g.memory_fd);
	return ;
}

void gpioMode(int gpio_num, int mode) {
	// Read
	if (mode == 0) {
		*(g.addr + gpio_num / 10)  = 0b000 << (gpio_num % 10 * 3);
	// Write
	}else if (mode == 1) {
		*(g.addr + gpio_num / 10)  = 0b001 << (gpio_num % 10 * 3);
	}
}

void gpioWrite(int gpio_num, int mode) {
	// Low
	if (mode == 0) {
		*(g.addr + 10)  = 0b1 << gpio_num;
	// High
	}else if (mode == 1) {
		*(g.addr + 7) = 0b1 << gpio_num;
	}
}

void gpioClear(int gpio_num) {
	*(g.addr + gpio_num / 10)  = 0x00000000;
}

