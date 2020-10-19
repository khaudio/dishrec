#include "WavHeader.h"


WavHeader::WavHeader() :
_formatSet(false),
_fileSizeSet(false),
_formatSize(16),
_headerSize(44)
{
    memcpy(this->_riffChunkID, "RIFF", 4);
    memcpy(this->_fileFormat, "WAVE", 4);
    memcpy(this->_formatChunkID, "fmt ", 4);
    memcpy(this->_dataChunkID, "data", 4);
}

WavHeader::~WavHeader()
{
}

void WavHeader::set_format(WavParameters params)
{
    set_sample_rate(params.sampleRate);
    if (params.isFloatingPoint) set_floating_point();
    else set_pcm();
    set_bit_depth(params.bitDepth);
    set_channels(params.numChannels);
    this->_byteRate = ((this->sampleRate * this->bitDepth * this->numChannels) / 8);
    this->_frameSize = (this->bitDepth * this->numChannels) / 8;
    this->_samplesPerSecond = this->sampleRate * this->numChannels;
    this->_formatSet = true;
}

void WavHeader::set_bit_depth(uint16_t bitsPerSample)
{
    this->bitDepth = bitsPerSample;
    this->sampleWidth = this->bitDepth / 8;
}

void WavHeader::set_pcm()
{
    this->isFloatingPoint = false;
    this->_formatCode = PCM;
}

void WavHeader::set_floating_point()
{
    this->isFloatingPoint = true;
    this->_formatCode = FLOATING_POINT;
}

void WavHeader::set_mpeg_1()
{
    this->isFloatingPoint = false;
    this->_formatCode = MPEG_1;
}

void WavHeader::set_sample_rate(uint32_t samplerate)
{
    this->sampleRate = samplerate;
}

void WavHeader::set_channels(uint16_t channels)
{
    this->numChannels = channels;
}

void WavHeader::set_data_size(uint32_t numBytes)
{
    this->_dataSize = numBytes;
    this->_fileSizeSet = true;
    size();
}

template <typename T>
void WavHeader::set_data_size(std::vector<T>& data)
{
    set_data_size(data.size() * sizeof(T));
}

uint32_t WavHeader::size()
{
    // File size (excludes RIFF chunk ID, which is 4 bytes)
    if (!this->_fileSizeSet) throw FILESIZE_NOT_SET;
    this->_fileSize = 4;
    this->_fileSize += this->_formatSize + 8;
    this->_fileSize += this->_dataSize + 8;
    return this->_fileSize;
}

size_t WavHeader::total_size()
{
    // Total file size including RIFF chunk ID
    return size() + 4;
}

void WavHeader::copy_to_buffer(uint8_t* buff)
{
    if (!this->_formatSet) throw FORMAT_NOT_SET;
    size_t index = 0;
    size();
    memcpy(buff + index, this->_riffChunkID, 4);
    index += 4;
    memcpy(buff + index, &this->_fileSize, 4);
    index += 4;
    memcpy(buff + index, this->_fileFormat, 4);
    index += 4;
    memcpy(buff + index, this->_formatChunkID, 4);
    index += 4;
    memcpy(buff + index, &this->_formatSize, 4);
    index += 4;
    memcpy(buff + index, &this->_formatCode, 2);
    index += 2;
    memcpy(buff + index, &this->numChannels, 2);
    index += 2;
    memcpy(buff + index, &this->sampleRate, 4);
    index += 4;
    memcpy(buff + index, &this->_byteRate, 4);
    index += 4;
    memcpy(buff + index, &this->sampleWidth, 2);
    index += 2;
    memcpy(buff + index, &this->bitDepth, 2);
    index += 2;
    memcpy(buff + index, this->_dataChunkID, 4);
    index += 4;
    memcpy(buff + index, &this->_dataSize, 4);
    index += 4;
}

void WavHeader::import_header(uint8_t* data)
{
    WavParameters readFormat;
    memcpy(this->_riffChunkID, data + WAV_CHUNK_ID, 4);
    memcpy(this->_fileFormat, data + WAV_FORMAT, 4);
    memcpy(this->_formatChunkID, data + WAV_SUBCHUNK_1_ID, 4);
    this->_formatSize = *reinterpret_cast<uint32_t*>(data[WAV_SUBCHUNK_1_SIZE]);
    this->_formatCode = *reinterpret_cast<uint16_t*>(data[WAV_AUDIO_FORMAT]);
    readFormat.numChannels = *reinterpret_cast<uint16_t*>(data[WAV_NUM_CHANNELS]);
    readFormat.sampleRate = *reinterpret_cast<uint32_t*>(data[WAV_SAMPLE_RATE]);
    readFormat.bitDepth = *reinterpret_cast<uint16_t*>(data[WAV_BITS_PER_SAMPLE]);
    memcpy(this->_dataChunkID, data + WAV_SUBCHUNK_2_ID, 4);
    set_data_size(*reinterpret_cast<uint32_t*>(data[WAV_SUBCHUNK_2_SIZE]));
    readFormat.isFloatingPoint = this->_formatCode == 0x0003;
    set_format(readFormat);
}
