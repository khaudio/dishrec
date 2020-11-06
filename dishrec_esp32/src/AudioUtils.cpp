#include "AudioUtils.h"

template <typename T>
inline void clip_float(T* value)
{
    *value = (
            *value > 1.0
            ? (*value < -1.0 ? -1.0 : *value)
            : *value
        );
}

template <typename I, typename F>
inline F int_to_float(I value)
{
    F converted;
    if (std::is_signed<I>())
    {
        converted = ((value >= 0)
                ? (static_cast<F>(value) / std::numeric_limits<I>::max())
                : -(static_cast<F>(value) / std::numeric_limits<I>::min())
            );
    }
    else
    {
        I zero = static_cast<F>(get_zero<I>());
        converted = ((value >= zero)
                ? ((static_cast<F>(value) / zero) - zero)
                : ((static_cast<F>(value) - zero) / zero)
            );
    }
    clip_float(&converted);
    return converted;
}

template <typename F, typename I>
inline I float_to_int(F value)
{
    constexpr I minimum(std::numeric_limits<I>::min());
    constexpr I maximum(std::numeric_limits<I>::max());
    if (std::is_signed<I>())
    {
        if (value == F(1.0)) return maximum;
        else if (value == F(-1.0)) return minimum;
        else return std::round(value * (
                value >= 0
                ? maximum
                : -static_cast<F>(minimum)
            ));
    }
    else
    {
        if (I(value) == I(1)) return maximum;
        else if (value == -1) return minimum;
        else
        {
            constexpr I zero = get_zero<I>();
            return std::round(value * ((value >= 0) ? (zero - 1) : zero) + zero);
        }
    }
}

