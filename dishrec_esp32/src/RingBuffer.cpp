#include "RingBuffer.h"

template <typename T>
constexpr T get_zero()
{
    return (
            std::numeric_limits<T>::is_integer
            && std::is_unsigned<T>()
        ) ? pow(2, (sizeof(T) * 8) - 1) - 1 : 0;
}

template <typename T>
void clip_float(T& value)
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
I float_to_int(F value)
{
    I zero = get_zero<I>();
    return round(
            value
            * ((value >= 0) ? (zero - 1) : zero)
            + (std::is_signed<I>() ? 0 : zero)
        );
}

template <typename F, typename I>
std::vector<I> float_to_int(std::vector<F> values)
{
    size_t length = values.size();
    std::vector<I> converted;
    converted.reserve(length);
    for (size_t i(0); i < length; ++i)
    {
        converted.emplace_back(float_to_int<F, I>(values[i]));
    }
    return converted;
}

template <typename I, typename F>
F int_to_float(I value)
{
    I zero = get_zero<I>();
    if (std::is_signed<I>())
    {
        return (
                static_cast<F>(value)
                / static_cast<F>(std::numeric_limits<I>::max())
            );
    }
    else
    {
        return (
                static_cast<F>(value - zero)
                / static_cast<F>(zero)
            );
    }
}

template <typename I, typename F>
std::vector<F> int_to_float(std::vector<I> values)
{
    size_t length = values.size();
    std::vector<F> converted;
    converted.reserve(length);
    for (size_t i(0); i < length; ++i)
    {
        converted.emplace_back(int_to_float<I, F>(values[i]));
    }
    return converted;
}

