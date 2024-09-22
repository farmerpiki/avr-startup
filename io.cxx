module;

#include <assert.h>
#include <avr/io.h>
#include <stdint.h>

export module io;
import tinyarray;

export struct pin_t {
public:
	pin_t(pin_t const &rhs) = default;
	pin_t(pin_t &&rhs) = delete;
	pin_t &operator=(pin_t const &rhs) = delete;
	pin_t &operator=(pin_t &&rhs) = delete;
	~pin_t() = default;

	inline void make_output() const noexcept { vport().DIR |= mask(); }
	inline void make_input() const noexcept { vport().DIR &= ~mask(); }

	inline void make_input_with_pullup() const noexcept {
		vport().DIR &= ~mask();
		pin_ctrl() |= PORT_PULLUPEN_bm;
	}

	inline void set() const noexcept { vport().OUT |= mask(); }
	inline void set_value(bool on) const noexcept {
		if (on) {
			set();
		} else {
			clear();
		}
	}
	inline void clear() const noexcept { vport().OUT &= ~mask(); }
	inline void toggle() const noexcept { vport().OUT ^= mask(); }

	[[nodiscard]] inline operator bool() const noexcept { return is_set(); }
	[[nodiscard]] inline bool is_set() const noexcept { return (vport().IN | mask()) == mask(); }
	[[nodiscard]] inline bool is_clear() const noexcept { return (vport().IN & mask()) == 0; }

	[[nodiscard]] constexpr uint8_t mask() const noexcept { return masks[pin_index]; }
	[[nodiscard]] constexpr uint8_t get_port_index() const noexcept { return port_index; }
	[[nodiscard]] constexpr uint8_t get_pin_index() const noexcept { return pin_index; }

private:
	constexpr explicit pin_t(uint8_t port_index, uint8_t pin_index) noexcept
		: port_index{port_index}
		, pin_index{pin_index} {}

	// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
	static constexpr auto masks =
		tinyarray<uint8_t, 8>{(1 << 0), (1 << 1), (1 << 2), (1 << 3), (1 << 4), (1 << 5), (1 << 6), (1 << 7)};
	// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

	[[nodiscard]] constexpr VPORT_t &vport() const noexcept {
		return *(port_index + &VPORTA); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	}
	[[nodiscard]] constexpr PORT_t &port() const noexcept {
		return *(port_index + &PORTA); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	}
	[[nodiscard]] constexpr register8_t &pin_ctrl() const noexcept {
		return *(&(port().PIN0CTRL) + pin_index); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	}

	uint8_t port_index;
	uint8_t pin_index;
	friend consteval pin_t operator""_pin(char const *PIN, size_t length) noexcept;
};

export consteval pin_t operator""_pin(char const *pin, [[maybe_unused]] size_t length) noexcept {
	assert(length == 3);
	// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	assert(pin[0] == 'P');
	assert(pin[1] >= 'A');
	assert(pin[1] <= 'F');
	assert(pin[2] >= '0');
	assert(pin[2] <= '7');
	return pin_t(pin[1] - 'A', pin[2] - '0');
	// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

export constexpr uint8_t ports_size = 6;

template<uint8_t N> inline constexpr tinyarray<uint8_t, ports_size> calculate_ioport_masks(tinyarray<pin_t, N> pins) {
	tinyarray<uint8_t, ports_size> masks{};
	for (auto pin: pins) {
		if (pin.get_port_index() < ports_size) {
			masks[pin.get_port_index()] |= pin.mask();
		}
	}
	return masks;
}

export template<uint8_t N> inline void setup_as_output(tinyarray<pin_t, N> pins) {
	constexpr auto masks = calculate_ioport_masks(pins);
	for (uint8_t port = 0; auto mask: masks) {
		(*(port + &VPORTA)).DIR |= mask; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		++port;
	}
}

export template<uint8_t N> inline void setup_as_input(tinyarray<pin_t, N> pins) {
	constexpr auto masks = calculate_ioport_masks(pins);
	for (uint8_t port = 0; auto mask: masks) {
		(*(port + &VPORTA)).DIR &= ~mask; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		++port;
	}
}

export template<uint8_t N> inline void setup_as_pullup_input(tinyarray<pin_t, N> pins) {
	auto const masks = calculate_ioport_masks(pins);
	for (auto &pin: pins) {
		//(*(pin.get_port_index() + &PORTA)).PIN0CTRL + pin.get_pin_index() |=
		//	PORT_PULLUPEN_bm; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		(*(pin.get_port_index() + &PORTA)).PIN0CTRL |=
			PORT_PULLUPEN_bm; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
	}
	for (uint8_t port = 0; auto mask: masks) {
		(*(port + &VPORTA)).DIR &= ~mask; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
		++port;
	}
}

export inline void set_pin(pin_t pin) {
	pin.set();
}

export inline void clear_pin(pin_t pin) {
	pin.clear();
}

export inline bool read_pin(pin_t pin) {
	return pin.is_set();
}
