#pragma once

#include <cstddef>
#include <cmath>
#include <type_traits>
#include <limits>
#include <array>

namespace aobx {
namespace cst {
	struct pair {
		float left, right;
	};

	//
	// Constant.
	//

	template <typename T>
	constexpr T pi = T(3.141592653589793);

	template <typename T>
	constexpr T two_pi = 2.0 * pi<T>;

	template <typename T>
	constexpr T one_over_two_pi = 1.0 / two_pi<T>;

	template <typename T>
	constexpr T pi_over_two = pi<T> * 0.5;

	template <typename T>
	constexpr T tolerance = 0.0001;

	template <typename T>
	constexpr T epsilon = tolerance<T>;

	template <>
	constexpr float epsilon<float> = std::numeric_limits<float>::epsilon();

	template <>
	constexpr double epsilon<double> = std::numeric_limits<double>::epsilon();

	//
	// Expr.
	//
	template <typename T>
	constexpr bool is_nan(T x)
	{
		return x != x;
	}

	template <typename T>
	constexpr bool is_inf(T x)
	{
		return (x + (T)1.0) == x;
	}

	constexpr float fma(double a, double b, double c)
	{
		return ((long double)a) * ((long double)b) + ((long double)c);
	}

	// Composes a floating point value with the magnitude of x and the sign of y.
	/// @warning Doesn't work with -0.0 sign.
	template <typename T>
	constexpr T copy_sign(T x, T y)
	{
		if (x == (T)0.0) {
			return x;
		}
		// Same sign.
		else if ((y < (T)0.0 && x < (T)0.0) || (y > (T)0.0 && x > (T)0.0)) {
			return x;
		}
		// Different sign.
		return -x;
	}

	template <typename T>
	constexpr int floor(T x)
	{
		return x >= (T)0.0 ? (int)x : x - (int)x < 0 ? (int)x - 1 : (int)x;
	}

	template <typename T>
	constexpr T abs(T x)
	{
		return x < (T)0.0 ? -x : x;
	}

	constexpr int get_exponent(float x)
	{
		return abs<float>(x) >= 2 ? get_exponent(x / 2) + 1 : abs<float>(x) < 1 ? get_exponent(x * 2) - 1 : 0;
	}

	template <typename T>
	constexpr T square(T x)
	{
		return x * x;
	}

	template <typename T>
	constexpr T cube(T x)
	{
		return x * x * x;
	}

	constexpr std::size_t digit_count(int number)
	{
		if (number == 0) {
			return 1;
		}

		int abs_number = number > 0 ? number : -number;

		double mu = 10.0;
		std::size_t i = 0;

		for (; ((double)abs_number / mu) >= 1.0; i++) {
			mu *= 10.0;
		}

		return i + 1;
	}

	constexpr float my_pow_2(int exponent)
	{
		long double v = 1.0;
		const int ex = abs<int>(exponent);

		if (exponent < 0) {
			for (long i = 0; i < ex; i++) {
				v /= 2.0;
			}
		} else {
			for (long i = 0; i < ex; i++) {
				v *= 2.0;
			}
		}

		return (float)v;
	}

	// Triple-angle formula: sin(3 * x) = 3 * sin(x) - 4 * sin^3(x)
	//	template <typename T>
	//	constexpr T sink(T x)
	//	{
	//		return abs<T>(x) < tolerance<T> ? x : (T)3.0 * (sin(x / (T)3.0)) - (T)4.0 * cube<T>(sin(x /
	//(T)3.0));
	//	}
	//
	template <typename T>
	constexpr T sin(T x)
	{
		int count = 0;
		long double x0 = x;
		while (abs<long double>(x0 / (long double)3.0) > (long double)0.000000000001) {
			x0 /= (long double)3.0;
			count++;
		}

		for (int i = 0; i < count; i++) {
			x0 = 3.0 * x0 - 4.0 * x0 * x0 * x0;
		}

		return x0;
	}

	template <typename T>
	constexpr T cos(T x)
	{
		return sin(pi_over_two<T> - x);
	}

	constexpr pair logf_ext(float a)
	{
		const float LOG2_HI = 0x1.62e430p-1f; //  6.93147182e-1
		const float LOG2_LO = -0x1.05c610p-29f; // -1.90465421e-9

		int e = get_exponent(a) + 1;
		float m = a / my_pow_2(e);

		if (m < 0.70703125f) { /* 181/256 */
			m = m + m;
			e = e - 1;
		}

		float i = (float)e;

		/* Compute q = (m-1)/(m+1) as a double-float qhi:qlo */
		float p = m + 1.0f;
		m = m - 1.0f;
		float r = 1.0f / p;
		float qhi = m * r;
		float t = fma(qhi, -2.0f, m);
		float s = fma(qhi, -m, t);
		float qlo = r * s;

		/* Approximate atanh(q), q in [-75/437, 53/309] */
		s = qhi * qhi;
		r = 0x1.06e000p-3f; // 1.28356934e-1
		r = fma(r, s, 0x1.22c318p-3f); // 1.41973674e-1
		r = fma(r, s, 0x1.99a29ep-3f); // 2.00017199e-1
		r = fma(r, s, 0x1.55554ep-2f); // 3.33333224e-1
		r = r * s;
		p = fma(r, qlo, qlo);
		s = fma(r, qhi, p);

		/* log(a) = 2 * atanh(q) + i * log(2) */
		t = fma(LOG2_HI * 0.5f, i, qhi);
		p = fma(-LOG2_HI * 0.5f, i, t);
		s = (qhi - p) + s;
		s = fma(LOG2_LO * 0.5f, i, s);
		r = t + t;
		t = fma(2.0f, s, r);
		return pair{ t, fma(2.0f, s, r - t) };
	}

