#include "testdata.h"

/* Functions for generating and measuring test audio data */

using namespace AudioAlgorithms;

template <typename T>
T AudioAlgorithms::get_radians(T degrees)
{
    return degrees * (M_PIl / 180);
}

template <typename T>
void AudioAlgorithms::visualize(std::vector<T> values)
{
    size_t size = values.size();
    for (size_t i = 0; i < size; ++i)
    {
        int value = std::round((values[i] + 1) * 40);
        for (int j = 0; j < value; ++j)
        {
            std::cout << " ";
        }
        std::cout << "\u2022" << " " << +values[i] << std::endl;
    }
}

template <typename T>
void AudioAlgorithms::visualize_scale(
        std::vector<T> values,
        bool fill,
        double width,
        double length
    )
{
    uint64_t range = (
            *std::max_element(values.begin(), values.end())
            - *std::min_element(values.begin(), values.end())
        );
    double scale = width / range,
    increment = (
            static_cast<double>(values.size())
            / (length > 0 ? length : values.size())
        );
    double difference = width - (*std::max_element(
            values.begin(), values.end()) * scale
        );
    for (double i = 0; i < values.size(); i += increment)
    {
        T value = (values[std::round(i)] * scale) + difference;
        for (T j = 0; j < value; ++j)
        {
            std::cout << (fill ? "-" : " ");
        }
        std::cout << "\u2022" << " " << +values[i] << std::endl;
    }
}

template <typename T>
std::vector<T> AudioAlgorithms::sine(
        double freq,
        double cycles,
        unsigned int samplerate
    )
{
    std::vector<T> buff;
    int numSamples((1 / freq) * samplerate);
    buff.reserve(numSamples * cycles);
    double step(TAU / numSamples), radians(0);
    for (T cycle(0); cycle < cycles; ++cycle)
    {
        for (int i(0); i < numSamples; ++i)
        {
            buff.emplace_back(sin(radians));
            radians += step;
        }
    }
    return buff;
}

template <typename T>
void AudioAlgorithms::sine(
        std::vector<T>* buff,
        double freq,
        unsigned int samplerate
    )
{
    int numSamples((1 / freq) * samplerate);
    double step(TAU / numSamples), radians(0);
    const size_t length = buff->size();
    for (size_t i(0); i < length; ++i)
    {
        buff->at(i) = (sin(radians));
        radians += step;
    }
}

template <typename T>
std::vector<T> AudioAlgorithms::sine_for_seconds(
        double freq,
        double seconds,
        unsigned int samplerate
    )
{
    std::vector<T> buff;
    int cycleLength((1 / freq) * samplerate);
    int numSamples(cycleLength * freq * seconds);
    buff.reserve(numSamples);
    double step(TAU / cycleLength), radians(0);
    for (int sampleNum(0); sampleNum < numSamples; ++sampleNum)
    {
        for (int i(0); i < cycleLength; ++i)
        {
            buff.emplace_back(sin(radians));
            radians += step;
        }
    }
    return buff;
}

template <typename T>
std::vector<T> AudioAlgorithms::fill_buffer(
        double (*operation)(double),
        double freq,
        double cycles,
        unsigned int samplerate
    )
{
    std::vector<T> buff;
    int numSamples((1 / freq) * samplerate);
    buff.reserve(numSamples * cycles);
    double step(TAU / numSamples), radians(0);
    for (T cycle(0); cycle < cycles; ++cycle)
    {
        for (int i(0); i < numSamples; ++i)
        {
            buff.emplace_back((*operation)(radians));
            radians += step;
        }
    }
    return buff;
}

template <typename T>
void AudioAlgorithms::fill_buffer(
        std::vector<T>* buff,
        double (*operation)(double),
        double freq,
        unsigned int samplerate
    )
{
    int samplesPerCycle((1 / freq) * samplerate);
    double step(TAU / samplesPerCycle), radians(0);
    const size_t length = buff->size();
    for (size_t i(0); i < length; ++i)
    {
        buff->at(i) = ((*operation)(radians));
        radians += step;
    }
}

template <typename T>
inline T AudioAlgorithms::hann_window(T values, int length)
{
    double divisor(length - 1);
    for (int i(0); i < length; ++i)
    {
        values[i] *= (1 - cos(TAU * i / divisor)) / 2;
    }
    return values;
}


