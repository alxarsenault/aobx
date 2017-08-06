#pragma once

#include <cstddef>
#include "aobx/obj_traits.hpp"
#include "aobx/math.hpp"
#include "aobx/value.hpp"
#include "aobx/multi_channel.hpp"

#include <fst/print.h>

namespace aobx {
template <typename T>
class phasor : public generator<buffer_player<T>, T> {
public:
	inline void set_frequency(T freq)
	{
		_freq = freq;
	}

	inline T get_frequency() const
	{
		return _freq;
	}
    
    inline void set_buffer(aobx::multi_channel_buffer<T>* buffer) {
        _buffer = buffer;
    }

	void process(aobx::mono_value<T> output)
	{
        aobx::mono_channel<T> data(*_buffer);
        index_info index = wave_interpole(_freq, 0, data.size());
        output[0] += math::lineair_interpole<T>(data[0][index.left], data[0][index.right], index.frac);
	}

	void process(aobx::mono_channel<T> buffer)
	{
        aobx::mono_channel<T> data(*_buffer);
		for (auto& n : buffer[0]) {
            index_info index = wave_interpole(_freq, 0, data.size());
			n += math::lineair_interpole<T>(data[0][index.left], data[0][index.right], index.frac);
		}
	}

	void process(aobx::stereo_value<T> output)
	{
        aobx::stereo_channel<T> data(*_buffer);
        index_info index = wave_interpole(_freq, 0, data.size());
		output[0] += math::lineair_interpole<T>(data[0][index.left], data[0][index.right], index.frac);
		output[1] += math::lineair_interpole<T>(data[1][index.left], data[1][index.right], index.frac);
	}

	void process(aobx::stereo_channel<T> buffer)
	{
        aobx::stereo_channel<T> data(*_buffer);
        
		for (std::size_t i = 0; i < buffer.size(); i++) {
            index_info index = wave_interpole(_freq, 0, data.size());
            buffer[0][i] += math::lineair_interpole<T>(data[0][index.left], data[0][index.right], index.frac);
            buffer[1][i] += math::lineair_interpole<T>(data[1][index.left], data[1][index.right], index.frac);
		}
	}

private:
	T _freq = (T)1.0;
	T _phase = (T)0.0;
    aobx::multi_channel_buffer<T>* _buffer;
    
    struct index_info {
        std::size_t left;
        std::size_t right;
        T frac;
    };
    
    index_info wave_interpole(T freq, T phase, std::size_t buffer_size)
	{
        aobx::stereo_channel<T> buffer(*_buffer);
        
        const T v = 1.0 - _phase * math::one_over_two_pi;
        _phase -= ((math::two_pi / 44100.0) * (_freq + phase));
        
        while (_phase > math::two_pi) {
            _phase -= math::two_pi;
        }
        while (_phase < 0.0) {
            _phase += math::two_pi;
        }

        std::size_t pos_left = v * ((T)buffer.size() - 1.0);
        std::size_t pos_right = pos_left + 1;

        if (pos_right == buffer.size()) {
            pos_right = 0;
        }
        
        return index_info{pos_left, pos_right, v * (T)buffer.size() - pos_left};
	}
};
} // namespace aobx
