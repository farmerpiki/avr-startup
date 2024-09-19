module;

#include <stdint.h>

export module tinyarray;

export template<typename T, uint8_t N> class tinyarray {
public:
	constexpr tinyarray() noexcept = default;
	constexpr tinyarray(T const (&init)[N]) noexcept {
		for (uint8_t i = 0; i < N; ++i) {
			m_data[i] = init[i];
		}
	}
	constexpr T operator[](uint8_t index) const noexcept { return m_data[index]; }
	constexpr T &operator[](uint8_t index) noexcept { return m_data[index]; }
	uint8_t size() const noexcept { return N; }

	constexpr T const *begin() const noexcept { return m_data; }
	constexpr T const *end() const noexcept { return m_data + N; }
	constexpr T *begin() noexcept { return m_data; }
	constexpr T *end() noexcept { return m_data + N; }

private:
	T m_data[N];
};
