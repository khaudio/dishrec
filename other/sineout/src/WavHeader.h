#ifndef DEBUG
#define DEBUG           true
#endif

#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>

template <typename T>
T reverse_endianness(T& source);
template <typename T>
int write_to_stream(std::ostream& stream, T obj, bool reversed=false);

struct WavParameters
{
    uint32_t sampleRate;
    uint16_t bitDepth;
    bool isFloatingPoint;
    uint16_t numChannels;
};

class WavHeader : public WavParameters
{
protected:
    bool sampleRateIsStandard, bitDepthIsStandard;
    uint16_t formatCode, dataSize, frameSize;
    uint32_t byteRate, fileSize, samplesPerSecond;
    void set_bit_depth(uint16_t bitsPerSample, bool isFloat);
    void set_sample_rate(uint32_t rate);
    void set_channels(uint16_t channels);
    void set_data_rates();
    uint8_t* headerData = nullptr;
public:
    uint8_t sampleWidth;
    uint32_t headerSize, formatLength = 16;
    static std::array<uint32_t, 4> stdSampleRates;
    static std::array<uint16_t, 3> stdIntBitDepths;
    static std::array<uint16_t, 2> stdFloatBitDepths;
    WavHeader(uint32_t rate=48000, uint16_t bitsPerSample=32, bool isFloat=true, uint16_t channels=1);
    WavHeader(WavParameters);
    ~WavHeader();
    void set_header_size(uint32_t length=44);
    void set_format(uint32_t rate, uint16_t bitsPerSample, bool isFloat, uint16_t channels);
    void set_format(WavParameters params);
    WavParameters get_format();
    void set_size(uint32_t dataSize);
    template <typename T>
    void set_size(std::vector<T>& data);
    uint32_t size();
    bool is_nonstandard();
    uint8_t* get_header();
};

#endif
