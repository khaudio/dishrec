#include "int24_t.h"


int24_t::int24_t()
{
}

// template <typename T>
// int24_t::int24_t(T num)
// {
// }

int24_t& int24_t::operator=(const int24_t& value)
{
    this->_data[0] = value._data[0];
    this->_data[1] = value._data[1];
    this->_data[2] = value._data[2];
    return *this;
}

inline int24_t::int24_t(const int24_t& value)
{
    this->_data[0] = value._data[0];
    this->_data[1] = value._data[1];
    this->_data[2] = value._data[2];
}

int24_t::int24_t(const uint32_t value)
{
    this->_data[2] = static_cast<uint8_t>(value);
    this->_data[1] = static_cast<uint8_t>(value >> 8);
    this->_data[0] = static_cast<uint8_t>(value >> 16);
}

int24_t::int24_t(const int32_t value)
{
    operator=(int24_t(static_cast<uint32_t>(value)));
}

int24_t::int24_t(const int16_t value)
{
    operator=(int24_t(static_cast<int32_t>(value)));
}

int24_t::int24_t(const uint16_t value)
{
    operator=(int24_t(static_cast<uint32_t>(value)));
}

int24_t::~int24_t()
{
}

static const int24_t& zero(void)
{
    static const int24_t value(0);
    return value;
}

static const int24_t& one(void)
{
    static const int24_t value(1);
    return value;
}

static const int24_t& max(void)
{
    static const int24_t value(0x7FFFFF);
    return value;
}

static const int24_t& min(void)
{
    static const int24_t value(-0x800000);
    return value;
}

int24_t int24_t::operator+(const int24_t& value) const
{
    int24_t conv;
    uint16_t result[3];
    result[2] = (
            static_cast<uint16_t>(this->_data[2])
            + static_cast<uint16_t>(value._data[2])
        );
    result[1] = (
            static_cast<uint16_t>(this->_data[1])
            + static_cast<uint16_t>(value._data[1])
            + static_cast<uint16_t>(result[2] >> 8)
        );
    result[0] = (
            static_cast<uint16_t>(this->_data[0])
            + static_cast<uint16_t>(value._data[0])
            + static_cast<uint16_t>(result[1] >> 8)
        );
    conv._data[0] = static_cast<uint8_t>(result[0]);
    conv._data[1] = static_cast<uint8_t>(result[1]);
    conv._data[2] = static_cast<uint8_t>(result[2]);
    return conv;
}

int24_t int24_t::operator-(const int24_t& value) const
{
    int24_t conv;
    uint16_t result[3];
    result[2] = (
            static_cast<uint16_t>(this->_data[2])
            - static_cast<uint16_t>(value._data[2])
        );
    result[1] = (
            static_cast<uint16_t>(this->_data[1])
            - static_cast<uint16_t>(value._data[1])
            - static_cast<uint16_t>((result[2] >> 8) != 0)
        );
    result[0] = (
            static_cast<uint16_t>(this->_data[0])
            - static_cast<uint16_t>(value._data[0])
            - static_cast<uint16_t>((result[1] >> 8) != 0)
        );
    conv._data[0] = static_cast<uint8_t>(result[0]);
    conv._data[1] = static_cast<uint8_t>(result[1]);
    conv._data[2] = static_cast<uint8_t>(result[2]);
    return conv;
}
