#pragma once

#include <cstddef>
#include "aobx/tuple_utils.hpp"
#include <array>

namespace aobx {
template <typename T, std::size_t N>
class value {
public:
	template <typename... Args>
	value(Args&... args)
		: _data{ &args... }
	{
		/// @todo add static assert with args.. size.
	}

	value(const value& v)
		: _data(v._data)
	{
	}

	inline T& operator[](std::size_t index)
	{
		return *_data[index];
	}

private:
	std::array<T*, N> _data;
};

template <typename T>
using stereo_value = value<T, 2>;

template <typename T>
using mono_value = value<T, 1>;
} // namespace aobx
