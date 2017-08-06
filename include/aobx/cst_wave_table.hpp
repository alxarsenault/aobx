#pragma once

#include <cstddef>
#include <cmath>
#include <type_traits>
#include <limits>
#include <array>
#include "aobx/cst_math.hpp"
#include <array>

namespace aobx {
namespace cst {
	template <typename T, std::size_t size>
	constexpr T sin_value_at(std::size_t i)
	{
		long double v = (long double)(2.0) * aobx::cst::pi<long double> * (long double)(i)
			* ((long double)1.0) / (long double)size;
		return aobx::cst::sin(v);
	}

	template <typename T, std::size_t... I>
	constexpr std::array<T, sizeof...(I)> create_sine_table(std::index_sequence<I...>)
	{
		constexpr std::size_t size = sizeof...(I);

		return std::array<T, size>{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
			sin_value_at<T, size>(I)...
#pragma clang diagnostic pop
		};
	}
	template <typename T, std::size_t N>
	constexpr std::array<T, N> create_sine_table()
	{
		return create_sine_table<T>(std::make_index_sequence<N>{});
	}

	/////

	template <typename T, std::size_t size, std::size_t K, std::size_t index>
	struct sum_getter {
		static constexpr T get(std::size_t i, std::array<T, K> coeff)
		{
			T value = aobx::cst::sin((long double)(2.0) * std::get<index>(coeff)
				* aobx::cst::pi<long double> * (long double)(i) * ((long double)1.0) / (long double)size);
			return value + sum_getter<T, size, K, index - 1>::get(i, coeff);
		}
	};

	template <typename T, std::size_t size, std::size_t K>
	struct sum_getter<T, size, K, 0> {
		static constexpr T get(std::size_t i, std::array<T, K> coeff)
		{
			return 0.0;
		}
	};

	template <typename T, std::size_t size, std::size_t K>
	constexpr T sin_sum_value_at(std::size_t i, std::array<T, K> coeff)
	{
		return sum_getter<T, size, K, K - 1>::get(i, coeff);
	}

	template <typename T, std::size_t K, std::size_t... I>
	constexpr std::array<T, sizeof...(I)> create_sine_sum_table(
		std::array<T, K> coeff, std::index_sequence<I...>)
	{
		constexpr std::size_t size = sizeof...(I);

		return std::array<T, size>{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
			sin_sum_value_at<T, size, K>(I, coeff)...
#pragma clang diagnostic pop
		};
	}
	template <typename T, std::size_t N, std::size_t K>
	constexpr std::array<T, N> create_sine_sum_table(std::array<T, K> coeff)
	{
		return create_sine_sum_table<T, K>(coeff, std::make_index_sequence<N>{});
	}
} // namespace cst
} // namespace aobx
