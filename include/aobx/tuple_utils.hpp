#pragma once

#include <cstddef>
#include <tuple>
#include <utility>

namespace aobx {
    template<typename T, unsigned N, typename... Args>
    struct generate_tuple_type
    {
        typedef typename generate_tuple_type<T, N - 1, T, Args...>::type type;
    };
    
    template<typename T, typename... Args>
    struct generate_tuple_type<T, 0, Args...>
    {
        typedef std::tuple<Args...> type;
    };
    
    
    
//    template<std::size_t I = 0, typename... Tp>
//    inline typename std::enable_if<I == sizeof...(Tp), void>::type
//    print(std::tuple<Tp...>& t)
//    { }
//    
//    template<std::size_t I = 0, typename... Tp>
//    inline typename std::enable_if<I < sizeof...(Tp), void>::type
//    print(std::tuple<Tp...>& t)
//    {
//        std::cout << std::get<I>(t) << std::endl;
//        print<I + 1, Tp...>(t);
//    }
    
    //    template<std::size_t index, std::size_t In, std::size_t... remPack>
    //    struct getVal
    //    {
    //        static const std::size_t val = getVal<index-1, remPack...>::val;
    //    };
    //
    //    template<std::size_t In, std::size_t...remPack>
    //    struct getVal<0, In, remPack...>
    //    {
    //        static const std::size_t val = In;
    //    };
    
    
    //    template<typename ValueType, std::size_t I = 0, typename... Tp>
    //    inline typename std::enable_if<I == sizeof...(Tp), void>::type
    //    assign(std::tuple<Tp...>& t, multi_channel_buffer<ValueType>& buffers)
    //    { }
    //
    //    template<typename ValueType, std::size_t I = 0, typename... Tp>
    //    inline typename std::enable_if<I < sizeof...(Tp), void>::type
    //    assign(std::tuple<Tp...>& t, multi_channel_buffer<ValueType>& buffers)
    //    {
    ////        std::cout << std::get<I>(t) << std::endl;
    //        std::get<I>(t) = buffers[I];
    //        assign<ValueType, I + 1, Tp...>(t, buffers);
    //    }
} // aobx
