#ifndef INTAUDIO_H
#define INTAUDIO_H

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <iostream>

union int_audio
{
    int_fast32_t data : 24;
    int_fast32_t data24 : 24;
    int_fast32_t data16 : 16;
    uint_fast8_t data8 : 8;
    uint8_t _data[3];

    constexpr int_audio() :
    data(0)
    {
    }

    constexpr int_audio(const int_audio& obj) :
    data(obj.data)
    {
    }

    constexpr int_audio(const int_audio& obj, int value) :
    data(value)
    {
    }

    // Copy constructor hack to allow optional constexpr constructor
    constexpr int_audio(int value, int unused = 0) :
    int_audio(int_audio(), value)
    {
    }

    constexpr size_t size() const
    {
        return sizeof(_data);
    }

    constexpr uint16_t bit_depth() const
    {
        return size() * 8;
    }

    static constexpr const int_audio max()
    {
        return int_audio(8388607);
    }

    static constexpr const int_audio min()
    {
        return int_audio(-8388608);
    }

    explicit inline operator int8_t() const
    {
        return static_cast<int8_t>(this->data);
    }

    explicit inline operator uint8_t() const
    {
        return static_cast<uint8_t>(this->data);
    }

    explicit inline operator int16_t() const
    {
        return static_cast<int16_t>(this->data);
    }

    explicit inline operator uint16_t() const
    {
        return static_cast<uint16_t>(this->data);
    }

    explicit inline operator int32_t() const
    {
        return static_cast<int32_t>(this->data);
    }

    explicit inline operator uint32_t() const
    {
        return static_cast<uint32_t>(this->data);
    }

    explicit inline operator int64_t() const
    {
        return static_cast<int64_t>(this->data);
    }

    explicit inline operator uint64_t() const
    {
        return static_cast<uint64_t>(this->data);
    }

    explicit inline operator float() const
    {
        return static_cast<float>(this->data);
    }

    explicit inline operator double() const
    {
        return static_cast<double>(this->data);
    }

    explicit inline operator long double() const
    {
        return static_cast<long double>(this->data);
    }

    inline int_audio& operator=(const int_audio& other)
    {
        this->data = other.data;
        return *this;
    }

    inline int_audio operator+(const int_audio& other) const
    {
        return this->data + other.data;
    }

    inline int_audio operator+(const int8_t& other) const
    {
        return this->data8 + other;
    }

    inline int_audio operator+(const uint8_t& other) const
    {
        return this->data8 + other;
    }

    inline int_audio operator+(const int16_t& other) const
    {
        return this->data16 + other;
    }

    inline int_audio operator+(const uint16_t& other) const
    {
        return this->data16 + other;
    }

    inline int_audio operator+(const int32_t& other) const
    {
        return this->data + other;
    }

    inline int_audio operator+(const uint32_t& other) const
    {
        return this->data + other;
    }

    inline int_audio operator+(const int64_t& other) const
    {
        return static_cast<int64_t>(this->data) + other;
    }

    inline int_audio operator+(const uint64_t& other) const
    {
        return static_cast<uint64_t>(this->data) + other;
    }

    inline int_audio operator+(const float& other) const
    {
        /* Cast to float and truncate result */
        return static_cast<float>(this->data) + other;
    }

    inline int_audio operator+(const double& other) const
    {
        /* Cast to double and truncate result */
        return static_cast<double>(this->data) + other;
    }
        inline int_audio operator+(const long double& other) const
    {
        /* Cast to long double and truncate result */
        return static_cast<long double>(this->data) + other;
    }

    inline int_audio operator-(const int_audio& other) const
    {
        return this->data - other.data;
    }

    inline int_audio operator-(const int8_t& other) const
    {
        return this->data8 - other;
    }

    inline int_audio operator-(const uint8_t& other) const
    {
        return this->data8 - other;
    }

    inline int_audio operator-(const int16_t& other) const
    {
        return this->data16 - other;
    }

    inline int_audio operator-(const uint16_t& other) const
    {
        return this->data16 - other;
    }

    inline int_audio operator-(const int32_t& other) const
    {
        return this->data - other;
    }

    inline int_audio operator-(const uint32_t& other) const
    {
        return this->data - other;
    }

    inline int_audio operator-(const int64_t& other) const
    {
        return static_cast<int64_t>(this->data) - other;
    }

    inline int_audio operator-(const uint64_t& other) const
    {
        return static_cast<uint64_t>(this->data) - other;
    }

    inline int_audio operator-(const float& other) const
    {
        /* Cast to float and truncate result */
        return static_cast<float>(this->data) - other;
    }

    inline int_audio operator-(const double& other) const
    {
        /* Cast to double and truncate result */
        return static_cast<double>(this->data) - other;
    }

