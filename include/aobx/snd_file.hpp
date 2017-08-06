#pragma once

#include <cstddef>
#include <string>
#include <sndfile.h>
#include "aobx/multi_channel.hpp"

namespace aobx {
struct snd_file_info {
	long long n_frames;
	int sample_rate;
	int n_channels;
};

template <typename T = float>
class snd_file {
public:
	snd_file()
	{
	}

	int open(const std::string& file_path)
	{
		SF_INFO snd_info;
		SNDFILE* snd_file = sf_open(file_path.c_str(), SFM_READ, &snd_info);

		if (!snd_file) {
			_info = snd_file_info{ 0, 0, 0 };
			_is_valid = false;
			return -1;
		}

		std::vector<T> buffer;
		buffer.resize(snd_info.frames * snd_info.channels);

		/// @todo Change this.
		sf_count_t n_frames = sf_readf_float(snd_file, buffer.data(), snd_info.frames);

		if (n_frames == 0) {
			_info = snd_file_info{ 0, 0, 0 };
			_is_valid = false;
			return -1;
		}

		_info.n_frames = snd_info.frames;
		_info.sample_rate = snd_info.samplerate;
		_info.n_channels = snd_info.channels;
		_is_valid = true;

		_snd_buffer.set_channel_size(_info.n_channels);
		_snd_buffer.set_chunk_size(_info.n_frames);
		_snd_buffer.copy_from_interlace_buffer(buffer.data());
		return 0;
	}

	inline long long get_frame_size() const
	{
		return _info.n_frames;
	}

	inline int get_channel_size() const
	{
		return _info.n_channels;
	}

	inline int get_sample_rate() const
	{
		return _info.sample_rate;
	}

	inline snd_file_info get_info() const
	{
		return _info;
	}

	inline aobx::multi_channel_buffer<T>& get_buffer()
	{
		return _snd_buffer;
	}

	inline bool is_valid() const
	{
		return _is_valid;
	}

private:
	snd_file_info _info = { 0, 0, 0 };
	aobx::multi_channel_buffer<T> _snd_buffer;
	bool _is_valid = false;
};
} // namespace aobx
