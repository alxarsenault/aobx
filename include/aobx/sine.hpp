#pragma once

#include "aobx/multi_channel.hpp"
#include "aobx/value.hpp"
#include "aobx/obj_traits.hpp"
#include <array>
#include "aobx/math.hpp"
#include "aobx/cst_wave_table.hpp"

namespace aobx {
template <typename T>
class sine : public generator<sine<T>, T> {
public:
	inline void set_frequency(T freq)
	{
		_freq = freq;
	}

	inline T get_frequency() const
	{
		return _freq;
	}

	void process(aobx::mono_value<T> output)
	{
		output[0] = wave_interpole(_freq, 0);
	}

	void process(aobx::mono_channel<T> buffer)
	{
		for (auto& n : buffer[0]) {
			n += wave_interpole(_freq, 0);
		}
	}

	void process(aobx::stereo_value<T> output)
	{
		const T v = wave_interpole(_freq, 0);
		output[0] = v;
		output[1] = v;
	}

	void process(aobx::stereo_channel<T> buffer)
	{
		for (std::size_t i = 0; i < buffer[0].size(); i++) {
			const T v = wave_interpole(_freq, 0);
			buffer[0][i] += v;
			buffer[1][i] += v;
		}
	}

private:
	static constexpr std::size_t table_size = 4096;
	T _freq = 500.0;
	T _phase = 0.0;

	//        class Table : public std::array<T, table_size + 1> {
	//        public:
	//            Table() {
	//                const T one_over_table_size = 1.0 / T(table_size);
	//                for(int i = 0; i < table_size; i++) {
	//                    _table[i] = sin(2.0 * M_PI * i * one_over_table_size);
	//                }
	//                _table[table_size] = _table[0];
	//            }
	//        };

	//        static Table _table;
	static constexpr std::array<T, table_size> _table = cst::create_sine_table<T, table_size>();
	//        static constexpr std::array<T, table_size> _table = cst::create_sine_sum_table<T, table_size,
	//        3>(std::array<T, 3>{1.0f, 2.0f, 3.0f});

	T wave_interpole(T freq, T phase)
	{
		const T v = 1.0 - _phase * math::one_over_two_pi;
		_phase += ((math::two_pi / 44100.0) * (_freq + phase));

		while (_phase > math::two_pi) {
			_phase -= math::two_pi;
		}
		while (_phase < 0.0) {
			_phase += math::two_pi;
		}

		unsigned int pos_left = v * ((T)table_size - 1.0);
		T frac = v * (T)table_size - pos_left;

		unsigned int pos_right = pos_left + 1;
		if (pos_right == table_size) {
			pos_right = 0;
		}

		return math::lineair_interpole<T>(_table[pos_left], _table[pos_right], frac);
	}
};

template <typename T>
constexpr std::array<T, sine<T>::table_size> sine<T>::_table;
} // namespace aobx
