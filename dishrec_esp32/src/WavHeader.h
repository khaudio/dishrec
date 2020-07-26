#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "ErrorEnums.h"

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

/* TODO 

remove
WavHeader(uint32_t samplerate, uint16_t bitsPerSample, bool isFloat, uint16_t channels);
virtual void set_format(uint32_t samplerate, uint16_t bitsPerSample, bool isFloat, uint16_t channels);
and write a new default uninitialized constructor

Set format with WavParameters */

enum wav_header_offset
{
/*                          Endianness              Size in bytes */
    CHUNK_ID = 0,           // Big endian           4
    CHUNK_SIZE = 4,         // Little endian        4
    FORMAT = 8,             // Big endian           4
    SUBCHUNK_1_ID = 12,     // Big endian           4
    SUBCHUNK_1_SIZE = 16,   // Little endian        4
    AUDIO_FORMAT = 20,      // Little endian        2
    NUM_CHANNELS = 22,      // Little endian        2
    SAMPLE_RATE = 24,       // Little endian        4
    BYTE_RATE = 28,         // Little endian        4
    BLOCK_ALIGN = 32,       // Little endian        2
    BITS_PER_SAMPLE = 34,   // Little endian        2
    SUBCHUNK_2_ID = 36,     // Big endian           4
    SUBCHUNK_2_SIZE = 40,   // Little endian        4
    DATA_START = 44         // Little endian        4
};

class WavHeader : public WavParameters
{
protected:
    bool _initialized, sampleRateIsStandard, bitDepthIsStandard;
    uint16_t formatCode, dataSize, frameSize;
    uint32_t byteRate, fileSize, samplesPerSecond;
    void set_data_rates();
    uint8_t* headerData = nullptr;
    std::string _chunkID = "RIFF", _subchunk1ID = "WAVEfmt ", _subchunkDataID = "data";
public:
    uint8_t sampleWidth;
    uint32_t headerSize, formatLength = 16;
    static std::array<uint32_t, 3> stdSampleRates;
    static std::array<uint16_t, 3> stdIntBitDepths;
    static std::array<uint16_t, 2> stdFloatBitDepths;
    WavHeader(uint32_t samplerate, uint16_t bitsPerSample, bool isFloat, uint16_t channels);
    WavHeader(WavParameters);
    WavHeader();
    ~WavHeader();
    virtual void set_bit_depth(uint16_t bitsPerSample, bool isFloat);
    virtual void set_sample_rate(uint32_t samplerate);
    virtual void set_channels(uint16_t channels);
    virtual void set_header_size(uint32_t length=44);
    virtual void set_format(uint32_t samplerate, uint16_t bitsPerSample, bool isFloat, uint16_t channels);
    virtual void set_format(WavParameters params);
    WavParameters get_format();
    void set_size(uint32_t dataSize);
    template <typename T>
    void set_size(std::vector<T>& data);
    uint32_t size();
    bool is_nonstandard();
    virtual std::string str();
    const uint8_t* get_header();
    void import_header(uint8_t* data);
};

#endif
