#include "int_audio.h"

int8_t operator+(const int8_t i, const int_audio& a)
{
    return a.data + static_cast<int_fast32_t>(i);
}

int8_t operator+(const int_audio& a, const int8_t i)
{
    return a.data + static_cast<int_fast32_t>(i);
}

uint8_t operator+(const uint8_t i, const int_audio& a)
{
    return a.data + static_cast<uint_fast32_t>(i);
}

uint8_t operator+(const int_audio& a, const uint8_t i)
{
    return a.data + static_cast<uint_fast32_t>(i);
}

int16_t operator+(const int16_t i, const int_audio& a)
{
    return a.data + static_cast<int_fast32_t>(i);
}

int16_t operator+(const int_audio& a, const int16_t i)
{
    return a.data + static_cast<int_fast32_t>(i);
}

uint16_t operator+(const uint16_t i, const int_audio& a)
{
    return a.data + static_cast<uint_fast32_t>(i);
}

uint16_t operator+(const int_audio& a, const uint16_t i)
{
    return a.data + static_cast<uint_fast32_t>(i);
}

int32_t operator+(const int32_t i, const int_audio& a)
{
    return a.data + static_cast<int_fast32_t>(i);
}

int32_t operator+(const int_audio& a, const int32_t i)
{
    return a.data + static_cast<int_fast32_t>(i);
}

uint32_t operator+(const uint32_t i, const int_audio& a)
{
    return a.data + static_cast<uint_fast32_t>(i);
}

uint32_t operator+(const int_audio& a, const uint32_t i)
{
    return a.data + static_cast<uint_fast32_t>(i);
}

int64_t operator+(const int64_t i, const int_audio& a)
{
    return static_cast<int_fast64_t>(a.data) + i;
}

int64_t operator+(const int_audio& a, const int64_t i)
{
    return static_cast<int_fast64_t>(a.data) + i;
}

uint64_t operator+(const uint64_t i, const int_audio& a)
{
    return static_cast<uint_fast64_t>(a.data) + i;
}

uint64_t operator+(const int_audio& a, const uint64_t i)
{
    return static_cast<uint_fast64_t>(a.data) + i;
}

float operator+(const int_audio& a, const float i)
{
    return static_cast<float>(a.data) + i;
}

double operator+(const int_audio& a, const double i)
{
    return static_cast<double>(a.data) + i;
}
long double operator+(const int_audio& a, const long double i)
{
    return static_cast<long double>(a.data) + i;
}

int8_t operator-(const int8_t i, const int_audio& a)
{
    return a.data - static_cast<int_fast32_t>(i);
}

int8_t operator-(const int_audio& a, const int8_t i)
{
    return a.data - static_cast<int_fast32_t>(i);
}

uint8_t operator-(const uint8_t i, const int_audio& a)
{
    return a.data - static_cast<uint_fast32_t>(i);
}

uint8_t operator-(const int_audio& a, const uint8_t i)
{
    return a.data - static_cast<uint_fast32_t>(i);
}

int16_t operator-(const int16_t i, const int_audio& a)
{
    return a.data - static_cast<int_fast32_t>(i);
}

int16_t operator-(const int_audio& a, const int16_t i)
{
    return a.data - static_cast<int_fast32_t>(i);
}

uint16_t operator-(const uint16_t i, const int_audio& a)
{
    return a.data - static_cast<uint_fast32_t>(i);
}

uint16_t operator-(const int_audio& a, const uint16_t i)
{
    return a.data - static_cast<uint_fast32_t>(i);
}

int32_t operator-(const int32_t i, const int_audio& a)
{
    return a.data - static_cast<int_fast32_t>(i);
}

int32_t operator-(const int_audio& a, const int32_t i)
{
    return a.data - static_cast<int_fast32_t>(i);
}

uint32_t operator-(const uint32_t i, const int_audio& a)
{
    return a.data - static_cast<uint_fast32_t>(i);
}

