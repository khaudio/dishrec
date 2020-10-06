#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include "ErrorEnums.h"

template <typename T>
T reverse_endianness(T& source);
template <typename T>
int write_to_stream(std::ostream& stream, T obj, bool reversed=false);

enum wav_header_offset
{
/*                              Endianness              Size in bytes */
    WAV_CHUNK_ID = 0,           // Big endian           4
    WAV_CHUNK_SIZE = 4,         // Little endian        4
    WAV_FORMAT = 8,             // Big endian           4
    WAV_SUBCHUNK_1_ID = 12,     // Big endian           4
    WAV_SUBCHUNK_1_SIZE = 16,   // Little endian        4
    WAV_AUDIO_FORMAT = 20,      // Little endian        2
    WAV_NUM_CHANNELS = 22,      // Little endian        2
    WAV_SAMPLE_RATE = 24,       // Little endian        4
    WAV_BYTE_RATE = 28,         // Little endian        4
    WAV_BLOCK_ALIGN = 32,       // Little endian        2
    WAV_BITS_PER_SAMPLE = 34,   // Little endian        2
    WAV_SUBCHUNK_2_ID = 36,     // Big endian           4
    WAV_SUBCHUNK_2_SIZE = 40,   // Little endian        4
    WAV_DATA_START = 44         // Little endian        4
};

enum wav_format_codes
{
    PCM = 0x0001,
    FLOATING_POINT = 0x0003,
    MPEG_1 = 0x0005
};

enum wav_format_err
{
    FORMAT_NOT_SET = 101,
    FILESIZE_NOT_SET = 102
};

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
    bool _formatSet, _fileSizeSet, _sampleRateIsStandard, _bitDepthIsStandard;
    uint16_t _formatCode, _frameSize;
    uint32_t _fileSize, _formatSize, _headerSize, _dataSize, _byteRate, _samplesPerSecond;
    char _chunkID[4], _fileFormat[4], _subchunkFormatID[4], _subchunkDataID[4];
public:
    uint16_t sampleWidth;
    static std::array<uint32_t, 3> stdSampleRates;
    static std::array<uint16_t, 5> stdBitDepths;
    WavHeader();
    ~WavHeader();
    virtual void set_bit_depth(uint16_t bitsPerSample, bool isFloat);
    virtual void set_sample_rate(uint32_t samplerate);
    virtual bool is_nonstandard();
    virtual void set_channels(uint16_t channels);
    virtual void set_format(WavParameters params);
    void set_data_size(uint32_t dataSize);
    template <typename T>
    void set_data_size(std::vector<T>& data);
    virtual uint32_t size();
    virtual size_t total_size();
    virtual void copy_to_buffer(uint8_t* buff);
    virtual void import_header(uint8_t* data);
};

#endif
