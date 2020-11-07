#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

#include "AudioUtils.h"

enum ringbuffer_err
{
    BUFFER_NOT_INITIALIZED = 150,
};

namespace Buffer
{

class Ring
{
public:
    uint8_t ringLength;

    Ring();
    ~Ring();
};

template <typename T>
class RingBuffer : virtual public Ring
{
public:
    // constexpr static T _zero = get_zero<T>();
    const static T _zero;

protected:
    bool _sizeIsSet;
    int
        _totalWritableLength, _buffered,
        _samplesWritten, _samplesRemaining;

public:
    uint32_t bufferLength, totalRingSampleLength, bytesPerBuffer;
    uint8_t readIndex, writeIndex;
    std::vector<std::vector<T>> ring;

    RingBuffer();
    RingBuffer(int bufferSize, uint8_t ringSize);
    ~RingBuffer();

    virtual size_t size();

    virtual void set_size(int bufferSize, uint8_t ringSize);

    virtual int buffered();
    virtual int available();
    virtual int sub_buffers_full();
    virtual bool is_writable();

    virtual void rotate_read_buffer();
    virtual void rotate_write_buffer();
    virtual void force_rotate_write_buffer();

    virtual uint8_t* get_read_ptr();
    virtual uint8_t* get_write_ptr();

/*                               Read                               */

protected:
    virtual const std::vector<T> _read() const;

public:
    virtual const std::vector<T> read();

/*                               Write                              */

    virtual int write(T data, bool force = false);
    virtual int write(std::vector<T> data, bool force = false);
    virtual int write(uint8_t* data, size_t numBytes, bool force = false);
};

};

#endif
