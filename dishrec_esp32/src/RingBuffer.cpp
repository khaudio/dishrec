#include "RingBuffer.h"

using namespace Buffer;

template <typename T>
constexpr T RingBuffer<T>::_zero = get_zero<T>();

Ring::Ring() :
ringLength(0)
{
}

Ring::~Ring()
{
}

template <typename T>
RingBuffer<T>::RingBuffer() :
Ring(),
_sizeIsSet(false)
{
}

template <typename T>
RingBuffer<T>::RingBuffer(int bufferSize, uint8_t ringSize) :
Ring(),
_sizeIsSet(true),
_buffered(0),
_samplesWritten(0),
_samplesRemaining(bufferSize),
bufferLength(bufferSize),
bytesPerBuffer(bufferSize * sizeof(T)),
readIndex(0),
writeIndex(1)
{
    this->ringLength = ringSize;
    this->totalRingSampleLength = this->ringLength * this->bufferLength;
    this->_totalWritableLength = this->totalRingSampleLength - this->bufferLength;
    this->ring.reserve(this->ringLength);
    for (int i(0); i < this->ringLength; ++i)
    {
        this->ring.emplace_back(std::vector<T>());
        this->ring[i].reserve(this->bufferLength);
        for (uint32_t j(0); j < this->bufferLength; ++j)
        {
            this->ring[i].emplace_back(_zero);
        }
    }
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
}

template <typename T>
void RingBuffer<T>::set_size(int bufferSize, uint8_t ringSize)
{
    this->bufferLength = bufferSize;
    this->ringLength = ringSize;
    this->totalRingSampleLength = this->ringLength * this->bufferLength;
    this->_totalWritableLength = this->totalRingSampleLength - this->bufferLength;
    this->bytesPerBuffer = this->bufferLength * sizeof(T);
    this->_samplesRemaining = this->bufferLength;
    this->_samplesWritten = 0;
    this->_buffered = 0;
    this->ring = std::vector<std::vector<T>>();
    this->ring.reserve(this->ringLength);
    for (int i(0); i < this->ringLength; ++i)
    {
        this->ring.emplace_back(std::vector<T>());
        this->ring[i].reserve(this->bufferLength);
        for (uint32_t j(0); j < this->bufferLength; ++j)
        {
            this->ring[i].emplace_back(_zero);
        }
    }
    this->readIndex = 0;
    this->writeIndex = 1;
    this->_sizeIsSet = true;
}

template <typename T>
size_t RingBuffer<T>::size()
{
    /* Total number of samples that can fit in all rings.
    Returns samples, not bytes. */
    return this->totalRingSampleLength;
}

template <typename T>
int RingBuffer<T>::buffered()
{
    /* Total number of samples buffered but unread,
    excluding the current write buffer */
    return (this->_buffered / this->bufferLength) * this->bufferLength;
}

template <typename T>
int RingBuffer<T>::available()
{
    /* Total number of samples unbuffered, not including read buffer */
    return this->_totalWritableLength - this->_buffered;
}

template <typename T>
int RingBuffer<T>::sub_buffers_full()
{
    /* Total number of unread readable buffers */
    return (this->_buffered / this->bufferLength);
}

template <typename T>
bool RingBuffer<T>::is_writable()
{
    #ifdef _DEBUG
    if (!this->_sizeIsSet) throw BUFFER_NOT_INITIALIZED;
    #endif
    
    return this->readIndex != this->writeIndex;
}

template <typename T>
void RingBuffer<T>::rotate_read_buffer()
{
    /* Rotates read buffer and forces write buffer forward if overrun */
    if (++this->readIndex >= this->ringLength)
    {
        this->readIndex = 0;
    }
    this->_buffered -= this->bufferLength;
    this->_buffered = (this->_buffered < 0) ? 0 : this->_buffered;
    if (!is_writable())
    {
        rotate_write_buffer();
    }
}