    inline int_audio operator-(const long double& other) const
    {
        /* Cast to long double and truncate result */
        return static_cast<long double>(this->data) - other;
    }

    inline int_audio operator*(const int_audio& other) const
    {
        return this->data * other.data;
    }

    inline int_audio operator*(const int8_t& other) const
    {
        return this->data * other;
    }

    inline int_audio operator*(const uint8_t& other) const
    {
        return this->data * other;
    }

    inline int_audio operator*(const int16_t& other) const
    {
        return this->data * other;
    }

    inline int_audio operator*(const uint16_t& other) const
    {
        return this->data * other;
    }

    inline int_audio operator*(const int32_t& other) const
    {
        return this->data * other;
    }

    inline int_audio operator*(const uint32_t& other) const
    {
        return this->data * other;
    }

    inline int_audio operator*(const int64_t& other) const
    {
        return static_cast<int64_t>(this->data) * other;
    }

    inline int_audio operator*(const uint64_t& other) const
    {
        return static_cast<uint64_t>(this->data) * other;
    }

    inline int_audio operator*(const float& value) const
    {
        /* Cast to float and truncate result */
        return static_cast<float>(this->data) * value;
    }

    inline int_audio operator*(const double& value) const
    {
        /* Cast to double and truncate result */
        return static_cast<double>(this->data) * value;
    }

    inline int_audio operator*(const long double& value) const
    {
        /* Cast to long double and truncate result */
        return static_cast<long double>(this->data) * value;
    }

    inline int_audio operator/(const int_audio& other) const
    {
        return this->data / other.data;
    }

    inline int_audio operator/(const int8_t& other) const
    {
        return this->data / other;
    }

    inline int_audio operator/(const uint8_t& other) const
    {
        return this->data / other;
    }

    inline int_audio operator/(const int16_t& other) const
    {
        return this->data / other;
    }

    inline int_audio operator/(const uint16_t& other) const
    {
        return this->data / other;
    }

    inline int_audio operator/(const int32_t& other) const
    {
        return this->data / other;
    }

    inline int_audio operator/(const uint32_t& other) const
    {
        return this->data / other;
    }

    inline int_audio operator/(const int64_t& other) const
    {
        return static_cast<int64_t>(this->data) / other;
    }

    inline int_audio operator/(const uint64_t& other) const
    {
        return static_cast<uint64_t>(this->data) / other;
    }

    inline int_audio operator/(const float& value) const
    {
        /* Cast to float and truncate result */
        return static_cast<float>(this->data) / value;
    }

    inline int_audio operator/(const double& value) const
    {
        /* Cast to double and truncate result */
        return static_cast<double>(this->data) / value;
    }

    inline int_audio operator/(const long double& value) const
    {
        /* Cast to long double and truncate result */
        return static_cast<long double>(this->data) / value;
    }

    inline int_audio operator%(const int_audio& value) const
    {
        return this->data % value.data;
    }

    inline int_audio operator%(const int8_t& value) const
    {
        return this->data % value;
    }

    inline int_audio operator%(const uint8_t& value) const
    {
        return this->data % value;
    }

    inline int_audio operator%(const int16_t& value) const
    {
        return this->data % value;
    }

    inline int_audio operator%(const uint16_t& value) const
    {
        return this->data % value;
    }

    inline int_audio operator%(const int32_t& value) const
    {
        return this->data % value;
    }

    inline int_audio operator%(const uint32_t& value) const
    {
        return this->data % value;
    }

    inline int_audio operator%(const int64_t& value) const
    {
        return static_cast<int64_t>(this->data) % value;
    }

    inline int_audio operator%(const uint64_t& value) const
    {
        return static_cast<uint64_t>(this->data) % value;
    }

    inline int_audio operator&(const int_audio& value) const
    {
        return this->data & value.data;
    }

    inline int_audio operator|(const int_audio& value) const
    {
        return this->data | value.data;
    }

    inline int_audio operator^(const int_audio& value) const
    {
        return this->data ^ value.data;
    }

    inline int_audio operator~() const
    {
        return ~(this->data);
    }

    inline int_audio operator<<(const int num) const
    {
        if (num > 0) return int_audio(this->data << num);
        else if (!num) return *this;
        else return *this >> -num;
    }

    inline int_audio operator>>(const int num) const
    {
        if (num > 0) return int_audio(this->data >> num);
        else if (!num) return *this;
        else return *this << -num;
    }

    inline int_audio& operator+=(const int_audio& value)
    {
        *this = (*this + value);
        return *this;
    }

    inline int_audio& operator-=(const int_audio& value)
    {
        *this = (*this - value);
        return *this;
    }

    inline int_audio& operator*=(const int_audio& value)
    {
        *this = (*this * value);
        return *this;
    }

