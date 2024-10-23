module;

#include <assert.h>
#include <avr/io.h>
#include <stdint.h>

export module io;
import tinyarray;

[[nodiscard]] constexpr uint8_t bitwise_not(uint8_t data) {
	return ~data;
}

constexpr uint8_t max_port_index = 6;

[[nodiscard]] inline constexpr VPORT_t &vport(uint8_t port_index) noexcept {
	assert(port_index < max_port_index);
	// NOLINTNEXTLINE(clang-analyzer-core.uninitialized.UndefReturn)
	return *(port_index + &VPORTA); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

[[nodiscard]] inline constexpr PORT_t &port(uint8_t port_index) noexcept {
	assert(port_index < max_port_index);
	return *(port_index + &PORTA); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

export struct pin_t {
public:
	consteval pin_t(pin_t const &rhs) noexcept = default;
	pin_t(pin_t &&rhs) = delete;
	pin_t &operator=(pin_t const &rhs) = delete;
	pin_t &operator=(pin_t &&rhs) = delete;
	~pin_t() noexcept = default;

	inline void make_output() const noexcept { vport().DIR |= mask(); }
	inline void make_input() const noexcept { vport().DIR &= bitwise_not(mask()); }

	inline void make_input_with_pullup() const noexcept {
		vport().DIR &= bitwise_not(mask());
		pin_ctrl() |= static_cast<uint8_t>(PORT_PULLUPEN_bm);
	}

	inline void set() const noexcept { vport().OUT |= mask(); }
	inline void set_value(bool on) const noexcept {
		if (on) {
			set();
		} else {
			clear();
		}
	}
	inline void clear() const noexcept { vport().OUT &= bitwise_not(mask()); }
	inline void toggle() const noexcept { vport().OUT ^= mask(); }

	[[nodiscard]] inline explicit operator bool() const noexcept { return is_set(); }
	[[nodiscard]] inline bool is_set() const noexcept { return (vport().IN | mask()) == mask(); }
	[[nodiscard]] inline bool is_clear() const noexcept { return (vport().IN & mask()) == 0; }

	[[nodiscard]] constexpr uint8_t mask() const noexcept { return masks[pin_index]; }
	[[nodiscard]] constexpr uint8_t get_port_index() const noexcept { return port_index; }
	[[nodiscard]] constexpr uint8_t get_pin_index() const noexcept { return pin_index; }

private:
	consteval explicit pin_t(uint8_t port_index, uint8_t pin_index) noexcept
		: port_index{port_index}
		, pin_index{pin_index} {}

	// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
	static constexpr auto masks = tinyarray<uint8_t, 8>{
		(1U << 0U),
		(1U << 1U),
		(1U << 2U),
		(1U << 3U),
		(1U << 4U),
		(1U << 5U),
		(1U << 6U),
		(1U << 7U)};
	// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)
	[[nodiscard]] inline constexpr VPORT_t &vport() const noexcept { return ::vport(port_index); }
	[[nodiscard]] inline constexpr PORT_t &port() const noexcept { return ::port(port_index); }

	[[nodiscard]] inline constexpr register8_t &pin_ctrl() const noexcept {
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

export class ioport_masks {
public:
	template<uint8_t N>
	[[gnu::always_inline]] inline consteval explicit ioport_masks(tinyarray<pin_t, N> pins) noexcept {
		for (auto pin: pins) {
			if (pin.get_port_index() < max_port_index) {
				masks[pin.get_port_index()] |= pin.mask();
			}
		}
	}

	[[nodiscard]] constexpr tinyarray<uint8_t, max_port_index> const &value() const noexcept { return masks; }

private:
	tinyarray<uint8_t, max_port_index> masks{};
};

export void mass_make_output(ioport_masks const &masks) {
	for (uint8_t port_index = 0; auto mask: masks.value()) {
		if (mask != 0U) {
			vport(port_index).DIR |= mask;
		}
		++port_index;
	}
}

export void mass_make_input(ioport_masks const &masks) {
	for (uint8_t port_index = 0; auto mask: masks.value()) {
		if (mask != 0U) {
			vport(port_index).DIR &= bitwise_not(mask);
		}
		++port_index;
	}
}

export void mass_make_input_with_pullup(ioport_masks const &masks) {
	for (uint8_t port_index = 0; auto mask: masks.value()) {
		if (mask != 0U) {
			port(port_index).PINCONFIG = PORT_PULLUPEN_bm;
			port(port_index).PINCTRLSET = mask;
			vport(port_index).DIR &= bitwise_not(mask);
		}
		++port_index;
	}
}
