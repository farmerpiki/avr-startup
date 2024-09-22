#include <avr/io.h>
#include <util/delay.h>

// next define required to include avr/sleep.h
#define SM1 1 // NOLINT(cppcoreguidelines-macro-usage)
#include <avr/sleep.h>

import io;
import tinyarray;

namespace {
void deep_sleep() {
	sleep_cpu();
}
} // namespace

[[noreturn]] int main(void) {
	constexpr auto update_interval = 10.0;

	constexpr auto water_overflow_sensor{"PB0"_pin};
	constexpr auto rain_sensor{"PB2"_pin};
	constexpr auto light_sensor{"PB5"_pin};

	// TODO: setup sleep properly, and set up a wakeup source
	sleep_enable();

	constexpr tinyarray sensors{water_overflow_sensor, rain_sensor, light_sensor};

	for (auto const &i: sensors) {
		i.make_input_with_pullup();
		// wait a bit
		if (i.is_set()) {
			// do stuff
			deep_sleep(); // placeholder
		}
	}

	constexpr auto green_led{"PC4"_pin};
	green_led.make_output();
	green_led.set();

	constexpr auto red_led{"PA2"_pin};
	red_led.make_output();

	for (;;) {
		if (water_overflow_sensor.is_set()) {
			red_led.set();
		} else {
			red_led.clear();
		}
		_delay_ms(update_interval);
	}
}