uint32_t operator-(const int_audio& a, const uint32_t i)
{
    return a.data - static_cast<uint_fast32_t>(i);
}

int64_t operator-(const int64_t i, const int_audio& a)
{
    return static_cast<int_fast64_t>(a.data) - i;
}

int64_t operator-(const int_audio& a, const int64_t i)
{
    return static_cast<int_fast64_t>(a.data) - i;
}

uint64_t operator-(const uint64_t i, const int_audio& a)
{
    return static_cast<uint_fast64_t>(a.data) - i;
}

uint64_t operator-(const int_audio& a, const uint64_t i)
{
    return static_cast<uint_fast64_t>(a.data) - i;
}

float operator-(const int_audio& a, const float i)
{
    return static_cast<float>(a.data) - i;
}

double operator-(const int_audio& a, const double i)
{
    return static_cast<double>(a.data) - i;
}
long double operator-(const int_audio& a, const long double i)
{
    return static_cast<long double>(a.data) - i;
}

int8_t operator*(const int8_t i, const int_audio& a)
{
    return a.data * static_cast<int_fast32_t>(i);
}

int8_t operator*(const int_audio& a, const int8_t i)
{
    return a.data * static_cast<int_fast32_t>(i);
}

uint8_t operator*(const uint8_t i, const int_audio& a)
{
    return a.data * static_cast<uint_fast32_t>(i);
}

uint8_t operator*(const int_audio& a, const uint8_t i)
{
    return a.data * static_cast<uint_fast32_t>(i);
}

int16_t operator*(const int16_t i, const int_audio& a)
{
    return a.data * static_cast<int_fast32_t>(i);
}

int16_t operator*(const int_audio& a, const int16_t i)
{
    return a.data * static_cast<int_fast32_t>(i);
}

uint16_t operator*(const uint16_t i, const int_audio& a)
{
    return a.data * static_cast<uint_fast32_t>(i);
}

uint16_t operator*(const int_audio& a, const uint16_t i)
{
    return a.data * static_cast<uint_fast32_t>(i);
}

int32_t operator*(const int32_t i, const int_audio& a)
{
    return a.data * static_cast<int_fast32_t>(i);
}

int32_t operator*(const int_audio& a, const int32_t i)
{
    return a.data * static_cast<int_fast32_t>(i);
}

uint32_t operator*(const uint32_t i, const int_audio& a)
{
    return a.data * static_cast<uint_fast32_t>(i);
}

uint32_t operator*(const int_audio& a, const uint32_t i)
{
    return a.data * static_cast<uint_fast32_t>(i);
}

int64_t operator*(const int64_t i, const int_audio& a)
{
    return static_cast<int_fast64_t>(a.data) * i;
}

int64_t operator*(const int_audio& a, const int64_t i)
{
    return static_cast<int_fast64_t>(a.data) * i;
}

uint64_t operator*(const uint64_t i, const int_audio& a)
{
    return static_cast<uint_fast64_t>(a.data) * i;
}

uint64_t operator*(const int_audio& a, const uint64_t i)
{
    return static_cast<uint_fast64_t>(a.data) * i;
}

float operator*(const int_audio& a, const float i)
{
    return static_cast<float>(a.data) * i;
}

double operator*(const int_audio& a, const double i)
{
    return static_cast<double>(a.data) * i;
}
long double operator*(const int_audio& a, const long double i)
{
    return static_cast<long double>(a.data) * i;
}

int8_t operator/(const int8_t i, const int_audio& a)
{
    return a.data / static_cast<int_fast32_t>(i);
}

int8_t operator/(const int_audio& a, const int8_t i)
{
    return a.data / static_cast<int_fast32_t>(i);
}

uint8_t operator/(const uint8_t i, const int_audio& a)
{
    return a.data / static_cast<uint_fast32_t>(i);
}

uint8_t operator/(const int_audio& a, const uint8_t i)
{
    return a.data / static_cast<uint_fast32_t>(i);
}

int16_t operator/(const int16_t i, const int_audio& a)
{
    return a.data / static_cast<int_fast32_t>(i);
}

