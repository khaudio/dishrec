#include "AudioUtils.h"

template <typename T>
inline void clip_float(T& value)
{
    value = (
            value > 1.0 ?
            (value < -1.0 ? -1.0 : value)
            : value
        );
}

template <typename I, typename F>
inline F int_to_float(I value)
{
    F converted;
    if (std::is_signed<I>())
    {
        converted = ((value >= 0) ?
                (static_cast<F>(value) / std::numeric_limits<I>::max())
                : -(static_cast<F>(value) / std::numeric_limits<I>::min())
            );
    }
    else
    {
        I zero = static_cast<F>(get_zero<I>());
        converted = ((value >= zero) ?
                ((static_cast<F>(value) / zero) - zero)
                : ((static_cast<F>(value) - zero) / zero)
            );
    }
    clip_float(converted);
    return converted;
}

template <typename F, typename I>
inline I float_to_int(F value)
{
    if (std::is_signed<I>())
    {
        return round(value * (
                value >= 0 ? 
                std::numeric_limits<I>::max()
                : -std::numeric_limits<I>::min()
            ));
    }
    else
    {
        I zero = get_zero<I>();
        return round(
                value
                * ((value >= 0) ? (zero - 1) : zero)
                + (std::is_signed<I>() ? 0 : zero)
            );
    }
}

template <typename I, typename F>
inline void int_to_float(std::vector<F>* converted, std::vector<I>* values)
{
    size_t length = converted->size();
    for (size_t i(0); i < length; ++i)
    {
        converted->at(i) = int_to_float<I, F>(values->at(i));
    }
}

template <typename F, typename I>
inline void float_to_int(std::vector<I>* converted, std::vector<F>* values)
{
    size_t length = converted->size();
    for (size_t i(0); i < length; ++i)
    {
        converted->at(i) = float_to_int<F, I>(values->at(i));
    }
}

template <typename I, typename F>
inline std::vector<F> int_to_float(std::vector<I> values)
{
    size_t length = values.size();
    std::vector<F> converted;
    converted.reserve(length);
    int_to_float<I, F>(&converted, &values);
    return converted;
}

template <typename F, typename I>
inline std::vector<I> float_to_int(std::vector<F> values)
{
    size_t length = values.size();
    std::vector<I> converted;
    converted.reserve(length);
    float_to_int<F, I>(&converted, &values);
    return converted;
}

template <typename T>
inline T get_radians(T degrees)
{
    return degrees * (M_PIl / 180);
}

template <typename T>
inline T get_decibels(T floatValue)
{
    return 20 * log10(floatValue);
}

template <typename T>
inline T get_abs_decibels(T floatValue)
{
    return 20 * log10(abs(floatValue));
}

template <typename T>
inline T hann_window(T values, int length)
{
    double divisor(length - 1);
    for (int i(0); i < length; ++i)
    {
        values[i] *= (1 - cos(TAU * i / divisor)) / 2;
    }
    return values;
}

template <typename T>
void sine(
        std::vector<T>* buff,
        T freq,
        unsigned int samplerate,
        T* rads
    )
{
    T samplesPerCycle((1 / freq) * samplerate), zeroRadians(0);
    rads = ((rads == nullptr) ? &zeroRadians : rads);
    double step(TAU / samplesPerCycle);
    const size_t length = buff->size();
    for (size_t i(0); i < length; ++i)
    {
        buff->at(i) = (sin(*rads));
        *rads += step;
    }
}

template <typename T>
std::vector<T> sine(
        T freq,
        unsigned int numSamples,
        unsigned int samplerate,
        T* rads
    )
{
    std::vector<T> buff;
    buff.reserve(numSamples);
    sine<T>(&buff, freq, samplerate, rads);
    return buff;
}