template <typename F, typename I>
inline I float_to_int(F value, I minimum, I maximum)
{
    if (std::is_signed<I>())
    {
        if (value == F(1.0)) return maximum;
        else if (value == F(-1.0)) return minimum;
        else return std::round(value * (
                value >= 0
                ? maximum
                : -static_cast<F>(minimum)
            ));
    }
    else
    {
        if (I(value) == I(1)) return maximum;
        else if (value == -1) return minimum;
        else
        {
            constexpr I zero = get_zero<I>();
            return std::round(value * ((value >= 0) ? (zero - 1) : zero) + zero);
        }
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

template <typename F, typename I>
inline void float_to_int(std::vector<I>* converted, std::vector<F>* values, I minimum, I maximum)
{
    size_t length = converted->size();
    for (size_t i(0); i < length; ++i)
    {
        converted->at(i) = float_to_int<F, I>(values->at(i), minimum, maximum);
    }
}

template <typename T>
inline void pack_data(uint8_t* packed, T* padded, int width)
{
    #ifdef _DEBUG
    constexpr int maxsize(sizeof(int_fast32_t));
    if (!width || (width > maxsize))
    {
        char message[128];
        sprintf(message, "Width must be 0 < width < %d", maxsize);
    }
    #endif

    int_audio sample(*padded);
    for (int i(0); i < width; ++i) packed[i] |= sample._data[i];
}

template <typename T>
inline void unpack_data(T* padded, uint8_t* packed, int width)
{
    #ifdef _DEBUG
    constexpr int maxsize(sizeof(int_fast32_t));
    if (!width || (width > maxsize))
    {
        char message[128];
        sprintf(message, "Width must be 0 < width < %d", maxsize);
    }
    #endif
    
    int_audio sample(0);
    for (int i(0); i < width; ++i) sample._data[i] |= packed[i];
    *padded = sample.data;
}

template <typename T>
inline T get_radians(T degrees)
{
    return degrees * (M_PIl / 180);
}

template <typename T>
inline T get_decibels(T floatValue)
{
    /* Get decibels in dBFS from floating point value
    in range 0.0 to 1.0 */
    return 20 * log10(floatValue);
}

template <typename T>
inline int sgn(T value)
{
    constexpr T zero(0);
    return ((std::signbit(value) * -1) + (value > zero));
}

template <>
inline int sgn(int_audio value)
{
    const int_audio zero(0);
    return (((value < zero) * -1) + (value > zero));
}

template <typename T>
inline int16_t convert_loudness_to_int(T value)
{
    T scaled = value * 100;
    return (scaled + sgn<T>(scaled) * .5);
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

void get_random_str(
        char* buff,
        size_t length,
        const char* validChars,
        unsigned int seed
    )
{
    srand(time(NULL) + seed);
    size_t available(std::strlen(validChars));
    for (size_t i(0); i < length; ++i)
    {
        buff[i] = validChars[(rand() % available)];
    }
    buff[length] = '\0';
}


template <typename T>
T get_max(uint8_t* values, size_t numBytes, int width)
{
    T maximum(std::numeric_limits<T>::min());
    for (size_t i(0); i < numBytes; i += width)
    {
        T unpacked(0);
        unpack_data<T>(&unpacked, &(values[i]), width);
        maximum = (unpacked > maximum) ? unpacked : maximum;
    }
    return maximum;
}

template <>
int_audio get_max(uint8_t* values, size_t numBytes, int width)
{
    int_audio maximum;
    maximum = int_audio::min();
    for (size_t i(0); i < numBytes; i += width)
    {
        int_audio unpacked(0);
        unpack_data<int_audio>(&unpacked, &(values[i]), width);
        maximum = (unpacked > maximum) ? unpacked : maximum;
    }
    return maximum;
}

template <typename T>
T get_min(uint8_t* values, size_t numBytes, int width)
{
    T minimum(std::numeric_limits<T>::max());
    for (size_t i(0); i < numBytes; i += width)
    {
        T unpacked(0);
        unpack_data<T>(&unpacked, &(values[i]), width);
        minimum = (unpacked < minimum) ? unpacked : minimum;
    }
    return minimum;
}

template <>
int_audio get_min(uint8_t* values, size_t numBytes, int width)
{
    int_audio minimum;
    minimum = int_audio::max();
    for (size_t i(0); i < numBytes; i += width)
    {
        int_audio unpacked(0);
        unpack_data<int_audio>(&unpacked, &(values[i]), width);
        minimum = (unpacked < minimum) ? unpacked : minimum;
    }
    return minimum;
}

template <typename T>
void visualize(
        std::vector<T> values,
        double length,
        double lineWidth,
        bool fill
    )
{
    double maximum(*std::max_element(values.begin(), values.end()));
    double minimum(*std::min_element(values.begin(), values.end()));
    double range = (maximum - minimum);
    double scale = lineWidth / range;
    double numItems = static_cast<double>(values.size());
    double increment = (numItems / (length > 0 ? length : numItems));
    double difference = lineWidth - (maximum * scale);
    for (double i = 0; i < numItems; i += increment)
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
void visualize(
        uint8_t* values,
        size_t numBytes,
        int sampleWidth,
        int lineWidth,
        bool fill
    )
{
    double maximum(get_max<T>(values, numBytes, sampleWidth));
    double minimum(get_min<T>(values, numBytes, sampleWidth));
    double range = (maximum - minimum);
    double scale = lineWidth / range;
    double difference = lineWidth - (maximum * scale);
    for (size_t i = 0; i < numBytes; i += sampleWidth)
    {
        T unpacked;
        unpack_data<T>(&unpacked, &values[i], sampleWidth);
        int value = (static_cast<double>(unpacked) * scale) + difference;
        for (int j = 0; j < value; ++j)
        {
            std::cout << (fill ? "-" : " ");
        }
        std::cout << "\u2022" << " " << +unpacked << std::endl;
    }
}

void print(const uint8_t* buff, size_t buffsize)
{
    for (size_t i(0); i < buffsize; ++i) std::cout << buff[i];
    std::cout << std::endl;
}

void print_hex(const uint8_t* buff, size_t buffsize)
{
    for (size_t i(0); i < buffsize; ++i)
    {
        std::cout << std::setw(2) << std::setfill('0');
        std::cout << std::hex << +buff[i] << " ";
    }
    std::cout << std::endl;
}

template void clip_float(float*);
template void clip_float(double*);
template void clip_float(long double*);

template float int_to_float(int8_t);
template float int_to_float(uint8_t);
template float int_to_float(int16_t);
template float int_to_float(uint16_t);
template float int_to_float(int32_t);
template float int_to_float(uint32_t);
template float int_to_float(int64_t);
template float int_to_float(uint64_t);
// template float int_to_float(int_audio);

template double int_to_float(int8_t);
template double int_to_float(uint8_t);
template double int_to_float(int16_t);
template double int_to_float(uint16_t);
template double int_to_float(int32_t);
template double int_to_float(uint32_t);
template double int_to_float(int64_t);
template double int_to_float(uint64_t);
// template double int_to_float(int_audio);

template long double int_to_float(int8_t);
template long double int_to_float(uint8_t);
template long double int_to_float(int16_t);
template long double int_to_float(uint16_t);
template long double int_to_float(int32_t);
template long double int_to_float(uint32_t);
template long double int_to_float(int64_t);
template long double int_to_float(uint64_t);
// template long double int_to_float(int_audio);

template int8_t float_to_int(float);
template uint8_t float_to_int(float);
template int16_t float_to_int(float);
template uint16_t float_to_int(float);
template int32_t float_to_int(float);
template uint32_t float_to_int(float);
template int64_t float_to_int(float);
template uint64_t float_to_int(float);
// template int_audio float_to_int(float);

template int8_t float_to_int(double);
template uint8_t float_to_int(double);
template int16_t float_to_int(double);
template uint16_t float_to_int(double);
template int32_t float_to_int(double);
template uint32_t float_to_int(double);
template int64_t float_to_int(double);
template uint64_t float_to_int(double);
// template int_audio float_to_int(double);

template int8_t float_to_int(long double);
template uint8_t float_to_int(long double);
template int16_t float_to_int(long double);
template uint16_t float_to_int(long double);
template int32_t float_to_int(long double);
template uint32_t float_to_int(long double);
template int64_t float_to_int(long double);
template uint64_t float_to_int(long double);
// template int_audio float_to_int(long double);

template int8_t float_to_int(float, int8_t, int8_t);
template uint8_t float_to_int(float, uint8_t, uint8_t);
template int16_t float_to_int(float, int16_t, int16_t);
template uint16_t float_to_int(float, uint16_t, uint16_t);
template int32_t float_to_int(float, int32_t, int32_t);
template uint32_t float_to_int(float, uint32_t, uint32_t);
template int64_t float_to_int(float, int64_t, int64_t);
template uint64_t float_to_int(float, uint64_t, uint64_t);
// template int_audio float_to_int(float, int_audio, int_audio);

template int8_t float_to_int(double, int8_t, int8_t);
template uint8_t float_to_int(double, uint8_t, uint8_t);
template int16_t float_to_int(double, int16_t, int16_t);
template uint16_t float_to_int(double, uint16_t, uint16_t);
template int32_t float_to_int(double, int32_t, int32_t);
template uint32_t float_to_int(double, uint32_t, uint32_t);
template int64_t float_to_int(double, int64_t, int64_t);
template uint64_t float_to_int(double, uint64_t, uint64_t);
// template int_audio float_to_int(double, int_audio, int_audio);

template int8_t float_to_int(long double, int8_t, int8_t);
template uint8_t float_to_int(long double, uint8_t, uint8_t);
template int16_t float_to_int(long double, int16_t, int16_t);
template uint16_t float_to_int(long double, uint16_t, uint16_t);
template int32_t float_to_int(long double, int32_t, int32_t);
template uint32_t float_to_int(long double, uint32_t, uint32_t);
template int64_t float_to_int(long double, int64_t, int64_t);
template uint64_t float_to_int(long double, uint64_t, uint64_t);
// template int_audio float_to_int(long double, int_audio, int_audio);

template void int_to_float(std::vector<float>*, std::vector<int8_t>*);
template void int_to_float(std::vector<float>*, std::vector<uint8_t>*);
template void int_to_float(std::vector<float>*, std::vector<int16_t>*);
template void int_to_float(std::vector<float>*, std::vector<uint16_t>*);
template void int_to_float(std::vector<float>*, std::vector<int32_t>*);
template void int_to_float(std::vector<float>*, std::vector<uint32_t>*);
template void int_to_float(std::vector<float>*, std::vector<int64_t>*);
template void int_to_float(std::vector<float>*, std::vector<uint64_t>*);
// template void int_to_float(std::vector<float>*, std::vector<int_audio>*);

template void int_to_float(std::vector<double>*, std::vector<int8_t>*);
template void int_to_float(std::vector<double>*, std::vector<uint8_t>*);
template void int_to_float(std::vector<double>*, std::vector<int16_t>*);
template void int_to_float(std::vector<double>*, std::vector<uint16_t>*);
template void int_to_float(std::vector<double>*, std::vector<int32_t>*);
template void int_to_float(std::vector<double>*, std::vector<uint32_t>*);
template void int_to_float(std::vector<double>*, std::vector<int64_t>*);
template void int_to_float(std::vector<double>*, std::vector<uint64_t>*);
// template void int_to_float(std::vector<double>*, std::vector<int_audio>*);

template void int_to_float(std::vector<long double>*, std::vector<int8_t>*);
template void int_to_float(std::vector<long double>*, std::vector<uint8_t>*);
template void int_to_float(std::vector<long double>*, std::vector<int16_t>*);
template void int_to_float(std::vector<long double>*, std::vector<uint16_t>*);
template void int_to_float(std::vector<long double>*, std::vector<int32_t>*);
template void int_to_float(std::vector<long double>*, std::vector<uint32_t>*);
template void int_to_float(std::vector<long double>*, std::vector<int64_t>*);
template void int_to_float(std::vector<long double>*, std::vector<uint64_t>*);
// template void int_to_float(std::vector<long double>*, std::vector<int_audio>*);

template void float_to_int(std::vector<int8_t>*, std::vector<float>*);
template void float_to_int(std::vector<uint8_t>*, std::vector<float>*);
template void float_to_int(std::vector<int16_t>*, std::vector<float>*);
template void float_to_int(std::vector<uint16_t>*, std::vector<float>*);
template void float_to_int(std::vector<int32_t>*, std::vector<float>*);
template void float_to_int(std::vector<uint32_t>*, std::vector<float>*);
template void float_to_int(std::vector<int64_t>*, std::vector<float>*);
template void float_to_int(std::vector<uint64_t>*, std::vector<float>*);
// template void float_to_int(std::vector<int_audio>*, std::vector<float>*);

template void float_to_int(std::vector<int8_t>*, std::vector<double>*);
template void float_to_int(std::vector<uint8_t>*, std::vector<double>*);
template void float_to_int(std::vector<int16_t>*, std::vector<double>*);
template void float_to_int(std::vector<uint16_t>*, std::vector<double>*);
template void float_to_int(std::vector<int32_t>*, std::vector<double>*);
template void float_to_int(std::vector<uint32_t>*, std::vector<double>*);
template void float_to_int(std::vector<int64_t>*, std::vector<double>*);
template void float_to_int(std::vector<uint64_t>*, std::vector<double>*);
// template void float_to_int(std::vector<int_audio>*, std::vector<double>*);

template void float_to_int(std::vector<int8_t>*, std::vector<long double>*);
template void float_to_int(std::vector<uint8_t>*, std::vector<long double>*);
template void float_to_int(std::vector<int16_t>*, std::vector<long double>*);
template void float_to_int(std::vector<uint16_t>*, std::vector<long double>*);
template void float_to_int(std::vector<int32_t>*, std::vector<long double>*);
template void float_to_int(std::vector<uint32_t>*, std::vector<long double>*);
template void float_to_int(std::vector<int64_t>*, std::vector<long double>*);
template void float_to_int(std::vector<uint64_t>*, std::vector<long double>*);
// template void float_to_int(std::vector<int_audio>*, std::vector<long double>*);

template void float_to_int(std::vector<int8_t>*, std::vector<float>*, int8_t, int8_t);
template void float_to_int(std::vector<uint8_t>*, std::vector<float>*, uint8_t, uint8_t);
template void float_to_int(std::vector<int16_t>*, std::vector<float>*, int16_t, int16_t);
template void float_to_int(std::vector<uint16_t>*, std::vector<float>*, uint16_t, uint16_t);
template void float_to_int(std::vector<int32_t>*, std::vector<float>*, int32_t, int32_t);
template void float_to_int(std::vector<uint32_t>*, std::vector<float>*, uint32_t, uint32_t);
template void float_to_int(std::vector<int64_t>*, std::vector<float>*, int64_t, int64_t);
template void float_to_int(std::vector<uint64_t>*, std::vector<float>*, uint64_t, uint64_t);
// template void float_to_int(std::vector<int_audio>*, std::vector<float>*, int_audio, int_audio);

template void float_to_int(std::vector<int8_t>*, std::vector<double>*, int8_t, int8_t);
template void float_to_int(std::vector<uint8_t>*, std::vector<double>*, uint8_t, uint8_t);
template void float_to_int(std::vector<int16_t>*, std::vector<double>*, int16_t, int16_t);
template void float_to_int(std::vector<uint16_t>*, std::vector<double>*, uint16_t, uint16_t);
template void float_to_int(std::vector<int32_t>*, std::vector<double>*, int32_t, int32_t);
template void float_to_int(std::vector<uint32_t>*, std::vector<double>*, uint32_t, uint32_t);
template void float_to_int(std::vector<int64_t>*, std::vector<double>*, int64_t, int64_t);
template void float_to_int(std::vector<uint64_t>*, std::vector<double>*, uint64_t, uint64_t);
// template void float_to_int(std::vector<int_audio>*, std::vector<double>*, int_audio, int_audio);

template void float_to_int(std::vector<int8_t>*, std::vector<long double>*, int8_t, int8_t);
template void float_to_int(std::vector<uint8_t>*, std::vector<long double>*, uint8_t, uint8_t);
template void float_to_int(std::vector<int16_t>*, std::vector<long double>*, int16_t, int16_t);
template void float_to_int(std::vector<uint16_t>*, std::vector<long double>*, uint16_t, uint16_t);
template void float_to_int(std::vector<int32_t>*, std::vector<long double>*, int32_t, int32_t);
template void float_to_int(std::vector<uint32_t>*, std::vector<long double>*, uint32_t, uint32_t);
template void float_to_int(std::vector<int64_t>*, std::vector<long double>*, int64_t, int64_t);
template void float_to_int(std::vector<uint64_t>*, std::vector<long double>*, uint64_t, uint64_t);
// template void float_to_int(std::vector<int_audio>*, std::vector<long double>*, int_audio, int_audio);

template void pack_data<int8_t>(uint8_t*, int8_t*, int);
template void pack_data<uint8_t>(uint8_t*, uint8_t*, int);
template void pack_data<int16_t>(uint8_t*, int16_t*, int);
template void pack_data<uint16_t>(uint8_t*, uint16_t*, int);
template void pack_data<int32_t>(uint8_t*, int32_t*, int);
template void pack_data<uint32_t>(uint8_t*, uint32_t*, int);
template void pack_data<int64_t>(uint8_t*, int64_t*, int);
template void pack_data<uint64_t>(uint8_t*, uint64_t*, int);
template void pack_data<int_audio>(uint8_t*, int_audio*, int);

template void unpack_data<int8_t>(int8_t*, uint8_t*, int);
template void unpack_data<uint8_t>(uint8_t*, uint8_t*, int);
template void unpack_data<int16_t>(int16_t*, uint8_t*, int);
template void unpack_data<uint16_t>(uint16_t*, uint8_t*, int);
template void unpack_data<int32_t>(int32_t*, uint8_t*, int);
template void unpack_data<uint32_t>(uint32_t*, uint8_t*, int);
template void unpack_data<int64_t>(int64_t*, uint8_t*, int);
template void unpack_data<uint64_t>(uint64_t*, uint8_t*, int);
template void unpack_data<int_audio>(int_audio*, uint8_t*, int);

template int8_t get_radians<int8_t>(int8_t);
template uint8_t get_radians<uint8_t>(uint8_t);
template int16_t get_radians<int16_t>(int16_t);
template uint16_t get_radians<uint16_t>(uint16_t);
template int32_t get_radians<int32_t>(int32_t);
template uint32_t get_radians<uint32_t>(uint32_t);
template float get_radians<float>(float);
template double get_radians<double>(double);
template long double get_radians<long double>(long double);
// template int_audio get_radians<int_audio>(int_audio);

template float get_decibels<float>(float);
template double get_decibels<double>(double);
template long double get_decibels<long double>(long double);

template int sgn(float);
template int sgn(double);
template int sgn(long double);
template int sgn(int8_t);
template int sgn(int16_t);
template int sgn(int32_t);
template int sgn(int64_t);
template int sgn(int_audio);

template int16_t convert_loudness_to_int(float);
template int16_t convert_loudness_to_int(double);
template int16_t convert_loudness_to_int(long double);

template std::vector<int8_t> hann_window<std::vector<int8_t>>(std::vector<int8_t>, int);
template std::vector<uint8_t> hann_window<std::vector<uint8_t>>(std::vector<uint8_t>, int);
template std::vector<int16_t> hann_window<std::vector<int16_t>>(std::vector<int16_t>, int);
template std::vector<uint16_t> hann_window<std::vector<uint16_t>>(std::vector<uint16_t>, int);
template std::vector<int32_t> hann_window<std::vector<int32_t>>(std::vector<int32_t>, int);
template std::vector<uint32_t> hann_window<std::vector<uint32_t>>(std::vector<uint32_t>, int);
template std::vector<float> hann_window<std::vector<float>>(std::vector<float>, int);
template std::vector<double> hann_window<std::vector<double>>(std::vector<double>, int);
template std::vector<long double> hann_window<std::vector<long double>>(std::vector<long double>, int);
// template std::vector<int_audio> hann_window<std::vector<int_audio>>(std::vector<int_audio>, int);

template int8_t get_max<int8_t>(uint8_t*, size_t, int);
template uint8_t get_max<uint8_t>(uint8_t*, size_t, int);
template int16_t get_max<int16_t>(uint8_t*, size_t, int);
template uint16_t get_max<uint16_t>(uint8_t*, size_t, int);
template int32_t get_max<int32_t>(uint8_t*, size_t, int);
template uint32_t get_max<uint32_t>(uint8_t*, size_t, int);
template int64_t get_max<int64_t>(uint8_t*, size_t, int);
template uint64_t get_max<uint64_t>(uint8_t*, size_t, int);
// template int_audio get_max<int_audio>(uint8_t*, size_t, int);

template int8_t get_min<int8_t>(uint8_t*, size_t, int);
template uint8_t get_min<uint8_t>(uint8_t*, size_t, int);
template int16_t get_min<int16_t>(uint8_t*, size_t, int);
template uint16_t get_min<uint16_t>(uint8_t*, size_t, int);
template int32_t get_min<int32_t>(uint8_t*, size_t, int);
template uint32_t get_min<uint32_t>(uint8_t*, size_t, int);
template int64_t get_min<int64_t>(uint8_t*, size_t, int);
template uint64_t get_min<uint64_t>(uint8_t*, size_t, int);
// template int_audio get_min<int_audio>(uint8_t*, size_t, int);

template void visualize<int8_t>(uint8_t*, size_t, int, int, bool);
template void visualize<uint8_t>(uint8_t*, size_t, int, int, bool);
template void visualize<int16_t>(uint8_t*, size_t, int, int, bool);
template void visualize<uint16_t>(uint8_t*, size_t, int, int, bool);
template void visualize<int32_t>(uint8_t*, size_t, int, int, bool);
template void visualize<uint32_t>(uint8_t*, size_t, int, int, bool);
template void visualize<int64_t>(uint8_t*, size_t, int, int, bool);
template void visualize<uint64_t>(uint8_t*, size_t, int, int, bool);
template void visualize<float>(uint8_t*, size_t, int, int, bool);
template void visualize<double>(uint8_t*, size_t, int, int, bool);
template void visualize<long double>(uint8_t*, size_t, int, int, bool);
// template void visualize<int_audio>(uint8_t*, size_t, int, int, bool);

template void visualize<int8_t>(std::vector<int8_t>, double, double, bool);
template void visualize<uint8_t>(std::vector<uint8_t>, double, double, bool);
template void visualize<int16_t>(std::vector<int16_t>, double, double, bool);
template void visualize<uint16_t>(std::vector<uint16_t>, double, double, bool);
template void visualize<int32_t>(std::vector<int32_t>, double, double, bool);
template void visualize<uint32_t>(std::vector<uint32_t>, double, double, bool);
template void visualize<int64_t>(std::vector<int64_t>, double, double, bool);
template void visualize<uint64_t>(std::vector<uint64_t>, double, double, bool);
template void visualize<float>(std::vector<float>, double, double, bool);
template void visualize<double>(std::vector<double>, double, double, bool);
template void visualize<long double>(std::vector<long double>, double, double, bool);
// template void visualize<int_audio>(std::vector<int_audio>, double, double, bool);

template void sine<float>(std::vector<float>*, float, unsigned int, float*);
template void sine<double>(std::vector<double>*, double, unsigned int, double*);
template void sine<long double>(std::vector<long double>*, long double, unsigned int, long double*);

template std::vector<float> sine<float>(float, unsigned int, unsigned int, float*);
template std::vector<double> sine<double>(double, unsigned int, unsigned int, double*);
template std::vector<long double> sine<long double>(long double, unsigned int, unsigned int, long double*);