int16_t operator/(const int_audio& a, const int16_t i)
{
    return a.data / static_cast<int_fast32_t>(i);
}

uint16_t operator/(const uint16_t i, const int_audio& a)
{
    return a.data / static_cast<uint_fast32_t>(i);
}

uint16_t operator/(const int_audio& a, const uint16_t i)
{
    return a.data / static_cast<uint_fast32_t>(i);
}

int32_t operator/(const int32_t i, const int_audio& a)
{
    return a.data / static_cast<int_fast32_t>(i);
}

int32_t operator/(const int_audio& a, const int32_t i)
{
    return a.data / static_cast<int_fast32_t>(i);
}

uint32_t operator/(const uint32_t i, const int_audio& a)
{
    return a.data / static_cast<uint_fast32_t>(i);
}

uint32_t operator/(const int_audio& a, const uint32_t i)
{
    return a.data / static_cast<uint_fast32_t>(i);
}

int64_t operator/(const int64_t i, const int_audio& a)
{
    return static_cast<int_fast64_t>(a.data) / i;
}

int64_t operator/(const int_audio& a, const int64_t i)
{
    return static_cast<int_fast64_t>(a.data) / i;
}

uint64_t operator/(const uint64_t i, const int_audio& a)
{
    return static_cast<uint_fast64_t>(a.data) / i;
}

uint64_t operator/(const int_audio& a, const uint64_t i)
{
    return static_cast<uint_fast64_t>(a.data) / i;
}

float operator/(const int_audio& a, const float i)
{
    return static_cast<float>(a.data) / i;
}

double operator/(const int_audio& a, const double i)
{
    return static_cast<double>(a.data) / i;
}
long double operator/(const int_audio& a, const long double i)
{
    return static_cast<long double>(a.data) / i;
}

int8_t operator%(const int8_t i, const int_audio& a)
{
    return a.data % static_cast<int_fast32_t>(i);
}

int8_t operator%(const int_audio& a, const int8_t i)
{
    return a.data % static_cast<int_fast32_t>(i);
}

uint8_t operator%(const uint8_t i, const int_audio& a)
{
    return a.data % static_cast<uint_fast32_t>(i);
}

uint8_t operator%(const int_audio& a, const uint8_t i)
{
    return a.data % static_cast<uint_fast32_t>(i);
}

int16_t operator%(const int16_t i, const int_audio& a)
{
    return a.data % static_cast<int_fast32_t>(i);
}

int16_t operator%(const int_audio& a, const int16_t i)
{
    return a.data % static_cast<int_fast32_t>(i);
}

uint16_t operator%(const uint16_t i, const int_audio& a)
{
    return a.data % static_cast<uint_fast32_t>(i);
}

uint16_t operator%(const int_audio& a, const uint16_t i)
{
    return a.data % static_cast<uint_fast32_t>(i);
}

int32_t operator%(const int32_t i, const int_audio& a)
{
    return a.data % static_cast<int_fast32_t>(i);
}

int32_t operator%(const int_audio& a, const int32_t i)
{
    return a.data % static_cast<int_fast32_t>(i);
}

uint32_t operator%(const uint32_t i, const int_audio& a)
{
    return a.data % static_cast<uint_fast32_t>(i);
}

uint32_t operator%(const int_audio& a, const uint32_t i)
{
    return a.data % static_cast<uint_fast32_t>(i);
}

int64_t operator%(const int64_t i, const int_audio& a)
{
    return static_cast<int_fast64_t>(a.data) % i;
}

int64_t operator%(const int_audio& a, const int64_t i)
{
    return static_cast<int_fast64_t>(a.data) % i;
}

uint64_t operator%(const uint64_t i, const int_audio& a)
{
    return static_cast<uint_fast64_t>(a.data) % i;
}

uint64_t operator%(const int_audio& a, const uint64_t i)
{
    return static_cast<uint_fast64_t>(a.data) % i;
}
