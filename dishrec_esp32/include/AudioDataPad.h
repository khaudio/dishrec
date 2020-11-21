#ifndef AUDIODATAPAD_H
#define AUDIODATAPAD_H

#include <cstdint>
#include <stdexcept>
#include <vector>
#include "AudioUtils.h"

#include <iostream>
#include <iomanip>
#include <bitset>

namespace DataPad
{

class PadMeta
{
protected:
    int _usableWidth, _absoluteWidth;

    void _check_width();

public:
    PadMeta();
    ~PadMeta();
    
    virtual void set_absolute_width(int width);
    virtual int get_usable_width();
    virtual int get_absolute_width();
    virtual int get_padded_width();
    virtual bool is_padded();

    virtual void set_bit_depth(uint16_t bitsPerSample);
    virtual uint16_t get_bit_depth();
};

class Packer : public PadMeta
{
public:
    Packer();
    ~Packer();

    template <typename T>
    void unpack(T* padded, uint8_t* packed);
    
    template <typename T>
    void pack(uint8_t* pack, T* padded);

    template <typename T>
    void unpack(std::vector<T>* padded, uint8_t* packed);

    template <typename T>
    void pack(uint8_t* packed, std::vector<T>* padded);
};

};

#endif


