#include "AudioDataPad.h"

template <typename T>
PadMeta<T>::PadMeta()
{
    this->_usableWidth = sizeof(T);
    this->_absoluteWidth = sizeof(T);
}

template <typename T>
PadMeta<T>::~PadMeta()
{
}

template <typename T>
bool PadMeta<T>::is_padded()
{
    return (this->_usableWidth < this->_absoluteWidth);
}

template <typename T>
void PadMeta<T>::set_usable_width(int_fast32_t numBytes)
{
    if (numBytes > this->_absoluteWidth)
    {
        throw std::out_of_range("Too many bits for datatype");
    }
    this->_usableWidth = numBytes;
    this->_paddedWidth = this->_absoluteWidth - this->_usableWidth;
}

template <typename T>
int_fast32_t PadMeta<T>::get_usable_width()
{
    return this->_usableWidth;
}

template <typename T>
int_fast32_t PadMeta<T>::get_absolute_width()
{
    return this->_absoluteWidth;
}

template <typename T>
int_fast32_t PadMeta<T>::get_padded_width()
{
    return this->_paddedWidth;
}

AudioInt::AudioInt() :
PadMeta<int_fast32_t>()
{
}

AudioInt::~AudioInt()
{
}

void AudioInt::set_bit_depth(uint16_t bitsPerSample)
{
    set_usable_width(bitsPerSample / 8);
}

template class PadMeta<int_fast32_t>;
template class PadMeta<uint8_t>;
template class PadMeta<float>;
template class PadMeta<double>;
