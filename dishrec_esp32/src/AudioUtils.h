#ifndef AUDIOUTILS_H
#define AUDIOUTILS_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <iostream>
#include <memory>

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
    unsigned int _index, _windowLength;
    std::vector<T> _window;

public:
    RMS();
    RMS(unsigned int windowLengthInSamples);

    ~RMS();
    
    void set_window(unsigned int numSamples);

    size_t size();
    
    T get();
    T get_dBFS();

    void set(T sample);
    template <typename I>
    void set(I samples);
    void set(int8_t sample);
    void set(uint8_t sample);
    void set(int16_t sample);
    void set(uint16_t sample);
    void set(int32_t sample);
    void set(uint32_t sample);
    void set(int64_t sample);
    void set(uint64_t sample);

    void set(std::vector<T> samples);
    template <typename I>
    void set(std::vector<I> samples);
    void set(std::vector<int8_t> samples);
    void set(std::vector<uint8_t> samples);
    void set(std::vector<int16_t> samples);
    void set(std::vector<uint16_t> samples);
    void set(std::vector<int32_t> samples);
    void set(std::vector<uint32_t> samples);
    void set(std::vector<int64_t> samples);
    void set(std::vector<uint64_t> samples);

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