	/* Compute exp(a). Maximum ulp error = 0.87161 */
	constexpr float my_expf_unchecked(float a)
	{
		// exp(a) = exp(i + f); i = rint (a / log(2))
		float r = fma(0x1.715476p0f, a, 0x1.8p23f) - 0x1.8p23f; // 1.442695, 12582912.0
		float f = fma(r, -0x1.62e400p-01f, a); // log_2_hi // -6.93145752e-1
		f = fma(r, -0x1.7f7d1cp-20f, f); // log_2_lo // -1.42860677e-6
		int i = (int)r;

		// Approximate r = exp(f) on interval [-log(2)/2,+log(2)/2]
		r = 0x1.6aa000p-10f; // 1.38330460e-3
		r = fma(r, f, 0x1.1272eep-07f); // 8.37551709e-3
		r = fma(r, f, 0x1.555a1cp-05f); // 4.16689441e-2
		r = fma(r, f, 0x1.55542ep-03f); // 1.66664466e-1
		r = fma(r, f, 0x1.fffff6p-02f); // 4.99999851e-1
		r = fma(r, f, 0x1.000000p+00f); // 1.00000000e+0
		r = fma(r, f, 0x1.000000p+00f); // 1.00000000e+0
		// exp(a) = 2**i * exp(f);
		// r = ldexpf (r, i);
		r = r * (float)my_pow_2(i);

		return r;
	}

	/* a**b = exp (b * log (a)), where a > 0, and log(a) is computed with extended
	 precision as a double-float. The maximum ulp error found so far is 5.49892
	 ulp for a = 1.41116774, b= -251.613571
	 */
	constexpr float powf_core(const pair a, float b)
	{
		float MAX_IEEE754_FLT = 0x1.fffffep127f;
		//		float EXP_OVFL_BOUND = 0x1.62e430p+6f;
		float EXP_OVFL_UNFL_F = 104.0f;

		/* compute lhi:llo = log(a) */
		float lhi = a.left;
		float llo = a.right;

		/* compute phi:plo = b * log(a) */
		float thi = lhi * b;

		float r = 0.0f;

		if (abs<float>(thi) > EXP_OVFL_UNFL_F) { // definitely overflow / underflow
			r = (thi < 0.0f) ? 0.0f : (1.0f / 0.0f);
		} else {
			float tlo = fma(lhi, b, -thi);
			tlo = fma(llo, b, +tlo);
			/* normalize intermediate result thi:tlo, giving final result phi:plo */

			float phi = thi + tlo;
			// avoid premature ovfl in exp() computation
			//			if (phi == EXP_OVFL_BOUND) {
			//				phi = nextafterf(phi, 0.0f);
			//			}

			float plo = (thi - phi) + tlo;
			/* exp'(x) = exp(x); exp(x+y) = exp(x) + exp(x) * y, for |y| << |x| */
			r = my_expf_unchecked(phi);

			/* prevent generation of NaN during interpolation due to r = INF */
			if (abs<float>(r) <= MAX_IEEE754_FLT) {
				r = fma(plo, r, r);
			}
		}
		return r;
	}

	constexpr float powf(float a, float b)
	{
		const int expo_odd_int = fma(-2.0f, floor(0.5f * b), b) == 1.0f;
		float r = 0.0f;

		/* special case handling per ISO C specification */
		if ((a == 1.0f) || (b == 0.0f)) {
			r = 1.0f;
		} else if (is_nan<float>(a) || is_nan<float>(b)) {
			r = a + b; // convert SNaN to QNanN or trigger exception
		} else if (is_inf<float>(b)) {
			r = ((abs<float>(a) < 1.0f) != (b < 0.0f)) ? 0.0f : (1.0f / 0.0f);
			if (a == -1.0f) {
				r = 1.0f;
			}
		} else if (is_inf<float>(a)) {
			r = (b < 0.0f) ? 0.0f : (1.0f / 0.0f);

			if ((a < 0.0f) && expo_odd_int) {
				r = -r;
			}
		} else if (a == 0.0f) {
			r = (expo_odd_int) ? (a + a) : 0.0f;
			if (b < 0.0f) {
				r = copy_sign<float>((1.0f / 0.0f), r);
			}
		} else if ((a < 0.0f) && (b != floor(b))) {
			r = (0.0f / 0.0f);
		} else {
			r = powf_core(logf_ext(abs<float>(a)), b);
			if ((a < 0.0f) && expo_odd_int) {
				r = -r;
			}
		}
		return r;
	}
} // namespace cst
} // namespace aobx