    inline int_audio& operator*=(const float& value)
    {
        *this = (*this * value);
        return *this;
    }

    inline int_audio& operator*=(const double& value)
    {
        *this = (*this * value);
        return *this;
    }

    inline int_audio& operator*=(const long double& value)
    {
        *this = (*this * value);
        return *this;
    }

    inline int_audio& operator/=(const int_audio& value)
    {
        *this = (*this / value);
        return *this;
    }

    inline int_audio& operator/=(const float& value)
    {
        *this = (*this / value);
        return *this;
    }

    inline int_audio& operator/=(const double& value)
    {
        *this = (*this / value);
        return *this;
    }

    inline int_audio& operator/=(const long double& value)
    {
        *this = (*this / value);
        return *this;
    }

    inline int_audio& operator%=(const int_audio& value)
    {
        *this = (*this % value);
        return *this;
    }

    inline int_audio& operator&=(const int_audio& value)
    {
        *this = (*this & value);
        return *this;
    }

    inline int_audio& operator|=(const int_audio& value)
    {
        *this = (*this | value);
        return *this;
    }

    inline int_audio& operator^=(const int_audio& value)
    {
        *this = (*this ^ value);
        return *this;
    }

    inline int_audio& operator<<=(const int num)
    {
        *this = *this << num;
        return *this;
    }

    inline int_audio& operator>>=(const int num)
    {
        *this = *this >> num;
        return *this;
    }

    inline bool operator==(const int_audio& other) const
    {
        return this->data == other.data;
    }

    inline bool operator!=(const int_audio& other) const
    {
        return this->data != other.data;
    }

    inline bool operator>(const int_audio& other) const
    {
        return this->data > other.data;
    }

    inline bool operator>=(const int_audio& other) const
    {
        return this->data >= other.data;
    }

    inline bool operator<(const int_audio& other) const
    {
        return this->data < other.data;
    }

    inline bool operator<=(const int_audio& other) const
    {
        return this->data <= other.data;
    }

    inline const int_audio& operator++()
    {
        ++this->data;
        return *this;
    }

    inline int_audio operator++(int)
    {
        int_audio state(*this);
        ++this->data;
        return state;
    }

    inline const int_audio& operator--()
    {
        --this->data;
        return *this;
    }

    inline int_audio operator--(int)
    {
        int_audio state(*this);
        --this->data;
        return state;
    }

    inline const int_audio& operator+() const
    {
        return *this;
    }

    inline const int_audio operator-() const
    {
        return -(this->data);
    }

    inline bool operator!() const
    {
        return bool(this->data);
    }
};

inline std::ostream& operator<<(std::ostream& stream, const int_audio& value)
{
    stream << value.data;
    return stream;
}

inline std::istream& operator>>(const int_audio& value, std::istream& stream)
{
    value.data >> stream;
    return stream;
}

#ifndef ESP32
template <>
class std::numeric_limits<int_audio>
{
public:
    static constexpr int_audio max()
    {
        int_audio _obj;
        return int_audio(_obj, 8388607);
    };

    static constexpr int_audio min()
    {
        int_audio _obj;
        return int_audio(_obj, -8388608);
    };

    static constexpr bool is_integer()
    {
        return true;
    };
};
#endif

int8_t operator+(const int8_t i, const int_audio& a);
int8_t operator+(const int_audio& a, const int8_t i);
uint8_t operator+(const uint8_t i, const int_audio& a);
uint8_t operator+(const int_audio& a, const uint8_t i);
int16_t operator+(const int16_t i, const int_audio& a);
int16_t operator+(const int_audio& a, const int16_t i);
uint16_t operator+(const uint16_t i, const int_audio& a);
uint16_t operator+(const int_audio& a, const uint16_t i);
int32_t operator+(const int32_t i, const int_audio& a);
int32_t operator+(const int_audio& a, const int32_t i);
uint32_t operator+(const uint32_t i, const int_audio& a);
uint32_t operator+(const int_audio& a, const uint32_t i);
int64_t operator+(const int64_t i, const int_audio& a);
int64_t operator+(const int_audio& a, const int64_t i);
uint64_t operator+(const uint64_t i, const int_audio& a);
uint64_t operator+(const int_audio& a, const uint64_t i);
float operator+(const float i, const int_audio& a);
float operator+(const int_audio& a, const float i);
double operator+(const double i, const int_audio& a);
double operator+(const int_audio& a, const double i);
long double operator+(const long double i, const int_audio& a);
long double operator+(const int_audio& a, const long double i);

