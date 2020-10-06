#include "WavHeader.h"

std::array<uint32_t, 3> WavHeader::stdSampleRates = {
        48000, 96000, 192000
    };
std::array<uint16_t, 5> WavHeader::stdBitDepths = {8, 16, 24, 32, 64};

WavHeader::WavHeader() :
_formatSet(false),
_fileSizeSet(false),
_formatSize(16),
_headerSize(44)
{
    memcpy(this->_chunkID, "RIFF", 4);
    memcpy(this->_fileFormat, "WAVE", 4);
    memcpy(this->_subchunkFormatID, "fmt ", 4);
    memcpy(this->_subchunkDataID, "data", 4);
}

WavHeader::~WavHeader()
{
}

void WavHeader::set_format(WavParameters params)
{
    set_sample_rate(params.sampleRate);
    set_bit_depth(params.bitDepth, params.isFloatingPoint);
    set_channels(params.numChannels);
    this->_byteRate = ((this->sampleRate * this->bitDepth * this->numChannels) / 8);
    this->_frameSize = (this->bitDepth * this->numChannels) / 8;
    this->_samplesPerSecond = this->sampleRate * this->numChannels;
    this->_formatSet = true;
}

void WavHeader::set_bit_depth(uint16_t bitsPerSample, bool isFloat)
{
    // MPEG-1 Audio format not supported; PCM and FP only.
    this->_bitDepthIsStandard = false;
    this->bitDepth = bitsPerSample;
    this->sampleWidth = this->bitDepth / 8;
    this->isFloatingPoint = isFloat;
    this->_formatCode = (this->isFloatingPoint ? FLOATING_POINT : PCM);
    std::array<uint16_t, 5>::iterator iter = std::find(
            stdBitDepths.begin(), stdBitDepths.end(), this->bitDepth
        );
    if (iter != stdBitDepths.end()) this->_bitDepthIsStandard = true;
    this->_bitDepthIsStandard = (
            this->isFloatingPoint
            ? ((this->bitDepth == 32) || (this->bitDepth == 64))
            : this->_bitDepthIsStandard
        );
    #ifdef _DEBUG
    if (!this->_bitDepthIsStandard)
    {
        std::cout << "Non-standard bit depth selected" << std::endl;
    }
    std::cout << "Setting format to " << this->bitDepth;
    std::cout << ((this->_formatCode == 1) ? "-bit PCM" : "-bit float");
    std::cout << std::endl;
    #endif
}

void WavHeader::set_sample_rate(uint32_t samplerate)
{
    this->_sampleRateIsStandard = false;
    this->sampleRate = samplerate;
    std::array<uint32_t, 3>::iterator iter = std::find(
            stdSampleRates.begin(), stdSampleRates.end(), this->sampleRate
        );
    if (iter != stdSampleRates.end()) this->_sampleRateIsStandard = true;
    #ifdef _DEBUG
    if (!this->_sampleRateIsStandard)
    {
        std::cout << "Non-standard sample rate selected" << std::endl;
    }
    std::cout << "Setting sample rate to " << this->sampleRate << std::endl;
    #endif
}

bool WavHeader::is_nonstandard()
{
    return !(this->_sampleRateIsStandard && this->_bitDepthIsStandard);
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
    memcpy(buff + index, this->_chunkID, 4);
    index += 4;
    memcpy(buff + index, &this->_fileSize, 4);
    index += 4;
    memcpy(buff + index, this->_fileFormat, 4);
    index += 4;
    memcpy(buff + index, this->_subchunkFormatID, 4);
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
    memcpy(buff + index, this->_subchunkDataID, 4);
    index += 4;
    memcpy(buff + index, &this->_dataSize, 4);
    index += 4;
}

void WavHeader::import_header(uint8_t* data)
{
    WavParameters readFormat;
    memcpy(this->_chunkID, data + WAV_CHUNK_ID, 4);
    memcpy(this->_fileFormat, data + WAV_FORMAT, 4);
    memcpy(this->_subchunkFormatID, data + WAV_SUBCHUNK_1_ID, 4);
    this->_formatSize = *reinterpret_cast<uint32_t*>(data[WAV_SUBCHUNK_1_SIZE]);
    this->_formatCode = *reinterpret_cast<uint16_t*>(data[WAV_AUDIO_FORMAT]);
    readFormat.numChannels = *reinterpret_cast<uint16_t*>(data[WAV_NUM_CHANNELS]);
    readFormat.sampleRate = *reinterpret_cast<uint32_t*>(data[WAV_SAMPLE_RATE]);
    readFormat.bitDepth = *reinterpret_cast<uint16_t*>(data[WAV_BITS_PER_SAMPLE]);
    memcpy(this->_subchunkDataID, data + WAV_SUBCHUNK_2_ID, 4);
    set_data_size(*reinterpret_cast<uint32_t*>(data[WAV_SUBCHUNK_2_SIZE]));
    readFormat.isFloatingPoint = this->_formatCode == 0x0003;
    set_format(readFormat);
}
