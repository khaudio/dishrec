#ifndef AUDIODATAPAD_H
#define AUDIODATAPAD_H

#include <cstdint>
#include <stdexcept>
#include <vector>

#include <iostream>


class PadMeta
{
protected:
    int _usableWidth, _absoluteWidth, _paddedWidth;
    
    void _check_width();

public:
    PadMeta();
    PadMeta(uint16_t absolute, uint16_t usable);

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
    Packer(uint16_t absolute, uint16_t usable);

    ~Packer();

    void unpack(uint8_t* packed, int_fast32_t padded);
    
    void pack(uint8_t* pack, int_fast32_t padded);

    // Unpack to ring buffer
    void unpack(std::vector<int_fast32_t>* padded, uint8_t* packed);

    // Pack from ring buffer to file or output
    void pack(uint8_t* packed, std::vector<int_fast32_t>* padded);
};

#endif
