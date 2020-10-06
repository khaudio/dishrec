#ifndef INT24_T_H
#define INT24_T_H

#include <cstdint>

class int24_t
{
private:
    uint8_t _data[3];
public:
    int24_t();
    template <typename T> int24_t(T num);
    ~int24_t();
};

#endif
