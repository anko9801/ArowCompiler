#include <cstdlib>
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

	gpioInfo(unsigned long GpioBase);
	~gpioInfo();
};

gpioInfo::gpioInfo(unsigned long GpioBase) {
	gpio_base = GpioBase;
	memory_fd = open("/dev/mem", O_RDWR|O_SYNC);

	if(memory_fd < 0) {
		perror("Failed to open /dev/mem, try change permission.");
		std::exit(1);
	}

	map = mmap(
			NULL,
			BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			memory_fd,
			gpio_base
			);

	if(map == MAP_FAILED) {
		perror("mmap");
		std::exit(1);
	}

	addr = (volatile unsigned int *)map;
}

gpioInfo::~gpioInfo() {
	munmap(map, BLOCK_SIZE);
	close(memory_fd);
}

gpioInfo gpio(GPIO_BASE);

void gpioMode(int gpio_num, int mode) {
	// Read
	if (mode == 0) {
		*(gpio.addr + gpio_num / 10)  = 0b000 << (gpio_num % 10 * 3);
	// Write
	}else if (mode == 1) {
		*(gpio.addr + gpio_num / 10)  = 0b001 << (gpio_num % 10 * 3);
	}
}

void gpioWrite(int gpio_num, int mode) {
	// Low
	if (mode == 0) {
		*(gpio.addr + 10)  = 0b1 << gpio_num;
	// High
	}else if (mode == 1) {
		*(gpio.addr + 7) = 0b1 << gpio_num;
	}
}

void gpioClear(int gpio_num) {
	*(gpio.addr + gpio_num / 10)  = 0x00000000;
}

