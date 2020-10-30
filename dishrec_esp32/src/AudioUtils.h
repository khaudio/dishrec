#ifndef AUDIOUTILS_H
#define AUDIOUTILS_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>
#include <memory>


#ifndef M_PIl
#define M_PIl 3.141592653589793238462643383279502884
#endif

#ifndef TAU
#define TAU \
6.28318530717958647692\
5286766559005768394338\
7987502116419498891846\
1563281257241799725606\
96506842341359
#endif

template <typename T>
constexpr T get_zero()
{
    return (
            std::numeric_limits<T>::is_integer
            && std::is_unsigned<T>()
        ) ? pow(2, (sizeof(T) * 8) - 1) - 1 : 0;
}

enum rms_err
{
    WINDOW_NOT_SET = 140
};

template <typename T>
void clip_float(T& value);

template <typename I, typename F>
F int_to_float(I value);

template <typename F, typename I>
I float_to_int(F value);

template <typename I, typename F>
void int_to_float(std::vector<F> *converted, std::vector<I>* values);

template <typename F, typename I>
void float_to_int(std::vector<I>* converted, std::vector<F>* values);

template <typename I, typename F>
std::vector<F> int_to_float(std::vector<I> values);

template <typename F, typename I>
std::vector<I> float_to_int(std::vector<F> values);

template <typename T>
T get_radians(T degrees = 0);

template <typename T>
T get_decibels(T floatValue);

template <typename T>
T get_abs_decibels(T floatValue);

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

template <typename T>
void visualize(
        std::vector<T> values,
        double width = 70,
        double length = 0,
        bool fill = false
    );

template <typename T>
class RMS
{
protected:
    unsigned int index, windowLength;
    std::vector<T> window;
public:
    RMS();
    RMS(unsigned int windowLengthInSamples);
    ~RMS();
    void set_window(unsigned int numSamples);
    void set(T sample);
    void set(std::vector<T> samples);
    T get();
    size_t size();
};

template <typename T>
class RMSdB : public RMS<T>
{
    RMSdB();
    RMSdB(unsigned int windowLengthInSamples);
    T get();
};

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

