#pragma once

#include <cmath>

namespace aobx {
namespace math {
	constexpr float two_pi = 2.0f * M_PI;
	constexpr float one_over_two_pi = 1.0f / two_pi;

	template <typename T>
	inline T lineair_interpole(T y1, T y2, T mu)
	{
		return y1 + mu * (y2 - y1);
	}

	template <typename T>
	inline T logarithmic_interpole(T y1, T y2, T mu)
	{
		return pow(y2, mu) * pow(y1, (T)1.0 - mu);
	}
} // namespace math
} // namespace aobx
