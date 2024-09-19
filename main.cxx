#include <avr/io.h>

#define SM1 1
#include <avr/sleep.h>

#include <stdint.h>

import io;
import tinyarray;

void deep_sleep() {
	sleep_cpu();
}

[[noreturn]] [[gnu::OS_task]] int main(void) {
	tinyarray<uint8_t, 3> r{
		{2, 6, 9}
	 };
	for (auto i: r) {
		auto _ = i;
	}

	Pin pin{1, 2};

	sleep_enable();

	while (!read_pin(pin)) {
		deep_sleep();
		set_pin(pin);
	}

	for (;;) {
		deep_sleep();
	}
}
