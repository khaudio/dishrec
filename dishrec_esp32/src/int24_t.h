#ifndef INT24_T_H
#define INT24_T_H

#include <cstdint>

class int24_t
{
private:
    uint8_t _data[3];
public:
    inline int24_t();
    inline int24_t(const int24_t& value);
    explicit inline int24_t(const uint32_t value);
    explicit int24_t(const int32_t value);
    explicit int24_t(const int16_t value);
    explicit int24_t(const uint16_t value);
    inline int24_t& operator=(const int24_t& value);
    // template <typename T> inline int24_t(T num);
    ~int24_t();

    static const int24_t& zero(void);
    static const int24_t& one(void);
    static const int24_t& max(void);
    static const int24_t& min(void);

    int24_t operator+(const int24_t& value) const;
    int24_t operator-(const int24_t& value) const;

    inline const bool is_neg (void) const { return (this->_data[0] & 0x80) != 0; }
    inline const bool is_zero(void) const { return this->_data[2] == 0 && this->_data[1] == 0 && this->_data[0] == 0; }
    static const int do_compare(const int24_t& u, const int24_t& value);
    inline const int compare(const int24_t& value) const;
    inline const int32_t to_int32(void) const;
};

#endif
