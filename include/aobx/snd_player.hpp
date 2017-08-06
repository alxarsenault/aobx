#pragma once

#include <cstddef>
#include "aobx/snd_file.hpp"
#include "aobx/multi_channel.hpp"
#include "aobx/obj_traits.hpp"
#include "aobx/math.hpp"

namespace aobx {
template <typename T = float>
class snd_player : public generator<snd_player<T>, T> {
public:
	enum class play_type { once, repeat };

	snd_player()
	{
	}

	inline void set_snd_file(snd_file<T>* snd_file)
	{
		_snd_file = snd_file;
	}

	inline void set_play_type(play_type p_type)
	{
		_ptype = p_type;
	}

	inline void set_speed(float speed)
	{
		_speed = speed;
	}

	inline void play()
	{
		_is_playing = true;
	}

	inline void stop()
	{
		_is_playing = false;
	}

	void process(aobx::mono_value<T> output)
	{
		if (!_is_playing || !_snd_file || !_snd_file->is_valid()) {
			return;
		}

		aobx::stereo_channel<T> snd_buffer(_snd_file->get_buffer());

		T left_index = floor(_index);
		float mu = _index - left_index;

		if (left_index + 1 >= snd_buffer.size()) {
			_index = (T)0;
		}

		if (_ptype == play_type::once) {
			_is_playing = false;
			return;
		}

		output[0] += math::lineair_interpole(
			snd_buffer[0][to_index(left_index)], snd_buffer[0][to_index(left_index + 1)], mu);
		_index += _speed;
	}

	void process(aobx::mono_channel<T> buffer)
	{
		if (!_is_playing || !_snd_file || !_snd_file->is_valid()) {
			return;
		}

		aobx::mono_channel<T> snd_buffer(_snd_file->get_buffer());
		T index = _index;

		std::size_t i = 0;
		for (; i < buffer.size() && to_index(index + 1) < snd_buffer.size(); i++, index += _speed) {
			T left_index = floor(index);
			float mu = index - left_index;
			buffer[0][i] += math::lineair_interpole(
				snd_buffer[0][to_index(left_index)], snd_buffer[0][to_index(left_index + 1)], mu);
		}

		if (i >= buffer.size()) {
			_index = index;
			return;
		}

		//
		// Over.
		//

		if (_ptype == play_type::once) {
			_is_playing = false;
			_index = 0;
			return;
		}

		index = 0;

		for (; i < buffer.size() && to_index(index + 1) < snd_buffer.size(); i++, index += _speed) {
			T left_index = floor(index);
			float mu = index - left_index;
			buffer[0][i] += math::lineair_interpole(
				snd_buffer[0][to_index(left_index)], snd_buffer[0][to_index(left_index + 1)], mu);
		}

		_index = index;
	}

	void process(aobx::stereo_value<T> output)
	{
		if (!_is_playing || !_snd_file || !_snd_file->is_valid()) {
			return;
		}

		aobx::stereo_channel<T> snd_buffer(_snd_file->get_buffer());

		T left_index = floor(_index);
		float mu = _index - left_index;

		if (left_index + 1 >= snd_buffer.size()) {
			_index = (T)0;
		}
		// neg.
		else if (left_index < 0) {
			_index = snd_buffer.size() - 1;
		}

		if (_ptype == play_type::once) {
			_is_playing = false;
			return;
		}

		output[0] += math::lineair_interpole(
			snd_buffer[0][to_index(left_index)], snd_buffer[0][to_index(left_index + 1)], mu);
		output[1] += math::lineair_interpole(
			snd_buffer[1][to_index(left_index)], snd_buffer[1][to_index(left_index + 1)], mu);
		_index += _speed;
	}

	static inline std::size_t to_index(T v)
	{
		return static_cast<std::size_t>(v);
	}

	void process(aobx::stereo_channel<T> buffer)
	{
		if (!_is_playing || !_snd_file || !_snd_file->is_valid()) {
			return;
		}

		aobx::stereo_channel<T> snd_buffer(_snd_file->get_buffer());
		T index = _index;

		std::size_t i = 0;
		T left_index = floor(index);
		float mu = index - left_index;
		for (; i < buffer.size() && to_index(left_index + 1) < snd_buffer.size(); i++) {
			buffer[0][i] += math::lineair_interpole(
				snd_buffer[0][to_index(left_index)], snd_buffer[0][to_index(left_index + 1)], mu);
			buffer[1][i] += math::lineair_interpole(
				snd_buffer[1][to_index(left_index)], snd_buffer[1][to_index(left_index + 1)], mu);
			index += _speed;
			left_index = floor(index);
			mu = index - left_index;
		}

		// Done sound buffer.
		if (i >= buffer.size()) {
			_index = index;
			return;
		}

		//
		// Over.
		//

		if (_ptype == play_type::once) {
			_is_playing = false;
			_index = 0;
			return;
		}

		index = 0;

		left_index = floor(index);
		mu = index - left_index;
		for (; i < buffer.size() && to_index(left_index + 1) < snd_buffer.size(); i++) {
			buffer[0][i] += math::lineair_interpole(
				snd_buffer[0][to_index(left_index)], snd_buffer[0][to_index(left_index + 1)], mu);
			buffer[1][i] += math::lineair_interpole(
				snd_buffer[1][to_index(left_index)], snd_buffer[1][to_index(left_index + 1)], mu);
			index += _speed;
			left_index = floor(index);
			mu = index - left_index;
		}

		_index = index;
	}

private:
	long long _current_index = 0;
	T _speed = (T)1.0;
	T _index = (T)0.0;
	snd_file<T>* _snd_file = nullptr;
	bool _is_playing = false;
	play_type _ptype = play_type::once;
};
} // namespace aobx
