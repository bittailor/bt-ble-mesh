#include <zephyr.h>
#include <sys/printk.h>

#define SLEEP_TIME_MS 1000

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
	while (1) {
		printk("hanna at %" PRId64 "\n", k_uptime_get());
		k_msleep(SLEEP_TIME_MS);
	}
}
