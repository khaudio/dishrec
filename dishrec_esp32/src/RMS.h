#ifndef RMS_H
#define RMS_H

#include "AudioUtils.h"

enum rms_err
{
    WINDOW_NOT_SET = 140
};

template <typename T>
class RMS
{
protected:
    unsigned int _index, _windowLength;
    std::vector<T> _window;

public:
    RMS();
    RMS(unsigned int windowLengthInSamples);

    ~RMS();
    
    void set_window(unsigned int numSamples);

    size_t size();
    
    T get();
    T get_dBFS();

    void set(T sample);
    template <typename I>
    void set(I samples);
    void set(int8_t sample);
    void set(uint8_t sample);
    void set(int16_t sample);
    void set(uint16_t sample);
    void set(int32_t sample);
    void set(uint32_t sample);
    void set(int64_t sample);
    void set(uint64_t sample);

    void set(std::vector<T> samples);
    template <typename I>
    void set(std::vector<I> samples);
    void set(std::vector<int8_t> samples);
    void set(std::vector<uint8_t> samples);
    void set(std::vector<int16_t> samples);
    void set(std::vector<uint16_t> samples);
    void set(std::vector<int32_t> samples);
    void set(std::vector<uint32_t> samples);
    void set(std::vector<int64_t> samples);
    void set(std::vector<uint64_t> samples);
};

#endif