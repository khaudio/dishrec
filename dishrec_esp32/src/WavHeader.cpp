#include "WavHeader.h"

std::array<uint32_t, 4> WavHeader::stdSampleRates = {
        44100, 48000, 96000, 192000
    };
std::array<uint16_t, 3> WavHeader::stdIntBitDepths = {8, 16, 24};
std::array<uint16_t, 2> WavHeader::stdFloatBitDepths = {32, 64};

template <typename T>
T reverse_endianness(T& source)
{
    uint32_t last(sizeof(T) - 1);
    T swapped;
    uint8_t* original = reinterpret_cast<uint8_t*>(&source);
    uint8_t* reversed = reinterpret_cast<uint8_t*>(&swapped);
    for (uint32_t i(0), j(last); i <= last; ++i, --j)
    {
        reversed[i] = original[j];
    }
    return swapped;
}

template <typename T>
int write_to_stream(std::ostream& stream, T obj, bool reversed)
{
    uint32_t length(sizeof(T));
    obj = reversed ? reverse_endianness<T>(obj) : obj;
    uint8_t* converted = reinterpret_cast<uint8_t*>(&obj);
    for (uint32_t i(0); i < length; ++i)
    {
        stream << converted[i];
    }
    return length;
}

WavHeader::WavHeader(
        uint32_t samplerate, uint16_t bitsPerSample, bool isFloat, uint16_t channels
    )
{
    set_format(samplerate, bitsPerSample, isFloat, channels);
    set_header_size();
}

WavHeader::WavHeader(WavParameters params)
{
    set_format(params);
    set_header_size();
}

WavHeader::~WavHeader()
{
    delete this->headerData;
    this->headerData = nullptr;
}

void WavHeader::set_header_size(uint32_t length)
{
    if (this->headerData != nullptr)
    {
        delete this->headerData;
    }
    this->headerSize = length;
    this->headerData = new uint8_t[this->headerSize];
}

void WavHeader::set_format(
        uint32_t samplerate, uint16_t bitsPerSample, bool isFloat, uint16_t channels
    )
{
    this->isFloatingPoint = isFloat;
    set_sample_rate(samplerate);
    set_bit_depth(bitsPerSample, isFloat);
    set_channels(channels);
    set_data_rates();
}

void WavHeader::set_format(WavParameters params)
{
    set_sample_rate(params.sampleRate);
    set_bit_depth(params.bitDepth, params.isFloatingPoint);
    set_channels(params.numChannels);
    set_data_rates();
}

void WavHeader::set_data_rates()
{
    this->byteRate = (
            (this->sampleRate * this->bitDepth * this->numChannels) / 8
        );
    this->frameSize = (this->bitDepth * this->numChannels) / 8;
    this->samplesPerSecond = this->sampleRate * this->numChannels;
}

void WavHeader::set_bit_depth(uint16_t bitsPerSample, bool isFloat)
{
    this->bitDepthIsStandard = false;
    this->bitDepth = bitsPerSample;
    this->sampleWidth = this->bitDepth / 8;
    this->isFloatingPoint = isFloat;
    this->formatCode = (this->isFloatingPoint ? 0x0003 : 0x0001);
    if (this->isFloatingPoint)
    {
        for (auto standardBps: stdFloatBitDepths)
        {
            if (this->bitDepth == standardBps)
            {
                this->bitDepthIsStandard = true;
                break;
            }
        }
    }
    else
    {
        for (auto standardBps: stdIntBitDepths)
        {
            if (this->bitDepth == standardBps)
            {
            this->bitDepthIsStandard = true;
            break;
            }
        }
    }
    #if DEBUG
    if (!this->bitDepthIsStandard)
    {
        std::cout << "Non-standard bit depth selected" << std::endl;
    }
    std::cout << "Setting format to " << this->bitDepth;
    std::cout << ((this->formatCode == 1) ? "-bit PCM" : "-bit float");
    std::cout << std::endl;
    #endif
}

void WavHeader::set_sample_rate(uint32_t samplerate)
{
    this->sampleRateIsStandard = false;
    this->sampleRate = samplerate;
    for (auto stdSampleRate: stdSampleRates)
    {
        if (this->sampleRate == stdSampleRate)
        {
            this->sampleRateIsStandard = true;
            break;
        }
    }
    #if DEBUG
    if (!this->sampleRateIsStandard)
    {
        std::cout << "Non-standard sample rate selected" << std::endl;
    }
    std::cout << "Setting sample rate to " << this->sampleRate << std::endl;
    #endif
}

void WavHeader::set_channels(uint16_t channels=1)
{
    this->numChannels = channels;
}

void WavHeader::set_size(uint32_t dataSize)
{
    this->dataSize = dataSize;
    this->fileSize = dataSize + 36;
}

template <typename T>
void WavHeader::set_size(std::vector<T>& data)
{
    set_size(data.size() * sizeof(T));
}

uint32_t WavHeader::size()
{
    return this->dataSize;
}

bool WavHeader::is_nonstandard()
{
    return !(this->sampleRateIsStandard && this->bitDepthIsStandard);
}

uint8_t* WavHeader::get_header()
{
    std::stringstream stream;
    stream << "RIFF";
    write_to_stream(stream, this->fileSize);
    stream << "WAVEfmt ";
    write_to_stream(stream, this->formatLength);
    write_to_stream(stream, this->formatCode);
    write_to_stream(stream, this->numChannels);
    write_to_stream(stream, this->sampleRate);
    write_to_stream(stream, this->byteRate);
    write_to_stream(stream, this->frameSize);
    write_to_stream(stream, this->bitDepth);
    stream << "data";
    write_to_stream(stream, this->dataSize);
    stream.seekp(0);
    for (uint32_t i(0); i < this->headerSize; ++i)
    {
        this->headerData[i] = stream.get();
    }
    return this->headerData;
}

std::string WavHeader::str()
{
    std::stringstream stream;
    stream << "RIFF";
    write_to_stream(stream, this->fileSize);
    stream << "WAVEfmt ";
    write_to_stream(stream, this->formatLength);
    write_to_stream(stream, this->formatCode);
    write_to_stream(stream, this->numChannels);
    write_to_stream(stream, this->sampleRate);
    write_to_stream(stream, this->byteRate);
    write_to_stream(stream, this->frameSize);
    write_to_stream(stream, this->bitDepth);
    stream << "data";
    write_to_stream(stream, this->dataSize);
    return stream.str();
}
