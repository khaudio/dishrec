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

    static const int_audio& max()
    {
        static constexpr int_audio maximum(8388607);
        return maximum;
    }

    static const int_audio& min()
    {
        static constexpr int_audio minimum(-8388608);
        return minimum;
    }

    inline operator int8_t() const
    {
        return static_cast<int8_t>(this->data);
    }

    inline operator uint8_t() const
    {
        return static_cast<uint8_t>(this->data);
    }

    inline operator int16_t() const
    {
        return static_cast<int16_t>(this->data);
    }

    inline operator uint16_t() const
    {
        return static_cast<uint16_t>(this->data);
    }

    inline operator int32_t() const
    {
        return static_cast<int32_t>(this->data);
    }

    inline operator uint32_t() const
    {
        return static_cast<uint32_t>(this->data);
    }

    inline operator int64_t() const
    {
        return static_cast<int64_t>(this->data);
    }

    inline operator uint64_t() const
    {
        return static_cast<uint64_t>(this->data);
    }

    inline operator float() const
    {
        return static_cast<float>(this->data);
    }

    inline operator double() const
    {
        return static_cast<double>(this->data);
    }

    inline operator long double() const
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

template <>
class std::numeric_limits<int_audio>
{
public:
    static constexpr int_audio max()
    {
        return int_audio(8388607);
    };

    static constexpr int_audio min()
    {
        return int_audio(-8388608);
    };

    static constexpr int_audio is_integer()
    {
        return true;
    };
};

#endif
