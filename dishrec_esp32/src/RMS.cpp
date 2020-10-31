#include "RMS.h"

template <typename T>
RMS<T>::RMS() :
_index(0)
{
}

template <typename T>
RMS<T>::RMS(unsigned int windowLengthInSamples) :
_index(0)
{
    set_window(windowLengthInSamples);
}

template <typename T>
RMS<T>::~RMS()
{
}

template <typename T>
void RMS<T>::set_window(unsigned int numSamples)
{
    this->_windowLength = numSamples;
    this->_window.reserve(this->_windowLength);
    for (unsigned int i(0); i < this->_windowLength; ++i)
    {
        this->_window.emplace_back(get_zero<T>());
    }
}

template <typename T>
inline void RMS<T>::set(T sample)
{
    this->_window[this->_index] = std::abs(sample);
    this->_index = (this->_index++ >= this->_windowLength ? 0 : this->_index);
}

template <typename T>
template <typename I>
inline void RMS<T>::set(I sample)
{
    T converted = int_to_float<I, T>(sample);
    set(converted);
}

template <typename T>
inline void RMS<T>::set(int8_t sample)
{
    set<int8_t>(sample);
}

template <typename T>
inline void RMS<T>::set(uint8_t sample)
{
    set<uint8_t>(sample);
}

template <typename T>
inline void RMS<T>::set(int16_t sample)
{
    set<int16_t>(sample);
}

template <typename T>
inline void RMS<T>::set(uint16_t sample)
{
    set<uint16_t>(sample);
}

template <typename T>
inline void RMS<T>::set(int32_t sample)
{
    set<int32_t>(sample);
}

template <typename T>
inline void RMS<T>::set(uint32_t sample)
{
    set<uint32_t>(sample);
}

template <typename T>
inline void RMS<T>::set(int64_t sample)
{
    set<int64_t>(sample);
}

template <typename T>
inline void RMS<T>::set(uint64_t sample)
{
    set<uint64_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<T> samples)
{
    size_t length = samples.size();
    for (size_t i(0); i < length; ++i) set(samples[i]);
}

template <typename T>
template <typename I>
inline void RMS<T>::set(std::vector<I> samples)
{
    for (const I& sample: samples) set<I>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<int8_t> samples)
{
    for (const int8_t& sample: samples) set<int8_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<uint8_t> samples)
{
    for (const uint8_t& sample: samples) set<uint8_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<int16_t> samples)
{
    for (const int16_t& sample: samples) set<int16_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<uint16_t> samples)
{
    for (const uint16_t& sample: samples) set<uint16_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<int32_t> samples)
{
    for (const int32_t& sample: samples) set<int32_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<uint32_t> samples)
{
    for (const uint32_t& sample: samples) set<uint32_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<int64_t> samples)
{
    for (const int64_t& sample: samples) set<int64_t>(sample);
}

template <typename T>
inline void RMS<T>::set(std::vector<uint64_t> samples)
{
    for (const uint64_t& sample: samples) set<uint64_t>(sample);
}

template <typename T>
size_t RMS<T>::size()
{
    return this->_windowLength;
}

template <typename T>
inline T RMS<T>::get()
{
    T sum(0);
    for (unsigned int i(0); i < this->_windowLength; ++i) sum += this->_window[i];
    return sum / this->_windowLength;
}

template <typename T>
inline T RMS<T>::get_dBFS()
{
    T value = RMS<T>::get();
    if (!value) return -HUGE_VAL;
    else return 20 * log10(value);
}

template class RMS<float>;
template class RMS<double>;
template class RMS<long double>;
