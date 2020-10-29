#ifndef INTFLOATCONV_H
#define INTFLOATCONV_H

#include <vector>
#include <algorithm>
#include <cmath>

template <typename T>
constexpr T get_zero()
{
    return (
            std::numeric_limits<T>::is_integer
            && std::is_unsigned<T>()
        ) ? pow(2, (sizeof(T) * 8) - 1) - 1 : 0;
}

template <typename T>
void clip_float(T& value);

template <typename I, typename F>
F int_to_float(I value);

template <typename F, typename I>
I float_to_int(F value);

template <typename I, typename F>
std::vector<F> int_to_float(std::vector<I> values);

template <typename F, typename I>
std::vector<I> float_to_int(std::vector<F> values);

template <typename I, typename F>
void int_to_float(std::vector<F> *converted, std::vector<I>* values);

template <typename F, typename I>
void float_to_int(std::vector<I>* converted, std::vector<F>* values);

template float get_zero<float>();
template double get_zero<double>();
template long double get_zero<long double>();
template int8_t get_zero<int8_t>();
template uint8_t get_zero<uint8_t>();
template int16_t get_zero<int16_t>();
template uint16_t get_zero<uint16_t>();
template int32_t get_zero<int32_t>();
template uint32_t get_zero<uint32_t>();
template int64_t get_zero<int64_t>();
template uint64_t get_zero<uint64_t>();

#endif
