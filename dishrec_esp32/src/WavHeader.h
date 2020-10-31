#ifndef WAVHEADER_H
#define WAVHEADER_H

#include <algorithm>
#include <array>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "ErrorEnums.h"

namespace WavMeta
{

class WavFormat;
class Chunk;
class RiffChunk;
class DataChunk;
class FormatChunk;
class WavHeader;

enum wav_format_codes
{
    FORMAT_PCM = 0x0001,
    FORMAT_FLOATING_POINT = 0x0003,
    FORMAT_MPEG_1 = 0x0005
};

enum wav_format_err
{
    FORMAT_NOT_SET = 101,
    FORMAT_NOT_FOUND = 102,
    DATA_SIZE_NOT_SET = 103
};

class WavFormat
{
public:
    uint32_t sampleRate;
    uint16_t bitDepth, numChannels, sampleWidth, formatCode;
    
    virtual void set_bit_depth(uint16_t bitsPerSample);
    virtual void set_sample_rate(uint32_t samplerate);
    virtual void set_channels(uint16_t channels);
    virtual void set_format_code(uint16_t formatcode);
    
    virtual void set_pcm();
    virtual void set_floating_point();
    virtual void set_mpeg_1();

    virtual bool is_pcm();
    virtual bool is_floating_point();
    virtual bool is_mpeg_1();
};

class Chunk
{
public:
    char chunkID[4];
    uint32_t chunkSize;
    Chunk();
    Chunk(const char* chunkid);
    ~Chunk();
    virtual void set_chunk_size(uint32_t chunksize);

    virtual uint32_t size();
    virtual size_t total_size();

    // Export chunk to buffer
    virtual size_t get(uint8_t* buff);
    virtual size_t get(char* buff);
    virtual std::string str();

    // Import chunk from data
    virtual size_t set(const uint8_t* data);
    virtual size_t set(const char* data);
};

class RiffChunk : public Chunk
{
public:
    char riffType[4];
    RiffChunk();
    size_t get(uint8_t* buff) override;
    size_t set(const uint8_t* data) override;
};

class DataChunk : public Chunk
{
public:
    DataChunk();
    template <typename T>
    void set_chunk_size(std::vector<T>& data);
};

class FormatChunk : public Chunk
{
public:
    uint8_t data[40], *extra;
    uint16_t *formatCode, *numChannels, *sampleWidth, *bitDepth;
    uint32_t *sampleRate, *byteRate, *extraSize;
    FormatChunk();
    size_t get(uint8_t* buff) override;
    size_t set(const uint8_t* data) override;
};

class WavHeader : virtual public WavFormat
{
public:
    WavHeader();
    ~WavHeader();

    // File size (Header + Data)
    virtual void set_file_size(size_t numBytes);
    virtual size_t get_file_size();

    // Wav header size (excluding RIFF ID + Size)
    virtual size_t size();
    
    // Total header size including RIFF header
    virtual size_t total_size();

    virtual size_t get(uint8_t* buff);
    
    virtual size_t set(const uint8_t* data);

/*                               RIFF                               */

protected:
    RiffChunk riffChunk;
    size_t _headerSize;

/*                              Format                              */

protected:
    FormatChunk formatChunk;
    bool _sampleRateSet, _bitDepthSet, _numChannelsSet;

    virtual void _set_data_rates();

public:
    uint16_t frameSize;
    uint32_t byteRate, samplesPerSecond;

    void set_sample_rate(uint32_t samplerate) override;
    void set_bit_depth(uint16_t bitsPerSample) override;
    void set_channels(uint16_t channels) override;
    void set_pcm() override;
    void set_floating_point() override;
    void set_mpeg_1() override;
    void set_format_code(uint16_t formatcode) override;
    virtual void set_byte_rate(uint32_t manualDataRate);
    virtual void set_extra_format_data(const uint8_t* data);
    virtual void set_extra_format_size(uint16_t length);
    virtual void set_format(WavFormat params);

    virtual size_t import_format_chunk(const uint8_t* data);

    // virtual bool is_floating_point();
    virtual bool is_set();

/*                               Data                               */

protected:
    DataChunk dataChunk;

public:
    virtual void set_data_size(size_t numBytes);
    virtual size_t get_data_size();
};

};

#endif
