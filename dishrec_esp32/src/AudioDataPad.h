#ifndef AUDIODATAPAD_H
#define AUDIODATAPAD_H

#include <cstdint>
#include <stdexcept>
#include <vector>

template <typename T>
class PadMeta
{
protected:
    bool _isPadded;
    int_least8_t _usableWidth, _absoluteWidth, _paddedWidth;
public:
    PadMeta();
    ~PadMeta();
    virtual bool is_padded();
    virtual void set_usable_width(int numBytes);
    virtual int_least8_t get_usable_width();
    virtual int_least8_t get_absolute_width();
    virtual int_least8_t get_padded_width();
};

class AudioInt : public PadMeta<int_fast32_t>
{
protected:
    virtual void _set_bit_depth(uint16_t bitsPerSample);
public:
    AudioInt();
    ~AudioInt();
};

#endif
