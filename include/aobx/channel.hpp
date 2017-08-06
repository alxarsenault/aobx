#pragma once

#include <cstddef>
#include <fst/print.h>
#include <type_traits>

namespace aobx {
template <typename T>
class channel {
public:
	inline channel(T* data = nullptr, std::size_t size = 0)
		: _data(data)
		, _size(size)
	{
	}

	channel(const channel&) = default;

	inline const T* data() const
	{
		return _data;
	}

	inline T* data()
	{
		return _data;
	}

	inline std::size_t size() const
	{
		return _size;
	}

	inline const T& operator[](std::size_t index) const
	{
		return _data[index];
	}

	inline T& operator[](std::size_t index)
	{
		return _data[index];
	}

	inline const T* begin() const
	{
		return _data;
	}

	inline const T* end() const
	{
		return _data + _size;
	}

	inline T* begin()
	{
		return _data;
	}

	inline T* end()
	{
		return _data + _size;
	}

	inline channel& operator*=(const channel& buffer)
	{
		/// @todo assert size.

		for (std::size_t i = 0; i < _size; i++) {
			_data[i] *= buffer._data[i];
		}
		return *this;
	}

	inline channel& operator*=(T v)
	{
		for (std::size_t i = 0; i < _size; i++) {
			_data[i] *= v;
		}
		return *this;
	}

	inline channel& operator+=(const channel& buffer)
	{
		/// @todo assert size.

		for (std::size_t i = 0; i < _size; i++) {
			_data[i] += buffer._data[i];
		}
		return *this;
	}

	inline channel& operator+=(T v)
	{
		for (std::size_t i = 0; i < _size; i++) {
			_data[i] += v;
		}
		return *this;
	}

	/// operation prototype :
	/// void op(T& v, std::size_t index) or void op(T&)
	template <typename... Ops>
	inline void operations(Ops... ops)
	{
		for (std::size_t i = 0; i < _size; i++) {
			internal_operations(_data[i], i, ops...);
		}
	}

	template <typename... Ops>
	inline void chain_operations(Ops... ops)
	{
		internal_chain_operations(ops...);
	}

private:
	T* _data;
	std::size_t _size;

	template <typename Op,
		typename std::enable_if<std::__invokable<Op, T&, std::size_t>::value>::type* = nullptr>
	static inline void internal_call_operation(Op op, T& v, std::size_t index)
	{
		op(v, index);
	}

	template <typename Op,
		typename std::enable_if<!std::__invokable<Op, T&, std::size_t>::value>::type* = nullptr>
	static inline void internal_call_operation(Op op, T& v, std::size_t index)
	{
		op(v);
	}

	template <typename Op>
	static inline void internal_operations(T& v, std::size_t index, Op op)
	{
		internal_call_operation(op, v, index);
	}

	template <typename Op, typename... Ops>
	static inline void internal_operations(T& v, std::size_t index, Op op, Ops... ops)
	{
		internal_call_operation(op, v, index);
		internal_operations(v, index, ops...);
	}

	template <typename Op>
	inline void internal_chain_operations(Op op)
	{
		for (std::size_t i = 0; i < _size; i++) {
			internal_call_operation(op, _data[i], i);
		}
	}

	template <typename Op, typename... Ops>
	inline void internal_chain_operations(Op op, Ops... ops)
	{
		for (std::size_t i = 0; i < _size; i++) {
			internal_call_operation(op, _data[i], i);
		}

		internal_chain_operations(ops...);
	}
};
} // namespace aobx
