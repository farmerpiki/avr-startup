module;

#include <avr/io.h>
#include <stdint.h>

export module io;
import tinyarray;

export constexpr uint8_t ports_size = 6;
VPORT_t *const ioport[ports_size]{&VPORTA, &VPORTB, &VPORTC, &VPORTD, &VPORTE, &VPORTF};
export struct [[gnu::packed]] Pin {
	VPORT_t *port;
	uint8_t mask;

	inline Pin(uint8_t port_index, uint8_t pin_nr)
		: port{ioport[port_index]}
		, mask(1 << pin_nr) {}
};

template<uint8_t N> constexpr tinyarray<uint8_t, ports_size> calculate_ioport_masks(tinyarray<Pin, N> pins) {
	tinyarray<uint8_t, ports_size> masks{};
	for (auto pin: pins) {
		if (pin.port < ports_size) {
			masks[pin.port] |= (1 << pin.pin_nr);
		}
	}
	return masks;
}

export template<uint8_t N> void setup_as_output(tinyarray<Pin, N> pins) {
	constexpr auto masks = calculate_ioport_masks(pins);
	for (uint8_t port = 0; auto mask: masks) {
		ioport[port]->DIR |= mask;
		++port;
	}
}
export template<uint8_t N> void setup_as_input(tinyarray<Pin, N> pins) {
	constexpr auto masks = calculate_ioport_masks(pins);
	for (uint8_t port = 0; auto mask: masks) {
		ioport[port]->DIR &= ~mask;
		++port;
	}
}
export template<uint8_t N> void setup_as_pullup_input(tinyarray<Pin, N> pins) {
	constexpr auto masks = calculate_ioport_masks(pins);
	for (uint8_t port = 0; auto mask: masks) {
		//		ioport[port]->pullup |= mask;
		ioport[port]->DIR &= ~mask;
		++port;
	}
}

export inline void set_pin(Pin pin) {
	pin.port->OUT |= pin.mask;
}

export inline bool read_pin(Pin pin) {
	return (pin.port->IN & pin.mask) == pin.mask;
}
