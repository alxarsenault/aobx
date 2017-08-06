#pragma once

#include <cstddef>
#include "aobx/cache.hpp"
#include "aobx/channel.hpp"
#include <vector>
#include <memory>
#include <type_traits>
#include <cstring>

#include <fst/print.h>
#include <fst/assert.h>
#include <iostream>

namespace aobx {
template <typename T, std::size_t MaximumChannelSize = 32>
class multi_channel_buffer {
public:
	typedef T value_type;
	static constexpr std::size_t mamixum_channel_size = MaximumChannelSize;

	static_assert(std::is_arithmetic<T>::value, "multi_channel_buffer value_type needs to be arithmetic");

	inline multi_channel_buffer()
		: _n_channel(0)
		, _chunk_size(0)
	{
	}

	inline multi_channel_buffer(std::size_t n_channel, std::size_t chunk_size)
		: _n_channel(n_channel)
		, _chunk_size(chunk_size)
	{
		allocate();
	}

	inline bool is_valid() const
	{
		return _n_channel && _chunk_size;
	}

	inline void set_channel_size(std::size_t size)
	{
		if (_n_channel == size) {
			return;
		}

		_n_channel = size;
		allocate();
	}

	inline void set_chunk_size(std::size_t size)
	{
		if (_chunk_size == size) {
			return;
		}

		_chunk_size = size;
		allocate();
	}

	inline std::size_t chunk_size() const
	{
		return _chunk_size;
	}

	inline std::size_t channel_size() const
	{
		return _n_channel;
	}

	inline aobx::channel<T> channel(std::size_t index)
	{
		return aobx::channel<T>(_buffers_ptr[index], _chunk_size);
	}

	inline aobx::channel<T> operator[](std::size_t index)
	{
		return channel(index);
	}

	inline void fill(T value)
	{
		for (std::size_t i = 0; i < _n_channel; i++) {
			for (std::size_t k = 0; k < _chunk_size; k++) {
				_buffers_ptr[i][k] = value;
			}
		}
	}

	inline void copy_from_interlace_buffer(const T* buffer)
	{
		T** buffers = _buffers_ptr;
		std::size_t index = 0;
		for (std::size_t chunk = 0; chunk < _chunk_size; chunk++) {
			for (std::size_t chan = 0; chan < _n_channel; chan++) {
				buffers[chan][chunk] = buffer[index++];
			}
		}
	}

	inline void copy_from_separated_buffer(const T** buffer)
	{
		T** buffers = _buffers_ptr;
		const std::size_t buffer_size = _chunk_size * sizeof(T);

		for (std::size_t chan = 0; chan < _n_channel; chan++) {
			std::memcpy(buffers[chan], buffer[chan], buffer_size);
		}
	}

	inline void copy_from_inline_buffer(const T* buffer)
	{
		T** buffers = _buffers_ptr;
		const std::size_t buffer_size = _chunk_size * sizeof(T);

		for (std::size_t chan = 0; chan < _n_channel; chan++) {
			std::memcpy(buffers[chan], buffer + chan * buffer_size, buffer_size);
		}
	}

	inline void copy_to_interlace_buffer(T* buffer)
	{
		T** buffers = _buffers_ptr;
		std::size_t index = 0;

		for (std::size_t chunk = 0; chunk < _chunk_size; chunk++) {
			for (std::size_t chan = 0; chan < _n_channel; chan++) {
				buffer[index++] = buffers[chan][chunk];
			}
		}
	}

	inline void copy_to_separated_buffer(T** buffer)
	{
		T** buffers = _buffers_ptr;
		const std::size_t buffer_size = _chunk_size * sizeof(T);

		for (std::size_t chan = 0; chan < _n_channel; chan++) {
			T* chan_buffer = buffer[chan];
			std::memcpy(chan_buffer, buffers[chan], buffer_size);
		}
	}

	inline void copy_to_inline_buffer(T* buffer)
	{
		T** buffers = _buffers_ptr;
		const std::size_t buffer_size = _chunk_size * sizeof(T);

		for (std::size_t chan = 0; chan < _n_channel; chan++) {
			std::memcpy(buffer + chan * buffer_size, buffers[chan], buffer_size);
		}
	}

private:
	std::size_t _n_channel;
	std::size_t _chunk_size;
	T* _buffers_ptr[mamixum_channel_size];
	std::unique_ptr<std::uint8_t[]> _raw_buffer;

	/// Allocate chunk buffer.
	/// Only allocate buffer if n_channel and chunk_size are greater than zero.
	inline void allocate()
	{
		if (!is_valid()) {
			return;
		}

		const std::size_t cache_size = cache::size();
		FST_ASSERT_MSG(cache_size > 0, "Can't get cache size.");

		std::memset(_buffers_ptr, 0, sizeof(T*) * mamixum_channel_size);
		const std::size_t single_buffer_size = _chunk_size * sizeof(T);

		/// @todo Is this too big ? + _n_channel * (cache_size - 1) ?? or + (cache_size - 1) ??
		const std::size_t raw_size = _n_channel * single_buffer_size + _n_channel * (cache_size - 1);
		_raw_buffer = std::unique_ptr<std::uint8_t[]>(new std::uint8_t[raw_size]);

		void* raw_ptr = static_cast<void*>(_raw_buffer.get());
		const std::uint8_t* raw_ptr_end = static_cast<std::uint8_t*>(raw_ptr) + raw_size;
		std::size_t space = raw_size;

		for (std::size_t i = 0; i < _n_channel; i++) {
			void* ptr = std::align(cache_size, single_buffer_size, raw_ptr, space);

			if (ptr == nullptr) {
				FST_ASSERT_MSG(ptr != nullptr, "Can't cache align buffer.");
			}

			_buffers_ptr[i] = static_cast<T*>(ptr);
			raw_ptr = static_cast<std::uint8_t*>(ptr) + single_buffer_size;
			space = raw_ptr_end - static_cast<std::uint8_t*>(static_cast<void*>(_buffers_ptr[i]));
		}
	}
};
} // namespace aobx
