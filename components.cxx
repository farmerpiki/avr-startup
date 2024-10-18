module;

#include <util/delay.h>

export module components;

import io;

class motorized_valve {
public:
	inline consteval explicit motorized_valve(pin_t open_pin, pin_t close_pin) noexcept
		: open_pin{open_pin}
		, close_pin{close_pin} {
		open_pin.clear();
		open_pin.make_output();
		close_pin.clear();
		close_pin.make_output();
	}

	void open() const noexcept {
		close_pin.clear();
		_delay_ms(1);
		open_pin.set();
	}

	void stop() const noexcept {
		close_pin.clear();
		open_pin.clear();
	}

	void close() const noexcept {
		open_pin.clear();
		_delay_ms(1);
		close_pin.set();
	}

private:
	pin_t open_pin;
	pin_t close_pin;
};
