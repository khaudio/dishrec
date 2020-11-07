#ifndef AUDIOUTILS_H
#define AUDIOUTILS_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>
#include <memory>
#include <cstring>
#include "int_audio.h"

#include <iomanip>

#ifndef M_PIl
#define M_PIl \
3.14159265358979323846\
2643383279502884197169\
3993751058209749445923\
0781640628620899862803\
4825342117
#endif

#ifndef TAU
#define TAU \
6.28318530717958647692\
5286766559005768394338\
7987502116419498891846\
1563281257241799725606\
9650684234
#endif

template <typename T>
constexpr T get_zero()
{
    return (
            std::numeric_limits<T>::is_integer
            && std::is_unsigned<T>()
        ) ? pow(2, (sizeof(T) * 8) - 1) - 1 : 0;
}

template <>
constexpr int_audio get_zero()
{
    return 0;
}

template <typename T>
void clip_float(T* value);

template <typename I, typename F>
void int_to_float(F* converted, I* value);

template <>
void int_to_float(float* converted, int_audio* value);

template <>
void int_to_float(double* converted, int_audio* value);

template <>
void int_to_float(long double* converted, int_audio* value);

template <typename F, typename I>
void float_to_int(I* converted, F* value);

template <>
void float_to_int(int_audio* converted, float* value);

template <>
void float_to_int(int_audio* converted, double* value);

template <>
void float_to_int(int_audio* converted, long double* value);

template <typename I, typename F>
void int_to_float(std::vector<F>* converted, std::vector<I>* values);

template <typename F, typename I>
void float_to_int(std::vector<I>* converted, std::vector<F>* values);

template <typename T>
void pack_data(uint8_t* data, T* value, int width);

template <typename T>
void unpack_data(T* value, uint8_t* data, int width);

template <typename T>
T get_radians(T degrees = 0);

template <typename T>
T get_decibels(T floatValue);

template <typename T = double>
int sgn(T value);

template <>
int sgn(int_audio value);

template <typename T = double>
int16_t convert_loudness_to_int(T value);

template <typename T>
T hann_window(T values, int length);

template <typename T>
void sine(
        std::vector<T>* buff,
        T freq,
        unsigned int samplerate,
        T* rads = nullptr
    );

template <typename T>
std::vector<T> sine(
        T freq,
        unsigned int numSamples,
        unsigned int samplerate,
        T* rads = nullptr
    );

void get_random_str(
        char* buff,
        size_t length,
        const char* validChars,
        unsigned int seed = 0
    );

template <typename T>
T get_max(uint8_t* values, size_t numBytes, int width);

template <typename T>
T get_min(uint8_t* values, size_t numBytes, int width);

template <typename T>
void visualize(
        std::vector<T> values,
        double length = 0,
        double lineWidth = 70,
        bool fill = false
    );

template <typename T>
void visualize(
        uint8_t* values,
        size_t numBytes,
        int sampleWidth,
        int lineWidth = 70,
        bool fill = false
    );

void print(const uint8_t* buff, size_t buffsize);

void print_hex(const uint8_t* buff, size_t buffsize);

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
template int_audio get_zero<int_audio>();

#endif