template <typename T>
RingBuffer<T>::RingBuffer(int bufferSize, uint8_t ringSize) :
ringLength(ringSize),
bufferLength(bufferSize)
{
    this->_zero = get_zero<T>();
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
    this->samplesWritten = 0;
    this->samplesRemaining = this->bufferLength;

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
    this->ring[this->writeIndex][this->samplesWritten] = data;
    ++this->samplesWritten;
    if (--this->samplesRemaining <= 0)
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
        if (remaining > this->samplesRemaining)
        {
            std::copy(
                    data.begin() + written,
                    data.begin() + written + this->samplesRemaining,
                    this->ring[this->writeIndex].begin() + this->samplesWritten
                );
            written += this->samplesRemaining;
            remaining -= this->samplesRemaining;
            rotate_write_buffer();
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

template float get_zero<float>();
template double get_zero<double>();
template long double get_zero<long double>();
template int8_t get_zero<int8_t>();
template uint8_t get_zero<uint8_t>();
template int16_t get_zero<int16_t>();
template uint16_t get_zero<uint16_t>();
template int32_t get_zero<int32_t>();
template uint32_t get_zero<uint32_t>();
template int64_t get_zero<int64_t>();
template uint64_t get_zero<uint64_t>();

template void clip_float(float& value);
template void clip_float(double& value);
template void clip_float(long double& value);

template int8_t float_to_int(float value);
template uint8_t float_to_int(float value);
template int16_t float_to_int(float value);
template uint16_t float_to_int(float value);
template int32_t float_to_int(float value);
template uint32_t float_to_int(float value);
template int64_t float_to_int(float value);
template uint64_t float_to_int(float value);

template int8_t float_to_int(double value);
template uint8_t float_to_int(double value);
template int16_t float_to_int(double value);
template uint16_t float_to_int(double value);
template int32_t float_to_int(double value);
template uint32_t float_to_int(double value);
template int64_t float_to_int(double value);
template uint64_t float_to_int(double value);

template int8_t float_to_int(long double value);
template uint8_t float_to_int(long double value);
template int16_t float_to_int(long double value);
template uint16_t float_to_int(long double value);
template int32_t float_to_int(long double value);
template uint32_t float_to_int(long double value);
template int64_t float_to_int(long double value);
template uint64_t float_to_int(long double value);

template std::vector<int8_t> float_to_int(std::vector<float> values);
template std::vector<uint8_t> float_to_int(std::vector<float> values);
template std::vector<int16_t> float_to_int(std::vector<float> values);
template std::vector<uint16_t> float_to_int(std::vector<float> values);
template std::vector<int32_t> float_to_int(std::vector<float> values);
template std::vector<uint32_t> float_to_int(std::vector<float> values);
template std::vector<int64_t> float_to_int(std::vector<float> values);
template std::vector<uint64_t> float_to_int(std::vector<float> values);

template std::vector<int8_t> float_to_int(std::vector<double> values);
template std::vector<uint8_t> float_to_int(std::vector<double> values);
template std::vector<int16_t> float_to_int(std::vector<double> values);
template std::vector<uint16_t> float_to_int(std::vector<double> values);
template std::vector<int32_t> float_to_int(std::vector<double> values);
template std::vector<uint32_t> float_to_int(std::vector<double> values);
template std::vector<int64_t> float_to_int(std::vector<double> values);
template std::vector<uint64_t> float_to_int(std::vector<double> values);

template std::vector<int8_t> float_to_int(std::vector<long double> values);
template std::vector<uint8_t> float_to_int(std::vector<long double> values);
template std::vector<int16_t> float_to_int(std::vector<long double> values);
template std::vector<uint16_t> float_to_int(std::vector<long double> values);
template std::vector<int32_t> float_to_int(std::vector<long double> values);
template std::vector<uint32_t> float_to_int(std::vector<long double> values);
template std::vector<int64_t> float_to_int(std::vector<long double> values);
template std::vector<uint64_t> float_to_int(std::vector<long double> values);

template float int_to_float(int8_t value);
template float int_to_float(uint8_t value);
template float int_to_float(int16_t value);
template float int_to_float(uint16_t value);
template float int_to_float(int32_t value);
template float int_to_float(uint32_t value);
template float int_to_float(int64_t value);
template float int_to_float(uint64_t value);

template double int_to_float(int8_t value);
template double int_to_float(uint8_t value);
template double int_to_float(int16_t value);
template double int_to_float(uint16_t value);
template double int_to_float(int32_t value);
template double int_to_float(uint32_t value);
template double int_to_float(int64_t value);
template double int_to_float(uint64_t value);

template long double int_to_float(int8_t value);
template long double int_to_float(uint8_t value);
template long double int_to_float(int16_t value);
template long double int_to_float(uint16_t value);
template long double int_to_float(int32_t value);
template long double int_to_float(uint32_t value);
template long double int_to_float(int64_t value);
template long double int_to_float(uint64_t value);

template std::vector<float> int_to_float(std::vector<int8_t> values);
template std::vector<float> int_to_float(std::vector<uint8_t> values);
template std::vector<float> int_to_float(std::vector<int16_t> values);
template std::vector<float> int_to_float(std::vector<uint16_t> values);
template std::vector<float> int_to_float(std::vector<int32_t> values);
template std::vector<float> int_to_float(std::vector<uint32_t> values);
template std::vector<float> int_to_float(std::vector<int64_t> values);
template std::vector<float> int_to_float(std::vector<uint64_t> values);

template std::vector<double> int_to_float(std::vector<int8_t> values);
template std::vector<double> int_to_float(std::vector<uint8_t> values);
template std::vector<double> int_to_float(std::vector<int16_t> values);
template std::vector<double> int_to_float(std::vector<uint16_t> values);
template std::vector<double> int_to_float(std::vector<int32_t> values);
template std::vector<double> int_to_float(std::vector<uint32_t> values);
template std::vector<double> int_to_float(std::vector<int64_t> values);
template std::vector<double> int_to_float(std::vector<uint64_t> values);

template std::vector<long double> int_to_float(std::vector<int8_t> values);
template std::vector<long double> int_to_float(std::vector<uint8_t> values);
template std::vector<long double> int_to_float(std::vector<int16_t> values);
template std::vector<long double> int_to_float(std::vector<uint16_t> values);
template std::vector<long double> int_to_float(std::vector<int32_t> values);
template std::vector<long double> int_to_float(std::vector<uint32_t> values);
template std::vector<long double> int_to_float(std::vector<int64_t> values);
template std::vector<long double> int_to_float(std::vector<uint64_t> values);

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
