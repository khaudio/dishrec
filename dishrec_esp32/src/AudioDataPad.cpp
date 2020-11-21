#include "AudioDataPad.h"

using namespace DataPad;

PadMeta::PadMeta() :
_usableWidth(0),
_absoluteWidth(0)
{
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
    return this->_absoluteWidth - this->_usableWidth;
}

inline bool PadMeta::is_padded()
{
    return (this->_usableWidth < this->_absoluteWidth);
}

void PadMeta::set_bit_depth(uint16_t bitsPerSample)
{
    // Get width in bytes
    this->_usableWidth = (bitsPerSample / 8);

    // Expand to fit
    if (this->_usableWidth > this->_absoluteWidth)
    {
        set_absolute_width(this->_usableWidth);
    }
    
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
}

Packer::~Packer()
{
}

template <typename T>
inline void Packer::unpack(T* padded, uint8_t* packed)
{
    #ifdef _DEBUG
    _check_width();
    #endif
    unpack_data<T>(padded, packed, this->_usableWidth);
}

template <typename T>
inline void Packer::pack(uint8_t* packed, T* padded)
{
    #ifdef _DEBUG
    _check_width();
    #endif
    pack_data<T>(packed, padded, this->_usableWidth);
}

template <typename T>
void Packer::unpack(std::vector<T>* padded, uint8_t* packed)
{
    size_t length(padded->size()), packedIndex(0);
    for (size_t i(0); i < length; ++i)
    {
        unpack(&padded->at(i), &packed[packedIndex]);
        packedIndex += this->_usableWidth;
    }
}

template <typename T>
void Packer::pack(uint8_t* packed, std::vector<T>* padded)
{
    size_t length(padded->size()), packedIndex(0);
    for (size_t i(0); i < length; ++i)
    {
        pack(&(packed[packedIndex]), &(padded->at(i)));
        packedIndex += this->_usableWidth;
    }
}

template void Packer::pack<int8_t>(uint8_t*, int8_t*);
template void Packer::pack<uint8_t>(uint8_t*, uint8_t*);
template void Packer::pack<int16_t>(uint8_t*, int16_t*);
template void Packer::pack<uint16_t>(uint8_t*, uint16_t*);
template void Packer::pack<int32_t>(uint8_t*, int32_t*);
template void Packer::pack<uint32_t>(uint8_t*, uint32_t*);
template void Packer::pack<int64_t>(uint8_t*, int64_t*);
template void Packer::pack<uint64_t>(uint8_t*, uint64_t*);
template void Packer::pack<int_audio>(uint8_t*, int_audio*);

template void Packer::unpack<int8_t>(int8_t*, uint8_t*);
template void Packer::unpack<uint8_t>(uint8_t*, uint8_t*);
template void Packer::unpack<int16_t>(int16_t*, uint8_t*);
template void Packer::unpack<uint16_t>(uint16_t*, uint8_t*);
template void Packer::unpack<int32_t>(int32_t*, uint8_t*);
template void Packer::unpack<uint32_t>(uint32_t*, uint8_t*);
template void Packer::unpack<int64_t>(int64_t*, uint8_t*);
template void Packer::unpack<uint64_t>(uint64_t*, uint8_t*);
template void Packer::unpack<int_audio>(int_audio*, uint8_t*);

template void Packer::pack<int8_t>(uint8_t*, std::vector<int8_t>*);
template void Packer::pack<uint8_t>(uint8_t*, std::vector<uint8_t>*);
template void Packer::pack<int16_t>(uint8_t*, std::vector<int16_t>*);
template void Packer::pack<uint16_t>(uint8_t*, std::vector<uint16_t>*);
template void Packer::pack<int32_t>(uint8_t*, std::vector<int32_t>*);
template void Packer::pack<uint32_t>(uint8_t*, std::vector<uint32_t>*);
template void Packer::pack<int64_t>(uint8_t*, std::vector<int64_t>*);
template void Packer::pack<uint64_t>(uint8_t*, std::vector<uint64_t>*);
template void Packer::pack<int_audio>(uint8_t*, std::vector<int_audio>*);

template void Packer::unpack<int8_t>(std::vector<int8_t>*, uint8_t*);
template void Packer::unpack<uint8_t>(std::vector<uint8_t>*, uint8_t*);
template void Packer::unpack<int16_t>(std::vector<int16_t>*, uint8_t*);
template void Packer::unpack<uint16_t>(std::vector<uint16_t>*, uint8_t*);
template void Packer::unpack<int32_t>(std::vector<int32_t>*, uint8_t*);
template void Packer::unpack<uint32_t>(std::vector<uint32_t>*, uint8_t*);
template void Packer::unpack<int64_t>(std::vector<int64_t>*, uint8_t*);
template void Packer::unpack<uint64_t>(std::vector<uint64_t>*, uint8_t*);
template void Packer::unpack<int_audio>(std::vector<int_audio>*, uint8_t*);
