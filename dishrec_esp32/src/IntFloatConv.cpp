#include "IntFloatConv.h"

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
inline std::vector<F> int_to_float(std::vector<I> values)
{
    size_t length = values.size();
    std::vector<F> converted;
    converted.reserve(length);
    for (size_t i(0); i < length; ++i)
    {
        converted.emplace_back(int_to_float<I, F>(values[i]));
    }
    return converted;
}

template <typename F, typename I>
inline std::vector<I> float_to_int(std::vector<F> values)
{
    size_t length = values.size();
    std::vector<I> converted;
    converted.reserve(length);
    for (size_t i(0); i < length; ++i)
    {
        converted.emplace_back(float_to_int<F, I>(values[i]));
    }
    return converted;
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