template <typename T>
void visualize(
        std::vector<T> values,
        double width,
        double length,
        bool fill
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
RMS<T>::RMS() :
index(0)
{
}

template <typename T>
RMS<T>::RMS(unsigned int windowLengthInSamples) :
index(0)
{
    set_window(windowLengthInSamples);
}

template <typename T>
RMS<T>::~RMS()
{
}

template <typename T>
void RMS<T>::set_window(unsigned int numSamples)
{
    this->windowLength = numSamples;
    this->window.reserve(this->windowLength);
    for (unsigned int i(0); i < this->windowLength; ++i)
    {
        this->window.emplace_back(get_zero<T>());
    }
}

template <typename T>
inline void RMS<T>::set(T sample)
{
    this->window[index] = abs(sample);
    index = (index++ >= this->windowLength ? 0 : index);
}

template <typename T>
inline void RMS<T>::set(std::vector<T> samples)
{
    unsigned int length = (
            samples.size() <= this->windowLength
            ? samples.size() : this->windowLength
        );
    std::copy(samples.begin(), samples.begin() + length, this->window.begin());
}

template <typename T>
inline T RMS<T>::get()
{
    if (!this->window.size()) throw WINDOW_NOT_SET;
    return (
            std::accumulate(this->window.begin(), this->window.end(), 0)
            / this->windowLength
        );
}

template <typename T>
size_t RMS<T>::size()
{
    return this->windowLength;
}

template <typename T>
RMSdB<T>::RMSdB() :
RMS<T>()
{
}

template <typename T>
RMSdB<T>::RMSdB(unsigned int windowLengthInSamples) :
RMS<T>(windowLengthInSamples)
{
}

template <typename T>
inline T RMSdB<T>::get()
{
    T value = RMS<T>::get();
    if (!value) return -HUGE_VAL;
    else return 20 * log10(value);
}

template void clip_float(float& value);
template void clip_float(double& value);
template void clip_float(long double& value);

template float int_to_float(int8_t value);
template float int_to_float(uint8_t value);
template float int_to_float(int16_t value);
template float int_to_float(uint16_t value);
template float int_to_float(int32_t value);
template float int_to_float(uint32_t value);
template float int_to_float(int64_t value);
template float int_to_float(uint64_t value);

template double int_to_float(int8_t value);
template double int_to_float(uint8_t value);
template double int_to_float(int16_t value);
template double int_to_float(uint16_t value);
template double int_to_float(int32_t value);
template double int_to_float(uint32_t value);
template double int_to_float(int64_t value);
template double int_to_float(uint64_t value);

template long double int_to_float(int8_t value);
template long double int_to_float(uint8_t value);
template long double int_to_float(int16_t value);
template long double int_to_float(uint16_t value);
template long double int_to_float(int32_t value);
template long double int_to_float(uint32_t value);
template long double int_to_float(int64_t value);
template long double int_to_float(uint64_t value);

template int8_t float_to_int(float value);
template uint8_t float_to_int(float value);
template int16_t float_to_int(float value);
template uint16_t float_to_int(float value);
template int32_t float_to_int(float value);
template uint32_t float_to_int(float value);
template int64_t float_to_int(float value);
template uint64_t float_to_int(float value);

template int8_t float_to_int(double value);
template uint8_t float_to_int(double value);
template int16_t float_to_int(double value);
template uint16_t float_to_int(double value);
template int32_t float_to_int(double value);
template uint32_t float_to_int(double value);
template int64_t float_to_int(double value);
template uint64_t float_to_int(double value);

template int8_t float_to_int(long double value);
template uint8_t float_to_int(long double value);
template int16_t float_to_int(long double value);
template uint16_t float_to_int(long double value);
template int32_t float_to_int(long double value);
template uint32_t float_to_int(long double value);
template int64_t float_to_int(long double value);
template uint64_t float_to_int(long double value);

template void int_to_float(std::vector<float>* converted, std::vector<int8_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<uint8_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<int16_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<uint16_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<int32_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<uint32_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<int64_t>* values);
template void int_to_float(std::vector<float>* converted, std::vector<uint64_t>* values);

template void int_to_float(std::vector<double>* converted, std::vector<int8_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<uint8_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<int16_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<uint16_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<int32_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<uint32_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<int64_t>* values);
template void int_to_float(std::vector<double>* converted, std::vector<uint64_t>* values);

template void int_to_float(std::vector<long double>* converted, std::vector<int8_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<uint8_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<int16_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<uint16_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<int32_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<uint32_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<int64_t>* values);
template void int_to_float(std::vector<long double>* converted, std::vector<uint64_t>* values);

template void float_to_int(std::vector<int8_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<uint8_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<int16_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<uint16_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<int32_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<uint32_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<int64_t>* converted, std::vector<float>* values);
template void float_to_int(std::vector<uint64_t>* converted, std::vector<float>* values);

template void float_to_int(std::vector<int8_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<uint8_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<int16_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<uint16_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<int32_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<uint32_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<int64_t>* converted, std::vector<double>* values);
template void float_to_int(std::vector<uint64_t>* converted, std::vector<double>* values);

template void float_to_int(std::vector<int8_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<uint8_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<int16_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<uint16_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<int32_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<uint32_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<int64_t>* converted, std::vector<long double>* values);
template void float_to_int(std::vector<uint64_t>* converted, std::vector<long double>* values);

template std::vector<float> int_to_float(std::vector<int8_t> values);
template std::vector<float> int_to_float(std::vector<uint8_t> values);
template std::vector<float> int_to_float(std::vector<int16_t> values);
template std::vector<float> int_to_float(std::vector<uint16_t> values);
template std::vector<float> int_to_float(std::vector<int32_t> values);
template std::vector<float> int_to_float(std::vector<uint32_t> values);
template std::vector<float> int_to_float(std::vector<int64_t> values);
template std::vector<float> int_to_float(std::vector<uint64_t> values);

template std::vector<double> int_to_float(std::vector<int8_t> values);
template std::vector<double> int_to_float(std::vector<uint8_t> values);
template std::vector<double> int_to_float(std::vector<int16_t> values);
template std::vector<double> int_to_float(std::vector<uint16_t> values);
template std::vector<double> int_to_float(std::vector<int32_t> values);
template std::vector<double> int_to_float(std::vector<uint32_t> values);
template std::vector<double> int_to_float(std::vector<int64_t> values);
template std::vector<double> int_to_float(std::vector<uint64_t> values);

template std::vector<long double> int_to_float(std::vector<int8_t> values);
template std::vector<long double> int_to_float(std::vector<uint8_t> values);
template std::vector<long double> int_to_float(std::vector<int16_t> values);
template std::vector<long double> int_to_float(std::vector<uint16_t> values);
template std::vector<long double> int_to_float(std::vector<int32_t> values);
template std::vector<long double> int_to_float(std::vector<uint32_t> values);
template std::vector<long double> int_to_float(std::vector<int64_t> values);
template std::vector<long double> int_to_float(std::vector<uint64_t> values);

template std::vector<int8_t> float_to_int(std::vector<float> values);
template std::vector<uint8_t> float_to_int(std::vector<float> values);
template std::vector<int16_t> float_to_int(std::vector<float> values);
template std::vector<uint16_t> float_to_int(std::vector<float> values);
template std::vector<int32_t> float_to_int(std::vector<float> values);
template std::vector<uint32_t> float_to_int(std::vector<float> values);
template std::vector<int64_t> float_to_int(std::vector<float> values);
template std::vector<uint64_t> float_to_int(std::vector<float> values);

template std::vector<int8_t> float_to_int(std::vector<double> values);
template std::vector<uint8_t> float_to_int(std::vector<double> values);
template std::vector<int16_t> float_to_int(std::vector<double> values);
template std::vector<uint16_t> float_to_int(std::vector<double> values);
template std::vector<int32_t> float_to_int(std::vector<double> values);
template std::vector<uint32_t> float_to_int(std::vector<double> values);
template std::vector<int64_t> float_to_int(std::vector<double> values);
template std::vector<uint64_t> float_to_int(std::vector<double> values);

template std::vector<int8_t> float_to_int(std::vector<long double> values);
template std::vector<uint8_t> float_to_int(std::vector<long double> values);
template std::vector<int16_t> float_to_int(std::vector<long double> values);
template std::vector<uint16_t> float_to_int(std::vector<long double> values);
template std::vector<int32_t> float_to_int(std::vector<long double> values);
template std::vector<uint32_t> float_to_int(std::vector<long double> values);
template std::vector<int64_t> float_to_int(std::vector<long double> values);
template std::vector<uint64_t> float_to_int(std::vector<long double> values);

template int8_t get_radians<int8_t>(int8_t);
template uint8_t get_radians<uint8_t>(uint8_t);
template int16_t get_radians<int16_t>(int16_t);
template uint16_t get_radians<uint16_t>(uint16_t);
template int32_t get_radians<int32_t>(int32_t);
template uint32_t get_radians<uint32_t>(uint32_t);
template float get_radians<float>(float);
template double get_radians<double>(double);

template float get_decibels<float>(float);
template double get_decibels<double>(double);
template long double get_decibels<long double>(long double);

template float get_abs_decibels<float>(float);
template double get_abs_decibels<double>(double);
template long double get_abs_decibels<long double>(long double);

template std::vector<int8_t> hann_window<std::vector<int8_t>>(std::vector<int8_t>, int);
template std::vector<uint8_t> hann_window<std::vector<uint8_t>>(std::vector<uint8_t>, int);
template std::vector<int16_t> hann_window<std::vector<int16_t>>(std::vector<int16_t>, int);
template std::vector<uint16_t> hann_window<std::vector<uint16_t>>(std::vector<uint16_t>, int);
template std::vector<int32_t> hann_window<std::vector<int32_t>>(std::vector<int32_t>, int);
template std::vector<uint32_t> hann_window<std::vector<uint32_t>>(std::vector<uint32_t>, int);
template std::vector<float> hann_window<std::vector<float>>(std::vector<float>, int);
template std::vector<double> hann_window<std::vector<double>>(std::vector<double>, int);

template void visualize<int8_t>(std::vector<int8_t>, double, double, bool);
template void visualize<uint8_t>(std::vector<uint8_t>, double, double, bool);
template void visualize<int16_t>(std::vector<int16_t>, double, double, bool);
template void visualize<uint16_t>(std::vector<uint16_t>, double, double, bool);
template void visualize<int32_t>(std::vector<int32_t>, double, double, bool);
template void visualize<uint32_t>(std::vector<uint32_t>, double, double, bool);
template void visualize<float>(std::vector<float>, double, double, bool);
template void visualize<double>(std::vector<double>, double, double, bool);

template void sine<float>(std::vector<float>*, float, unsigned int, float*);
template void sine<double>(std::vector<double>*, double, unsigned int, double*);
template void sine<long double>(std::vector<long double>*, long double, unsigned int, long double*);

template std::vector<float> sine<float>(float, unsigned int, unsigned int, float*);
template std::vector<double> sine<double>(double, unsigned int, unsigned int, double*);
template std::vector<long double> sine<long double>(long double, unsigned int, unsigned int, long double*);

template class RMS<float>;
template class RMS<double>;
template class RMS<long double>;

template class RMSdB<float>;
template class RMSdB<double>;
template class RMSdB<long double>;