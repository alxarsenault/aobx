#pragma once

#include <cstddef>
#include "aobx/multi_channel_buffer.hpp"
#include <type_traits>
#include <fst/assert.h>
#include <fst/print.h>

namespace aobx {
template <typename _ValueType, std::size_t _ChannelSize>
class multi_channel {
public:
	typedef _ValueType value_type;
	static constexpr std::size_t channel_size = _ChannelSize;
	static_assert(std::is_arithmetic<value_type>::value, "multi_channel value_type needs to be arithmetic.");
	static_assert(channel_size != 0, "multi_channel channel size must be greater than zero.");

	inline multi_channel(multi_channel_buffer<value_type>& buffers)
	{
		for (int i = 0; i < channel_size; i++) {
			_buffers[i] = buffers[i];
		}
	}

	inline multi_channel(const multi_channel<value_type, channel_size>& mcp)
	{
		for (int i = 0; i < channel_size; i++) {
			_buffers[i] = mcp._buffers[i];
		}
	}

	template <std::size_t N>
	inline multi_channel(const multi_channel<value_type, N>& mcp)
	{
		static_assert(channel_size <= N, "multi_channel channel size must be smaller or equal to N.");
		for (int i = 0; i < channel_size; i++) {
			_buffers[i] = mcp._buffers[i];
		}
	}

	inline channel<value_type> operator[](std::size_t index)
	{
		return _buffers[index];
	}

	constexpr inline std::size_t size() const
	{
		return _buffers[0].size();
	}

	template <std::size_t... _ChannelOrder>
	static inline multi_channel create(multi_channel_buffer<value_type>& buffers)
	{
		constexpr std::size_t channel_order_size = sizeof...(_ChannelOrder);
		static_assert(channel_order_size == channel_size,
			"The order channel number doesn't match the multi_channel channel size.");
		return multi_channel(buffers.channel(_ChannelOrder)...);
	}

	static inline multi_channel create(multi_channel_buffer<value_type>& buffers)
	{
		return multi_channel(buffers);
	}

	template <std::size_t... _ChannelOrder, std::size_t CSize>
	static inline multi_channel create(multi_channel<value_type, CSize>& buffers)
	{
		constexpr std::size_t channel_order_size = sizeof...(_ChannelOrder);
		static_assert(channel_order_size == CSize, "The order channel number doesn't match CSize.");
		return multi_channel(buffers[_ChannelOrder]...);
	}

	template <std::size_t... _ChannelOrder>
	inline multi_channel get_reorder() const
	{
		return multi_channel(_buffers[_ChannelOrder]...);
	}

	template <std::size_t... _ChannelOrder>
	inline void reorder()
	{
		multi_channel mc(get_reorder());

		for (std::size_t i = 0; i < channel_size; i++) {
			_buffers[i] = mc[i];
		}
	}

private:
	channel<value_type> _buffers[channel_size];

	template <typename... Buffers>
	inline multi_channel(Buffers... buffers)
		: _buffers{ buffers... }
	{
	}

	template <typename VType, std::size_t CSize>
	friend class multi_channel;
};

template <typename T>
using stereo_channel = multi_channel<T, 2>;

template <typename T>
using mono_channel = multi_channel<T, 1>;
} // namespace aobx
