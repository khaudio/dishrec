#include "RingBuffer.h"

using namespace Buffer;

Ring::Ring() :
ringLength(0)
{
}

Ring::~Ring()
{
}

template <typename T>
RingBuffer<T>::RingBuffer(int bufferSize, uint8_t ringSize) :
Ring(),
bufferLength(bufferSize)
{
    this->ringLength = ringSize;
    this->_zero = get_zero<T>();
    this->totalRingSampleLength = this->ringLength * this->bufferLength;
    this->_totalWritableLength = this->totalRingSampleLength - this->bufferLength;
    this->bytesPerBuffer = this->bufferLength * sizeof(T);
    this->_samplesRemaining = this->bufferLength;
    this->_samplesWritten = 0;
    this->_buffered = 0;
    this->ring.reserve(this->ringLength);
    for (int i(0); i < this->ringLength; ++i)
    {
        this->ring.emplace_back(std::vector<T>());
        this->ring[i].reserve(this->bufferLength);
        for (uint32_t j(0); j < this->bufferLength; ++j)
        {
            this->ring[i].emplace_back(this->_zero);
        }
    }
    this->readIndex = 0;
    this->writeIndex = 1;
    #ifdef _DEBUG
    std::cout << "" << std::endl;
    std::cout << "Bytes per buffer: " << this->bytesPerBuffer << std::endl;
    #endif
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
}

template <typename T>
int RingBuffer<T>::sub_buffers_full()
{
    // Total number of unread readable buffers
    return (this->_buffered / this->bufferLength);
}

template <typename T>
int RingBuffer<T>::buffered()
{
    /* Total number of samples buffered but unread,
    excluding the current write buffer */
    return (this->_buffered / this->bufferLength) * this->bufferLength;
}

template <typename T>
int RingBuffer<T>::samples_buffered()
{
    /* Total number of samples buffered but unread,
    including written to the current write buffer */
    return this->_buffered;
}

template <typename T>
int RingBuffer<T>::available()
{
    // Total number of samples unbuffered, not including read buffer
    return this->_totalWritableLength - this->_buffered;
}

template <typename T>
void RingBuffer<T>::rotate_read_buffer()
{
    // Rotates read buffer and forces write buffer forward if overrun
    if (++this->readIndex >= this->ringLength)
    {
        this->readIndex = 0;
    }
    this->_buffered -= this->bufferLength;
    this->_buffered = (this->_buffered < 0) ? 0 : this->_buffered;
    if (!writable())
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
void RingBuffer<T>::force_rotate_write_buffer()
{
    rotate_write_buffer();
    if (!writable())
    {
        rotate_read_buffer();
    }
}

template <typename T>
bool RingBuffer<T>::writable()
{
    return this->readIndex != this->writeIndex;
}

template <typename T>
std::vector<T> RingBuffer<T>::_read()
{
    // Returns current read buffer
    std::vector<T> output(this->ring[this->readIndex]);
    return output;
}

template <typename T>
uint8_t* RingBuffer<T>::get_read_ptr()
{
    uint8_t* currentReadBuffer = reinterpret_cast<uint8_t*>(
            &(this->ring[this->readIndex][0])
        );
    return currentReadBuffer;
}

template <typename T>
uint8_t* RingBuffer<T>::get_write_ptr()
{
    uint8_t* currentWriteBuffer = reinterpret_cast<uint8_t*>(
            &(this->ring[this->writeIndex][0])
        );
    return currentWriteBuffer;
}

template <typename T>
void* RingBuffer<T>::get_write_void_ptr()
{
    void* currentWriteBuffer = reinterpret_cast<void*>(
            &(this->ring[this->writeIndex][0])
        );
    return currentWriteBuffer;
}

template <typename T>
void* RingBuffer<T>::get_read_void_ptr()
{
    void* currentReadBuffer = reinterpret_cast<void*>(
            &(this->ring[this->readIndex][0])
        );
    return currentReadBuffer;
}

template <typename T>
std::vector<T> RingBuffer<T>::read()
{
    // Returns current read buffer and rotates
    std::vector<T> output(_read());
    rotate_read_buffer();
    return output;
}

template <typename T>
int RingBuffer<T>::_write(T data, bool force)
{
    // Write a single sample
    if (!writable() && !force)
    {
        return 0;
    }
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
int RingBuffer<T>::_write(std::vector<T> data, bool force)
{
    // Writes along ring and returns total number of samples written
    int written(0), remaining(data.size());
    int8_t index(this->ringLength);
    while ((remaining > 0) && (writable() || force) && (index-- > 0))
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
int RingBuffer<T>::_write_trim(std::vector<T>& data, bool force)
{
    // Writes along ring and removes samples from input vector
    int written = _write(data, force);
    int remaining = data.size() - written;
    std::vector<T> unwritten;
    if (remaining > 0)
    {
        for (int i(0); i < remaining; ++i)
        {
            unwritten.emplace_back(data[written + i]);
        }
    }
    data = unwritten;
    return written;
}

template <typename T>
int RingBuffer<T>::write(T data, bool force)
{
    return _write(data, force);
}

template <typename T>
int RingBuffer<T>::write(std::vector<T> data, bool force)
{
    return _write(data, force);
}

template <typename T>
int RingBuffer<T>::write(uint8_t* data, size_t numBytes, bool force)
{
    std::vector<T> converted(
            reinterpret_cast<T*>(data),
            reinterpret_cast<T*>(data + numBytes)
        );
    return _write(converted, force);
}


template class RingBuffer<float>;
template class RingBuffer<double>;
template class RingBuffer<long double>;
template class RingBuffer<int8_t>;
template class RingBuffer<uint8_t>;
template class RingBuffer<int16_t>;
template class RingBuffer<uint16_t>;
template class RingBuffer<int32_t>;
template class RingBuffer<uint32_t>;
template class RingBuffer<int64_t>;
template class RingBuffer<uint64_t>;