int8_t operator-(const int8_t i, const int_audio& a);
int8_t operator-(const int_audio& a, const int8_t i);
uint8_t operator-(const uint8_t i, const int_audio& a);
uint8_t operator-(const int_audio& a, const uint8_t i);
int16_t operator-(const int16_t i, const int_audio& a);
int16_t operator-(const int_audio& a, const int16_t i);
uint16_t operator-(const uint16_t i, const int_audio& a);
uint16_t operator-(const int_audio& a, const uint16_t i);
int32_t operator-(const int32_t i, const int_audio& a);
int32_t operator-(const int_audio& a, const int32_t i);
uint32_t operator-(const uint32_t i, const int_audio& a);
uint32_t operator-(const int_audio& a, const uint32_t i);
int64_t operator-(const int64_t i, const int_audio& a);
int64_t operator-(const int_audio& a, const int64_t i);
uint64_t operator-(const uint64_t i, const int_audio& a);
uint64_t operator-(const int_audio& a, const uint64_t i);
float operator-(const float i, const int_audio& a);
float operator-(const int_audio& a, const float i);
double operator-(const double i, const int_audio& a);
double operator-(const int_audio& a, const double i);
long double operator-(const long double i, const int_audio& a);
long double operator-(const int_audio& a, const long double i);

int8_t operator*(const int8_t i, const int_audio& a);
int8_t operator*(const int_audio& a, const int8_t i);
uint8_t operator*(const uint8_t i, const int_audio& a);
uint8_t operator*(const int_audio& a, const uint8_t i);
int16_t operator*(const int16_t i, const int_audio& a);
int16_t operator*(const int_audio& a, const int16_t i);
uint16_t operator*(const uint16_t i, const int_audio& a);
uint16_t operator*(const int_audio& a, const uint16_t i);
int32_t operator*(const int32_t i, const int_audio& a);
int32_t operator*(const int_audio& a, const int32_t i);
uint32_t operator*(const uint32_t i, const int_audio& a);
uint32_t operator*(const int_audio& a, const uint32_t i);
int64_t operator*(const int64_t i, const int_audio& a);
int64_t operator*(const int_audio& a, const int64_t i);
uint64_t operator*(const uint64_t i, const int_audio& a);
uint64_t operator*(const int_audio& a, const uint64_t i);
float operator*(const float i, const int_audio& a);
float operator*(const int_audio& a, const float i);
double operator*(const double i, const int_audio& a);
double operator*(const int_audio& a, const double i);
long double operator*(const long double i, const int_audio& a);
long double operator*(const int_audio& a, const long double i);

int8_t operator/(const int8_t i, const int_audio& a);
int8_t operator/(const int_audio& a, const int8_t i);
uint8_t operator/(const uint8_t i, const int_audio& a);
uint8_t operator/(const int_audio& a, const uint8_t i);
int16_t operator/(const int16_t i, const int_audio& a);
int16_t operator/(const int_audio& a, const int16_t i);
uint16_t operator/(const uint16_t i, const int_audio& a);
uint16_t operator/(const int_audio& a, const uint16_t i);
int32_t operator/(const int32_t i, const int_audio& a);
int32_t operator/(const int_audio& a, const int32_t i);
uint32_t operator/(const uint32_t i, const int_audio& a);
uint32_t operator/(const int_audio& a, const uint32_t i);
int64_t operator/(const int64_t i, const int_audio& a);
int64_t operator/(const int_audio& a, const int64_t i);
uint64_t operator/(const uint64_t i, const int_audio& a);
uint64_t operator/(const int_audio& a, const uint64_t i);
float operator/(const float i, const int_audio& a);
float operator/(const int_audio& a, const float i);
double operator/(const double i, const int_audio& a);
double operator/(const int_audio& a, const double i);
long double operator/(const long double i, const int_audio& a);
long double operator/(const int_audio& a, const long double i);

int8_t operator%(const int8_t i, const int_audio& a);
int8_t operator%(const int_audio& a, const int8_t i);
uint8_t operator%(const uint8_t i, const int_audio& a);
uint8_t operator%(const int_audio& a, const uint8_t i);
int16_t operator%(const int16_t i, const int_audio& a);
int16_t operator%(const int_audio& a, const int16_t i);
uint16_t operator%(const uint16_t i, const int_audio& a);
uint16_t operator%(const int_audio& a, const uint16_t i);
int32_t operator%(const int32_t i, const int_audio& a);
int32_t operator%(const int_audio& a, const int32_t i);
uint32_t operator%(const uint32_t i, const int_audio& a);
uint32_t operator%(const int_audio& a, const uint32_t i);
int64_t operator%(const int64_t i, const int_audio& a);
int64_t operator%(const int_audio& a, const int64_t i);
uint64_t operator%(const uint64_t i, const int_audio& a);
uint64_t operator%(const int_audio& a, const uint64_t i);

#endif
