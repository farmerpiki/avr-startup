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