template <typename T>
void RingBuffer<T>::rotate_write_buffer()
{
    if (++this->writeIndex >= this->ringLength)
    {
        this->writeIndex = 0;
    }
    this->_samplesWritten = 0;
    this->_samplesRemaining = this->bufferLength;

    this->_buffered += this->bufferLength;
}

template <typename T>
inline void RingBuffer<T>::force_rotate_write_buffer()
{
    rotate_write_buffer();
    if (!is_writable())
    {
        rotate_read_buffer();
    }
}

template <typename T>
inline uint8_t* RingBuffer<T>::get_read_ptr()
{
    /* Returns current read buffer */
    return reinterpret_cast<uint8_t*>(
            &(this->ring[this->readIndex][0])
        );
}

template <typename T>
inline uint8_t* RingBuffer<T>::get_write_ptr()
{
    /* Returns current write buffer */
    return reinterpret_cast<uint8_t*>(
            &(this->ring[this->writeIndex][0])
        );
}

template <typename T>
inline const std::vector<T> RingBuffer<T>::_read() const
{
    /* Returns current read buffer */
    return this->ring[this->readIndex];
}

template <typename T>
const std::vector<T> RingBuffer<T>::read()
{
    /* Returns current read buffer and rotates */
    #ifdef _DEBUG
    if (!this->_sizeIsSet) throw BUFFER_NOT_INITIALIZED;
    #endif

    std::vector<T> output(_read());
    rotate_read_buffer();
    return output;
}

template <typename T>
int RingBuffer<T>::write(T data, bool force)
{
    /* Write a single sample */
    #ifdef _DEBUG
    if (!this->_sizeIsSet) throw BUFFER_NOT_INITIALIZED;
    #endif

    if (!is_writable() && !force) return 0;
    this->ring[this->writeIndex][this->_samplesWritten] = data;
    ++this->_samplesWritten;
    if (--this->_samplesRemaining <= 0)
    {
        rotate_write_buffer();
    }
    ++this->_buffered;
    return 1;
}

template <typename T>
int RingBuffer<T>::write(std::vector<T> data, bool force)
{
    /* Writes along ring and returns total number of samples written.
    If forced, unread data will be overwritten. */
    int written(0), remaining(data.size());
    int8_t index(this->ringLength);
    while ((remaining > 0) && (is_writable() || force) && (index-- > 0))
    {
        if (remaining > this->_samplesRemaining)
        {
            std::copy(
                    data.begin() + written,
                    data.begin() + written + this->_samplesRemaining,
                    this->ring[this->writeIndex].begin() + this->_samplesWritten
                );
            written += this->_samplesRemaining;
            remaining -= this->_samplesRemaining;
            rotate_write_buffer();
        }
        else
        {
            std::copy(
                    data.begin() + written,
                    data.end(),
                    this->ring[this->writeIndex].begin() + this->_samplesWritten
                );
            this->_samplesWritten += remaining;
            this->_samplesRemaining -= remaining;
            written += remaining;
            remaining = 0;
            if (this->_samplesRemaining <= 0)
            {
                rotate_write_buffer();
            }
        }
    }
    return written;
}

template <typename T>
int RingBuffer<T>::write(uint8_t* data, size_t numBytes, bool force)
{
    /* Casts to std::vector and returns number of samples written.
    If forced, unread data will be overwritten. */
    std::vector<T> converted(
            reinterpret_cast<T*>(data),
            reinterpret_cast<T*>(data + numBytes)
        );
    return write(converted, force);
}

template class Buffer::RingBuffer<int8_t>;
template class Buffer::RingBuffer<uint8_t>;
template class Buffer::RingBuffer<int16_t>;
template class Buffer::RingBuffer<uint16_t>;
template class Buffer::RingBuffer<int32_t>;
template class Buffer::RingBuffer<uint32_t>;
template class Buffer::RingBuffer<int64_t>;
template class Buffer::RingBuffer<uint64_t>;
template class Buffer::RingBuffer<int_audio>;
template class Buffer::RingBuffer<float>;
template class Buffer::RingBuffer<double>;
template class Buffer::RingBuffer<long double>;
