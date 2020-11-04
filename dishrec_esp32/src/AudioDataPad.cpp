#include "AudioDataPad.h"

PadMeta::PadMeta() :
_usableWidth(0),
_absoluteWidth(0)
{
}

PadMeta::PadMeta(uint16_t absolute, uint16_t usable) :
_usableWidth(usable),
_absoluteWidth(absolute)
{
    _check_width();
}

PadMeta::~PadMeta()
{
}

void PadMeta::_check_width()
{

    if (!this->_absoluteWidth || !this->_usableWidth)
    {
        throw std::out_of_range("Width must be > 0");
    }
    else if (this->_usableWidth > this->_absoluteWidth)
    {
        throw std::out_of_range("Too many bits for datatype");
    }
}

void PadMeta::set_absolute_width(int width)
{
    #ifdef _DEBUG
    if (!width) throw std::out_of_range("Absolute width must be > 0");
    #endif
    
    this->_absoluteWidth = width;
}

inline int PadMeta::get_usable_width()
{
    return this->_usableWidth;
}

inline int PadMeta::get_absolute_width()
{
    return this->_absoluteWidth;
}

inline int PadMeta::get_padded_width()
{
    return this->_paddedWidth;
}

inline bool PadMeta::is_padded()
{
    return (this->_usableWidth < this->_absoluteWidth);
}

void PadMeta::set_bit_depth(uint16_t bitsPerSample)
{
    int numBytes(bitsPerSample / 8);
    this->_usableWidth = numBytes;
    this->_paddedWidth = this->_absoluteWidth - this->_usableWidth;
    #ifdef _DEBUG
    _check_width();
    #endif
}

inline uint16_t PadMeta::get_bit_depth()
{
    return this->_usableWidth * 8;
}

Packer::Packer() :
PadMeta()
{
    // Default to 32-bit int absolute width
    set_absolute_width(sizeof(int_fast32_t));
}

Packer::Packer(uint16_t absolute, uint16_t usable) :
PadMeta(absolute, usable)
{
}

Packer::~Packer()
{
}

void Packer::unpack(uint8_t* packed, int_fast32_t padded)
{
    #ifdef _DEBUG
    _check_width();
    #endif

    padded = 0;
    uint8_t* unpacked = reinterpret_cast<uint8_t*>(&padded);
    for (int i(0); i < this->_usableWidth; ++i)
    {
        unpacked[i] = packed[i];
    }
}

void Packer::pack(uint8_t* packed, int_fast32_t padded)
{
    #ifdef _DEBUG
    _check_width();
    #endif

    uint8_t* unpacked = reinterpret_cast<uint8_t*>(&padded);
    for (int i(0); i < this->_usableWidth; ++i)
    {
        packed[i] = unpacked[i];
    }
}

void Packer::unpack(std::vector<int_fast32_t>* padded, uint8_t* packed)
{
    #ifdef _DEBUG
    _check_width();
    #endif

    size_t length(padded->size()), packedIndex(0);
    for (size_t i(0); i < length; ++i)
    {
        uint8_t* unpacked = reinterpret_cast<uint8_t*>(&padded->at(i));
        for (int j(0); j < this->_usableWidth; ++j)
        {
            unpacked[j] = packed[packedIndex++];
        }
    }
}

void Packer::pack(uint8_t* packed, std::vector<int_fast32_t>* padded)
{
    #ifdef _DEBUG
    _check_width();
    #endif

    size_t length(padded->size()), packedIndex(0);
    for (size_t i(0); i < length; ++i)
    {
        uint8_t* unpacked = reinterpret_cast<uint8_t*>(&padded->at(i));
        for (int j(0); j < this->_usableWidth; ++j)
        {
            packed[packedIndex++] = unpacked[j];
        }
    }
}
