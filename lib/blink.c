#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define BLOCK_SIZE      4 * 1024
#define PERIPHERAL_BASE 0x3F000000
#define GPIO_BASE       PERIPHERAL_BASE + 0x00200000

// メモリの配置
typedef struct {
	unsigned long         gpio_base;
	int                   memory_fd;
	void                  *map;
	volatile unsigned int *addr;
} rpi_gpio;

// 初期化の仕方
rpi_gpio gpio = {GPIO_BASE};

int MapGPIO() {
	gpio.memory_fd = open("/dev/mem", O_RDWR|O_SYNC);

	if(gpio.memory_fd < 0) {
		perror("Failed to open /dev/mem, try change permission.");
		return 1;
	}
	printf("void:%x\n", sizeof(void*));
	printf("rpi:%x\n", sizeof(rpi_gpio));
	printf("base:%x\n", gpio.gpio_base);
	printf("memory:%x\n", gpio.memory_fd);

	gpio.map = mmap(
			NULL,
			BLOCK_SIZE,
			PROT_READ|PROT_WRITE,
			MAP_SHARED,
			gpio.memory_fd,
			gpio.gpio_base
			);

	if(gpio.map == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	gpio.addr = (volatile unsigned int *)gpio.map;
	return 0;
}

void GPIOclear() {
	munmap(gpio.map, BLOCK_SIZE);
	close(gpio.memory_fd);
}

void BlinkLED(int gpio_num) {
	int n = 5;

	*(gpio.addr + gpio_num / 10)  = 0b001 << (gpio_num % 10 * 3);

	while(n -= 1) {
		*(gpio.addr + 7)  = 0b1 << gpio_num;
		usleep(500 * 1000);
		*(gpio.addr + 10) = 0b1 << gpio_num;
		usleep(500 * 1000);
	}

	*(gpio.addr + gpio_num / 10)  = 0x00000000;
}

int GPIOsetup() {
	int map_status;

	map_status = MapGPIO();
	if(map_status) {
		printf("Failed to blink LED.\n");
		return map_status;
	}
}

int main() {
	GPIOsetup();
	BlinkLED(23);
	GPIOclear();
	return 0;
}

