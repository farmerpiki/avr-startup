module;

#include <stddef.h>

export module nonstd;

template<typename T, typename U> struct same_as_impl {
	static constexpr bool value = false;
};

template<typename T> struct same_as_impl<T, T> {
	static constexpr bool value = true;
};

export template<typename T, typename U>
concept same_as = same_as_impl<T, U>::value;
