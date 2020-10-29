#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <iostream>
#include <vector>

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

namespace AudioAlgorithms
{

template <typename T>
T get_radians(T degrees = 0);

template <typename T>
void visualize(std::vector<T> values);

template <typename T>
void visualize_scale(
        std::vector<T> values,
        bool fill = false,
        double width = 70,
        double length = 0
    );

template <typename T>
std::vector<T> sine(
        double freq = 1000,
        double cycles = 1,
        unsigned int samplerate = 48000
    );

template <typename T>
void sine(
        std::vector<T>* buff,
        double freq,
        unsigned int samplerate
    );

template <typename T>
std::vector<T> sine_for_seconds(
        double freq,
        double seconds,
        unsigned int samplerate
    );

template <typename T>
std::vector<T> fill_buffer(
        double (*operation)(double),
        double freq = 1000,
        double cycles = 1,
        unsigned int samplerate = 48000
    );

template <typename T>
void fill_buffer(
        std::vector<T>* buff,
        double (*operation)(double),
        double freq = 1000,
        unsigned int samplerate = 48000
    );

template <typename T>
T hann_window(T values, int length);

};