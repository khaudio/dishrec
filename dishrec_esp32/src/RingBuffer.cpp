#include "RingBuffer.h"

template <typename T>
void _getZero(T& var)
{
    static T half = (
            std::numeric_limits<T>::is_integer
            && std::is_unsigned<T>()
        ) ? pow(2, (sizeof(var) * 8) - 1) - 1 : 0;
    var = half;
}

template <typename T>
void clipFloat(T& value)
{
    if (value > 1.0)
    {
        value = 1.0;
    }
    else if (value < -1.0)
    {
        value = -1.0;
    }
}

template <typename F, typename I>
I floatToInt(F value)
{
    I zero;
    _getZero(zero);
    return round(
            value *
            ((value >= 0) ? (zero - 1) : zero)
            + (std::is_signed<I>() ? 0 : zero)
        );
}

template <typename F, typename I>
std::vector<I> floatToInt(std::vector<F> values)
{
    size_t length(values.size());
    std::vector<I> converted;
    converted.reserve(length);
    for (size_t i(0); i < length; ++i)
    {
        converted.emplace_back(floatToInt(values[i]));
    }
    return converted;
}


template <typename T>
RingBuffer<T>::RingBuffer(int bufferSize, uint8_t ringSize) :
ringLength(ringSize),
bufferLength(bufferSize)
{
    _getZero<T>(this->_zero);
    this->totalRingSampleLength = this->ringLength * this->bufferLength;
    this->_totalWritableLength = this->totalRingSampleLength - this->bufferLength;
    this->bytesPerBuffer = this->bufferLength * sizeof(T);
    this->samplesRemaining = this->bufferLength;
    this->samplesWritten = 0;
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
}

template <typename T>
RingBuffer<T>::~RingBuffer()
{
}

template <typename T>
int RingBuffer<T>::buffered()
{
    // Total number of samples buffered but unread
    return this->_buffered;
}

template <typename T>
int RingBuffer<T>::available()
{
    // Total number of samples unbuffered, not including read buffer
    return this->_totalWritableLength - this->_buffered;
}

template <typename T>
void RingBuffer<T>::rotateReadBuffer()
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
        rotateWriteBuffer();
    }
}

template <typename T>
void RingBuffer<T>::rotateWriteBuffer()
{
    if (++this->writeIndex >= this->ringLength)
    {
        this->writeIndex = 0;
    }
    this->samplesWritten = 0;
    this->samplesRemaining = this->bufferLength;
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
void RingBuffer<T>::rotate_write_buffer_manual()
{
    rotateWriteBuffer();
    this->_buffered += this->bufferLength;
    if (!writable())
    {
        rotateReadBuffer();
    }
}

template <typename T>
std::vector<T> RingBuffer<T>::read()
{
    // Returns current read buffer and rotates
    std::vector<T> output(_read());
    rotateReadBuffer();
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
    this->ring[this->writeIndex][this->samplesWritten] = data;
    ++this->samplesWritten;
    if (--this->samplesRemaining <= 0)
    {
        rotateWriteBuffer();
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
        if (remaining > this->samplesRemaining)
        {
            std::copy(
                    data.begin() + written,
                    data.begin() + written + this->samplesRemaining,
                    this->ring[this->writeIndex].begin() + this->samplesWritten
                );
            written += this->samplesRemaining;
            remaining -= this->samplesRemaining;
            rotateWriteBuffer();
        }
        else
        {
            std::copy(
                    data.begin() + written,
                    data.end(),
                    this->ring[this->writeIndex].begin() + this->samplesWritten
                );
            this->samplesWritten += remaining;
            this->samplesRemaining -= remaining;
            written += remaining;
            remaining = 0;
            if (this->samplesRemaining <= 0)
            {
                rotateWriteBuffer();
            }
        }
    }
    return written;
}

template <typename T>
int RingBuffer<T>::_writeTrim(std::vector<T>& data, bool force)
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
int RingBuffer<T>::write(std::vector<T>& data, bool trim, bool force)
{
    // Write method that optionally trims input vector
    return trim ? _writeTrim(data, force) : _write(data, force);
}

template <typename T>
int RingBuffer<T>::write(T data, bool force)
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
