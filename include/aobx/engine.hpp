#pragma once

#include <portaudio.h>
#include <vector>
#include <experimental/string_view>
#include <cmath>
#include "aobx/multi_channel_buffer.hpp"

namespace aobx {
struct output_device {
	std::experimental::string_view name;
	int channels;
	double default_sample_rate;
	int index;
};

template <typename T>
class engine {
public:
	engine();

	~engine();

	inline bool is_init() const
	{

		return _is_init;
	}

	bool open(const output_device& o_device, unsigned int sample_rate, unsigned int buffer_size);

	bool start();

	bool stop();

	std::vector<output_device> get_output_devices()
	{
		int n_devices = Pa_GetDeviceCount();

		if (n_devices <= 0) {
			return {};
		}

		std::vector<output_device> devices;
		devices.reserve(n_devices);

		for (int i = 0; i < n_devices; i++) {
			const PaDeviceInfo* device_info = Pa_GetDeviceInfo(i);

			// If output.
			if (device_info->maxOutputChannels != 0) {
				output_device d;
				d.name = std::experimental::string_view(device_info->name);
				d.channels = device_info->maxOutputChannels;
				d.default_sample_rate = device_info->defaultSampleRate;
				d.index = i;

				devices.emplace_back(std::move(d));
			}
		}

		return devices;
	}

protected:
	virtual void callback(const multi_channel_buffer<T>& inputs, multi_channel_buffer<T>& output)
	{
		for (std::size_t chan = 0; chan < output.channel_size(); chan++) {
			for (std::size_t i = 0; i < output.chunk_size(); i++) {
				output[chan][i] = 0.0;
			}
		}
	}

private:
	bool _is_init = false;
	multi_channel_buffer<T> _input_buffers;
	multi_channel_buffer<T> _output_buffers;
	PaStream* _stream;

	static int pa_callback(const void* in, void* out, unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
	{
		engine<T>& eng = *static_cast<engine<T>*>(userData);
		eng._output_buffers.fill(0.0);
		eng.callback(eng._input_buffers, eng._output_buffers);
		eng._output_buffers.copy_to_interlace_buffer(static_cast<float*>(out));
		return 0;
	}
};

template <typename T>
engine<T>::engine()
{
	PaError err = Pa_Initialize();

	if (err != paNoError) {
		std::cerr << Pa_GetErrorText(err) << std::endl;
		return;
	}

	_is_init = true;
}

template <typename T>
engine<T>::~engine()
{
	if (_is_init) {
		Pa_Terminate();
	}
}

template <typename T>
bool engine<T>::open(const output_device& o_device, unsigned int sample_rate, unsigned int buffer_size)
{
	_output_buffers = multi_channel_buffer<T>(2, buffer_size);

	PaStreamParameters output_params;

	// Ouput parameters.
	output_params.channelCount = 2;
	output_params.device = o_device.index;
	output_params.hostApiSpecificStreamInfo = nullptr;
	output_params.sampleFormat = paFloat32;
	output_params.suggestedLatency = Pa_GetDeviceInfo(output_params.device)->defaultLowInputLatency;

	PaError err = Pa_OpenStream(&_stream,
		nullptr, // No input.
		&output_params, sample_rate, buffer_size,
		paClipOff, // No cliping.
		&engine<T>::pa_callback, this);

	if (err != paNoError) {
		std::cerr << Pa_GetErrorText(err) << std::endl;
		return false;
	}

	return true;
}

template <typename T>
bool engine<T>::start()
{
	PaError err = Pa_StartStream(_stream);

	if (err != paNoError) {
		std::cerr << Pa_GetErrorText(err) << std::endl;
		return false;
	}

	return true;
}

template <typename T>
bool engine<T>::stop()
{
	return false;
}
} // namespace aobx
