#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

template <typename T>
void _getZero(T&);
template <typename T = double>
void clipFloat(T&);
template <typename F = double, typename I = int16_t>
I floatToInt(F);
template <typename F = double, typename I = int16_t>
std::vector<I> floatToInt(std::vector<F>);

template <typename T = double>
class RingBuffer
{
public:
    uint8_t ringLength;
    uint32_t bufferLength, totalRingSampleLength, bytesPerBuffer;
    RingBuffer(int bufferSize = 1024, uint8_t ringSize = 8);
    ~RingBuffer();
    int sub_buffers_full();
    int buffered();
    int samples_buffered();
    int available();
    void fill(T filler=0, bool force = false);
    void zero(uint8_t bufferIndex);
    void rotate_read_buffer();
    void rotate_write_buffer();
    bool writable();
    std::vector<T> _read();
    std::vector<T> read();
    uint8_t* get_read_ptr();
    uint8_t* get_write_ptr();
    void* get_read_void_ptr();
    void* get_write_void_ptr();
    void rotate_write_buffer_manual();
    int _write(std::vector<T> data, bool force = false);
    int _write(T data, bool force = false);
    int _write_trim(std::vector<T>& data, bool force = false);
    int write(T data, bool force = false);
    int write(std::vector<T> data, bool force = false);
    int write(uint8_t* data, size_t numBytes, bool force = false);
    uint8_t readIndex, writeIndex;
    int
        samplesWritten,
        samplesRemaining,
        _totalWritableLength,
        _buffered;
    T _zero;
    std::vector<std::vector<T>> ring;
};

#endif
