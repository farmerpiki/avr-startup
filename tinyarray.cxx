module;

#include <assert.h>
#include <stdint.h>

export module tinyarray;
import nonstd;

export template<typename T, uint8_t N> struct tinyarray {
	typedef T value_type;
	using pointer = value_type *;
	using const_pointer = value_type const *;
	using reference = value_type &;
	using const_reference = value_type const &;
	using iterator = value_type *;
	using const_iterator = value_type const *;
	using size_type = uint8_t;

	T m_data[N];

	[[nodiscard]] inline constexpr iterator begin() noexcept { return iterator(data()); }
	[[nodiscard]] inline constexpr const_iterator begin() const noexcept { return const_iterator(data()); }
	[[nodiscard]] inline constexpr iterator end() noexcept { return iterator(data() + N); }
	[[nodiscard]] inline constexpr const_iterator end() const noexcept { return const_iterator(data() + N); }
	[[nodiscard]] inline constexpr const_iterator cbegin() const noexcept { return const_iterator(data()); }
	[[nodiscard]] inline constexpr const_iterator cend() const noexcept { return const_iterator(data() + N); }

	[[nodiscard]] inline constexpr size_type size() const noexcept { return N; }
	[[nodiscard]] inline constexpr bool is_empty() const noexcept { return size() == 0; }

	[[nodiscard]] constexpr reference operator[](size_type n) noexcept {
		if consteval {
			assert(n < this->size());
		}
		return m_data[n];
	}

	[[nodiscard]] constexpr const_reference operator[](size_type n) const noexcept {
		if consteval {
			assert(n < this->size());
		}
		return m_data[n];
	}

	[[nodiscard]] constexpr reference front() noexcept {
		if consteval {
			assert(!this->is_empty());
		}
		return m_data[(size_type)0];
	}

	[[nodiscard]] constexpr const_reference front() const noexcept {
		if consteval {
			assert(!this->is_empty());
		}
		return m_data[(size_type)0];
	}

	[[nodiscard]] inline constexpr reference back() noexcept {
		if consteval {
			assert(!this->is_empty());
		}
		return m_data[N - 1];
	}

	[[nodiscard]] inline constexpr const_reference back() const noexcept {
		if consteval {
			assert(!this->is_empty());
		}
		return m_data[N - 1];
	}

	[[nodiscard]] inline constexpr pointer data() noexcept { return static_cast<pointer>(m_data); }

	[[nodiscard]] constexpr const_pointer data() const noexcept { return static_cast<const_pointer>(m_data); }
};

export template<typename T>
tinyarray(T const &t, same_as<T> auto const &...args) -> tinyarray<T, static_cast<uint8_t>(sizeof...(args) + 1)>;
