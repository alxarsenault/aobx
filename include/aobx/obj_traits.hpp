#pragma once

#include <cstddef>
#include "aobx/multi_channel.hpp"

namespace aobx {
template <typename _FuncType>
using has_fct = typename std::is_same<_FuncType, void>;

template <typename _FuncType>
using enable_if_func = typename std::enable_if<has_fct<_FuncType>::value>::type;

// Mono_block_output.
template <typename T>
using mono_block_output_func
	= decltype(std::declval<T>().process(std::declval<aobx::mono_channel<typename T::value_type>>()));
template <typename T, typename = void>
struct _has_mono_block_output : std::false_type {
};
template <typename T>
struct _has_mono_block_output<T, enable_if_func<mono_block_output_func<T>>> : std::true_type {
};

// Mono_block_input_output.
template <typename T>
using mono_block_input_output_func
	= decltype(std::declval<T>().process(std::declval<const aobx::mono_channel<typename T::value_type>>(),
		std::declval<aobx::mono_channel<typename T::value_type>>()));

template <typename T, typename = void>
struct _has_mono_block_input_output : std::false_type {
};

template <typename T>
struct _has_mono_block_input_output<T, enable_if_func<mono_block_input_output_func<T>>> : std::true_type {
};

// Stereo_block_output.
template <typename T>
using stereo_block_output_func
	= decltype(std::declval<T>().process(std::declval<aobx::stereo_channel<typename T::value_type>>()));

template <typename T, typename = void>
struct _has_stereo_block_output : std::false_type {
};

template <typename T>
struct _has_stereo_block_output<T, enable_if_func<stereo_block_output_func<T>>> : std::true_type {
};

template <class _AudioObject, typename _ValueType, typename _Generator, typename _Effect, typename _Analyser,
	typename _Buffer>
class obj_traits {
public:
	typedef _AudioObject object_type;
	typedef _ValueType value_type;
	typedef _Generator is_generator;
	typedef _Effect is_effect;
	typedef _Analyser is_analyser;
	typedef _Buffer is_buffer;

	typedef _has_mono_block_output<object_type> has_mono_block_out;
	typedef _has_mono_block_input_output<object_type> has_mono_block_in_out;
	typedef _has_stereo_block_output<object_type> has_stereo_block_out;
};

template <class _AudioObject, typename _ValueType>
class generator : public obj_traits<_AudioObject, //
					  _ValueType, //
					  std::true_type, // Generator.
					  std::false_type, // Effect.
					  std::false_type, // Analyser.
					  std::false_type // Buffer.
					  > {
public:
};
} // namespace aobx