template int8_t AudioAlgorithms::get_radians<int8_t>(int8_t);
template uint8_t AudioAlgorithms::get_radians<uint8_t>(uint8_t);
template int16_t AudioAlgorithms::get_radians<int16_t>(int16_t);
template uint16_t AudioAlgorithms::get_radians<uint16_t>(uint16_t);
template int32_t AudioAlgorithms::get_radians<int32_t>(int32_t);
template uint32_t AudioAlgorithms::get_radians<uint32_t>(uint32_t);
template float AudioAlgorithms::get_radians<float>(float);
template double AudioAlgorithms::get_radians<double>(double);

template void AudioAlgorithms::visualize<int8_t>(std::vector<int8_t>);
template void AudioAlgorithms::visualize<uint8_t>(std::vector<uint8_t>);
template void AudioAlgorithms::visualize<int16_t>(std::vector<int16_t>);
template void AudioAlgorithms::visualize<uint16_t>(std::vector<uint16_t>);
template void AudioAlgorithms::visualize<int32_t>(std::vector<int32_t>);
template void AudioAlgorithms::visualize<uint32_t>(std::vector<uint32_t>);
template void AudioAlgorithms::visualize<float>(std::vector<float>);
template void AudioAlgorithms::visualize<double>(std::vector<double>);

template void AudioAlgorithms::visualize_scale<int8_t>(std::vector<int8_t>, bool, double, double);
template void AudioAlgorithms::visualize_scale<uint8_t>(std::vector<uint8_t>, bool, double, double);
template void AudioAlgorithms::visualize_scale<int16_t>(std::vector<int16_t>, bool, double, double);
template void AudioAlgorithms::visualize_scale<uint16_t>(std::vector<uint16_t>, bool, double, double);
template void AudioAlgorithms::visualize_scale<int32_t>(std::vector<int32_t>, bool, double, double);
template void AudioAlgorithms::visualize_scale<uint32_t>(std::vector<uint32_t>, bool, double, double);
template void AudioAlgorithms::visualize_scale<float>(std::vector<float>, bool, double, double);
template void AudioAlgorithms::visualize_scale<double>(std::vector<double>, bool, double, double);

template std::vector<float> AudioAlgorithms::sine<float>(double, double, unsigned int);
template std::vector<double> AudioAlgorithms::sine<double>(double, double, unsigned int);

template void AudioAlgorithms::sine<float>(std::vector<float>*, double, unsigned int);
template void AudioAlgorithms::sine<double>(std::vector<double>*, double, unsigned int);

template std::vector<float> AudioAlgorithms::sine_for_seconds<float>(double, double, unsigned int);
template std::vector<double> AudioAlgorithms::sine_for_seconds<double>(double, double, unsigned int);

template std::vector<float> AudioAlgorithms::fill_buffer<float>(double (*)(double), double, double, unsigned int);
template std::vector<double> AudioAlgorithms::fill_buffer<double>(double (*)(double), double, double, unsigned int);

template void AudioAlgorithms::fill_buffer<float>(std::vector<float>*, double (*)(double), double, unsigned int);
template void AudioAlgorithms::fill_buffer<double>(std::vector<double>*, double (*)(double), double, unsigned int);

template std::vector<int8_t> AudioAlgorithms::hann_window<std::vector<int8_t>>(std::vector<int8_t>, int);
template std::vector<uint8_t> AudioAlgorithms::hann_window<std::vector<uint8_t>>(std::vector<uint8_t>, int);
template std::vector<int16_t> AudioAlgorithms::hann_window<std::vector<int16_t>>(std::vector<int16_t>, int);
template std::vector<uint16_t> AudioAlgorithms::hann_window<std::vector<uint16_t>>(std::vector<uint16_t>, int);
template std::vector<int32_t> AudioAlgorithms::hann_window<std::vector<int32_t>>(std::vector<int32_t>, int);
template std::vector<uint32_t> AudioAlgorithms::hann_window<std::vector<uint32_t>>(std::vector<uint32_t>, int);
template std::vector<float> AudioAlgorithms::hann_window<std::vector<float>>(std::vector<float>, int);
template std::vector<double> AudioAlgorithms::hann_window<std::vector<double>>(std::vector<double>, int);
