#include "WavHeader.h"

using namespace WavMeta;

inline void WavFormat::set_bit_depth(uint16_t bitsPerSample)
{
    this->bitDepth = bitsPerSample;
    this->sampleWidth = this->bitDepth / 8;
}

inline void WavFormat::set_sample_rate(uint32_t samplerate)
{
    this->sampleRate = samplerate;
}

inline void WavFormat::set_channels(uint16_t channels)
{
    this->numChannels = channels;
}

inline void WavFormat::set_format_code(uint16_t formatcode)
{
    switch (formatcode)
    {
        case (FORMAT_PCM):
            set_pcm();
            break;
        case (FORMAT_FLOATING_POINT):
            set_floating_point();
            break;
        case (FORMAT_MPEG_1):
            set_mpeg_1();
            break;
        default:
            throw FORMAT_NOT_FOUND;
    }
}

inline void WavFormat::set_pcm()
{
    this->formatCode = FORMAT_PCM;
}

inline void WavFormat::set_floating_point()
{
    this->formatCode = FORMAT_FLOATING_POINT;
}

inline void WavFormat::set_mpeg_1()
{
    this->formatCode = FORMAT_MPEG_1;
}

inline bool WavFormat::is_pcm()
{
    return (this->formatCode == FORMAT_PCM);
}

inline bool WavFormat::is_floating_point()
{
    return (this->formatCode == FORMAT_FLOATING_POINT);
}

inline bool WavFormat::is_mpeg_1()
{
    return (this->formatCode == FORMAT_MPEG_1);
}

Chunk::Chunk()
{
}

Chunk::Chunk(const char* chunkid)
{
    memcpy(this->chunkID, chunkid, sizeof(this->chunkID));
}

Chunk::~Chunk()
{
}

void Chunk::set_chunk_size(uint32_t chunksize)
{
    this->chunkSize = chunksize;
}

uint32_t Chunk::size()
{
    return this->chunkSize;
}

size_t Chunk::total_size()
{
    return size() + 8;
}

size_t Chunk::get(uint8_t* buff)
{
    memcpy(buff, this->chunkID, 4);
    memcpy(buff + 4, &this->chunkSize, 4);
    return 8;
}

size_t Chunk::get(char* buff)
{
    return get(reinterpret_cast<uint8_t*>(buff));
}

std::string Chunk::str()
{
    const size_t bufflen(total_size());
    uint8_t buff[bufflen];
    get(buff);
    return std::string(reinterpret_cast<const char*>(buff));
}

size_t Chunk::set(const uint8_t* data)
{
    memcpy(this->chunkID, data, 4);
    memcpy(&this->chunkSize, data + 4, 4);
    return 8;
}

size_t Chunk::set(const char* data)
{
    return set(reinterpret_cast<const uint8_t*>(data));
}

RiffChunk::RiffChunk() :
Chunk("RIFF")
{
    set_chunk_size(4);
    memcpy(this->riffType, "WAVE", 4);
}

size_t RiffChunk::get(uint8_t* buff)
{
    size_t index(Chunk::get(buff));
    memcpy(buff + index, this->riffType, 4);
    return index + 4;
}

size_t RiffChunk::set(const uint8_t* data)
{
    size_t index(Chunk::set(data));
    memcpy(this->riffType, data + index, 4);
    return index + 4;
}

DataChunk::DataChunk() :
Chunk("data")
{
}

template <typename T>
void DataChunk::set_chunk_size(std::vector<T>& data)
{
    this->chunkSize = (data.size() * sizeof(T));
}

FormatChunk::FormatChunk() :
Chunk("fmt ")
{
    this->formatCode = reinterpret_cast<uint16_t*>(&this->data);
    this->numChannels = reinterpret_cast<uint16_t*>(&this->data[2]);
    this->sampleRate = reinterpret_cast<uint32_t*>(&this->data[4]);
    this->byteRate = reinterpret_cast<uint32_t*>(&this->data[8]);
    this->sampleWidth = reinterpret_cast<uint16_t*>(&this->data[12]);
    this->bitDepth = reinterpret_cast<uint16_t*>(&this->data[14]);
    this->extraSize = reinterpret_cast<uint32_t*>(&this->data[16]);
    this->extra = &this->data[18];

    set_chunk_size(40);
    
    // Set WAVE_FORMAT_EXTENSIBLE extra data length
    *this->extraSize = 22;

    // Zero extra data
    memset(this->data + 18, 0, *reinterpret_cast<uint16_t*>(this->data + 16));
}

size_t FormatChunk::get(uint8_t* buff)
{
    size_t index(Chunk::get(buff));
    memcpy(buff + index, this->data, this->chunkSize);
    return index + this->chunkSize;
}

size_t FormatChunk::set(const uint8_t* data)
{
    size_t index(Chunk::set(data));
    memcpy(this->data, data + index, this->chunkSize);
    return index + this->chunkSize;
}

WavHeader::WavHeader() :
_headerSize(60),
_sampleRateSet(false),
_bitDepthSet(false),
_numChannelsSet(true)
{
    this->numChannels = 1; // Default to 1 channel
    set_pcm(); // Default to Linear PCM
}

