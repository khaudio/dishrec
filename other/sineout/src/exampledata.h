#include <cstdint>
#include <vector>

#ifndef M_PIl
#define M_PIl               3.141592653589793238462643383279502884
#endif

template <typename T>
std::vector<T> sine(float seconds, float freq, unsigned int sampleRate, float amplitude);
template <typename T>
T sine_sample(float freq, unsigned int sampleRate, float amplitude, float& rads);

std::vector<float> sine_32fp(
        float seconds, float freq, unsigned int sampleRate, float amplitude
    )
{
    std::vector<float> data;
    size_t samplesPerCycle(sampleRate / freq);
    size_t numSamples(seconds * sampleRate);
    float step((2 * M_PIl) / samplesPerCycle), rads(0);
    data.reserve(numSamples);
    for (size_t i(0); i < numSamples; ++i)
    {
        float state = sin(rads) * amplitude;
        data.push_back(state);
        rads += step;
    }
    return data;
}

float sine_32fp_sample(
        float freq, unsigned int sampleRate, float amplitude, float& rads
    )
{
    static size_t resetIndex(0);
    size_t samplesPerCycle(sampleRate / freq);
    if (resetIndex++ > samplesPerCycle)
    {
        rads = 0;
        resetIndex = 0;
    }
    float step((2 * M_PIl) / samplesPerCycle);
    float state = sin(rads) * amplitude;
    rads += step;
    return state;
}

std::vector<int16_t> sine_int16(
        float seconds, float freq, unsigned int sampleRate, float amplitude
    )
{
    std::vector<int16_t> samples;
    std::vector<float> floaters(sine_32fp(seconds, freq, sampleRate, amplitude));
    samples.reserve(floaters.size());
    for (auto floater: floaters)
    {
        samples.emplace_back(floater * (floater > 0 ? 32767 : 32768));
    }
    return samples;
}

int16_t sine_int16_sample(
        float freq, unsigned int sampleRate, float amplitude, float& rads
    )
{
    float floater(sine_32fp_sample(freq, sampleRate, amplitude, rads));
    return floater * (floater > 0 ? 32767 : 32768);
}

template <>
std::vector<float> sine(float seconds, float freq, unsigned int sampleRate, float amplitude)
{
    return sine_32fp(seconds, freq, sampleRate, amplitude);
}

template <>
std::vector<int16_t> sine(float seconds, float freq, unsigned int sampleRate, float amplitude)
{
    return sine_int16(seconds, freq, sampleRate, amplitude);
}

template <>
float sine_sample(float freq, unsigned int sampleRate, float amplitude, float& rads)
{
    return sine_32fp_sample(freq, sampleRate, amplitude, rads);
}

template <>
int16_t sine_sample(float freq, unsigned int sampleRate, float amplitude, float& rads)
{
    return sine_int16_sample(freq, sampleRate, amplitude, rads);
}
