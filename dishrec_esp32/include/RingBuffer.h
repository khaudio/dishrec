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

namespace Buffer
{

class Ring
{
public:
    uint8_t ringLength;

    Ring();
    ~Ring();
};

template <typename T = double>
class RingBuffer : virtual public Ring
{
protected:
    int
        _totalWritableLength, _buffered,
        _samplesWritten, _samplesRemaining;
    T _zero;

public:
    uint32_t bufferLength, totalRingSampleLength, bytesPerBuffer;
    uint8_t readIndex, writeIndex;
    std::vector<std::vector<T>> ring;

    RingBuffer(int bufferSize = 1024, uint8_t ringSize = 8);
    ~RingBuffer();

    int sub_buffers_full();
    int buffered();
    int samples_buffered();
    int available();

    void fill(T filler = 0, bool force = false);
    void zero(uint8_t bufferIndex);

    void rotate_read_buffer();
    void rotate_write_buffer();
    void force_rotate_write_buffer();

    bool writable();

protected:
    std::vector<T> _read();

public:
    std::vector<T> read();

    uint8_t* get_read_ptr();
    uint8_t* get_write_ptr();
    void* get_read_void_ptr();
    void* get_write_void_ptr();

protected:
    int _write(std::vector<T> data, bool force = false);
    int _write(T data, bool force = false);
    int _write_trim(std::vector<T>& data, bool force = false);
    
public:
    int write(T data, bool force = false);
    int write(std::vector<T> data, bool force = false);
    int write(uint8_t* data, size_t numBytes, bool force = false);
};

};

#endif