WavHeader::~WavHeader()
{
}

void WavHeader::set_sample_rate(uint32_t samplerate)
{
    WavFormat::set_sample_rate(samplerate);
    *this->formatChunk.sampleRate = this->sampleRate;
    this->_sampleRateSet = true;
    _set_data_rates();
}

void WavHeader::set_bit_depth(uint16_t bitsPerSample)
{
    WavFormat::set_bit_depth(bitsPerSample);
    *this->formatChunk.bitDepth = this->bitDepth;
    *this->formatChunk.sampleWidth = this->sampleWidth;
    this->_bitDepthSet = true;
    _set_data_rates();
}

void WavHeader::set_channels(uint16_t channels)
{
    WavFormat::set_channels(channels);
    *this->formatChunk.numChannels = this->numChannels;
    this->_numChannelsSet = true;
    _set_data_rates();
}

void WavHeader::set_pcm()
{
    WavFormat::set_pcm();
    *this->formatChunk.formatCode = this->formatCode;
}

void WavHeader::set_floating_point()
{
    WavFormat::set_floating_point();
    *this->formatChunk.formatCode = this->formatCode;
}

void WavHeader::set_mpeg_1()
{
    WavFormat::set_mpeg_1();
    *this->formatChunk.formatCode = this->formatCode;
}

void WavHeader::set_format_code(uint16_t formatcode)
{
    WavFormat::set_format_code(formatcode);
    *this->formatChunk.formatCode = this->formatCode;
}

void WavHeader::set_byte_rate(uint32_t rate)
{
    this->byteRate = rate;
    *this->formatChunk.byteRate = this->byteRate;
}

void WavHeader::set_format(WavFormat params)
{
    set_sample_rate(params.sampleRate);
    set_bit_depth(params.bitDepth);
    set_channels(params.numChannels);
    set_format_code(params.formatCode);
}

void WavHeader::_set_data_rates()
{
    // Do not use is_set() here in case it is overriden
    if (!this->_sampleRateSet || !this->_bitDepthSet || !this->_numChannelsSet) return;
    set_byte_rate((this->sampleRate * this->bitDepth * this->numChannels) / 8);
    this->frameSize = this->sampleWidth * this->numChannels;
    this->samplesPerSecond = this->sampleRate * this->numChannels;
}

void WavHeader::set_extra_format_data(const uint8_t* data)
{
    memcpy(this->formatChunk.extra, data, *this->formatChunk.extraSize);
}

void WavHeader::set_extra_format_size(uint16_t length)
{
    *this->formatChunk.extraSize = length;
}

size_t WavHeader::import_format_chunk(const uint8_t* data)
{
    // Imports format chunk using setter methods
    size_t index(8);
    memcpy(this->formatChunk.chunkID, data, 4);
    this->formatChunk.chunkSize = *reinterpret_cast<uint32_t*>(data[4]);
    set_format_code(*reinterpret_cast<uint16_t*>(data[index]));
    index += 2;
    set_channels(*reinterpret_cast<uint16_t*>(data[index]));
    index += 2;
    set_sample_rate(*reinterpret_cast<uint32_t*>(data[index]));
    index += 10; // Skip byte rate and sample width
    set_bit_depth(*reinterpret_cast<uint16_t*>(data[index]));
    index += 2;
    int remaining(this->formatChunk.chunkSize - index);
    if (remaining > 2)
    {
        set_extra_format_size(*reinterpret_cast<uint16_t*>(data[index]));
        index += 2;
        set_extra_format_data(data + index);
    }
    return index;
}

bool WavHeader::is_set()
{
    return (
            this->_sampleRateSet
            && this->_bitDepthSet
            && this->_numChannelsSet
        );
}

void WavHeader::set_data_size(size_t numBytes)
{
    this->dataChunk.chunkSize = static_cast<uint32_t>(numBytes);
}

size_t WavHeader::get_data_size()
{
    return this->dataChunk.chunkSize;
}

void WavHeader::set_file_size(size_t numBytes)
{
    this->riffChunk.chunkSize = static_cast<uint32_t>(numBytes);
}

size_t WavHeader::get_file_size()
{
    return this->riffChunk.chunkSize;
}

size_t WavHeader::size()
{
    this->_headerSize = 4; // _riffType
    this->_headerSize += this->formatChunk.total_size();
    this->_headerSize += 8; // Data chunk ID and size
    this->riffChunk.chunkSize = this->_headerSize + this->dataChunk.chunkSize;
    return this->_headerSize;
}

size_t WavHeader::total_size()
{
    return size() + 8;
}

size_t WavHeader::get(uint8_t* buff)
{
    size();
    size_t index(this->riffChunk.get(buff));
    index += this->formatChunk.get(buff + index);
    index += this->dataChunk.get(buff + index);
    return index;
}

size_t WavHeader::set(const uint8_t* data)
{
    size_t index(this->riffChunk.set(data));
    index += this->formatChunk.set(data + index);
    index += this->dataChunk.set(data + index);
    return index;
}
