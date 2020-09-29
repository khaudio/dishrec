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
constexpr T get_zero();
template <typename T>
void clip_float(T& value);
template <typename F, typename I>
I float_to_int(F value);
template <typename F, typename I>
std::vector<I> float_to_int(std::vector<F> values);
template <typename I, typename F>
F int_to_float(I value);
template <typename I, typename F>
std::vector<F> int_to_float(std::vector<I> values);

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
    void force_rotate_write_buffer();
    bool writable();
    std::vector<T> _read();
    std::vector<T> read();
    uint8_t* get_read_ptr();
    uint8_t* get_write_ptr();
    void* get_read_void_ptr();
    void* get_write_void_ptr();
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
