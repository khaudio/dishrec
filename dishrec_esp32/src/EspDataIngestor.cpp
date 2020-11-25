#include "EspDataIngestor.h"

DataIngestor::DataIngestor() :
I2S::Bus(),
_buff(),
_size(0)
{
    set_pcm();
    DataPad::Packer::set_absolute_width(sizeof(int_audio));
    I2S::Bus::set_receive();
}

DataIngestor::DataIngestor(
        uint32_t bufferSize,
        uint32_t ringSize,
        int i2sNum,
        bool isMaster,
        bool receives,
        bool transmits
    ) :
WavMeta::WavFormat(),
I2S::Bus(i2sNum, isMaster, receives, transmits),
_buff(bufferSize, ringSize)
{
    DataPad::Packer::set_absolute_width(sizeof(int_audio));
    set_pcm();
    this->_size = this->_buff.bufferLength * this->sampleWidth;
}

DataIngestor::~DataIngestor()
{
    delete this->_packedInput;
    delete this->_packedOutput;
}

size_t DataIngestor::size()
{
    this->_size = this->_buff.bufferLength * this->sampleWidth;
    return this->_size;
}

void DataIngestor::_create_packed_buffers()
{
    if (!this->sampleWidth || !this->_buff.bufferLength) return;
    if (this->_packedInput != nullptr) delete this->_packedInput;
    if (this->_packedOutput != nullptr) delete this->_packedOutput;
    this->_packedInput = new uint8_t[size()];
    this->_packedOutput = new uint8_t[size()];
}

void DataIngestor::set_size(int bufferSize, uint8_t ringSize)
{
    this->_buff.set_size(bufferSize, ringSize);
    I2S::Bus::set_buffer_size(bufferSize);
    _create_packed_buffers();
    if (is_running()) reload();
}

size_t DataIngestor::delay_in_samples() const
{
    return this->_buff.totalRingSampleLength;
}

void DataIngestor::set_bit_depth(uint16_t bitsPerSample)
{
    #ifdef _DEBUG
    if ((bitsPerSample > 32) || (bitsPerSample < 8))
    {
        throw std::out_of_range("Must be 8 <= bitsPerSample <= 32");
    }
    #endif
    WavMeta::WavFormat::set_bit_depth(bitsPerSample);
    DataPad::Packer::set_bit_depth(bitsPerSample);
    I2S::Bus::set_bit_depth(bitsPerSample);
    _create_packed_buffers();
    if (is_running()) reload();
}

void DataIngestor::set_sample_rate(uint32_t samplerate)
{
    WavMeta::WavFormat::set_sample_rate(samplerate);
    I2S::Bus::set_sample_rate(samplerate);
    if (is_running()) reload();
}

void DataIngestor::set_channels(uint16_t channels)
{
    WavMeta::WavFormat::set_channels(channels);
    I2S::Bus::set_channels(channels);
    if (is_running()) reload();
}

void DataIngestor::set_format(const WavFormatData& data)
{
    WavMeta::WavFormat::set_format(data);
    DataPad::Packer::set_bit_depth(this->bitDepth);
    if (is_running()) reload();
}

void DataIngestor::set_pcm()
{
    WavMeta::WavFormat::set_pcm();
    _create_packed_buffers();
    if (is_running()) reload();
}

void DataIngestor::set_floating_point()
{
    WavMeta::WavFormat::set_floating_point();
    _create_packed_buffers();
    if (is_running()) reload();
}

void DataIngestor::set_mpeg_1()
{
    throw NOT_IMPLEMENTED;
}

void DataIngestor::start()
{
    I2S::Bus::config();
    I2S::Bus::start();
}

void DataIngestor::stop()
{
    I2S::Bus::stop();
}

void DataIngestor::reload()
{
    stop();
    start();
}

inline uint8_t* DataIngestor::_read_raw()
{
    I2S::Bus::read(this->_packedInput, this->_size);
    return this->_packedInput;
}

inline void DataIngestor::_unpack_from_i2s()
{
    /* Read to ring buffer from I2S input */
    I2S::Bus::read(this->_packedInput, this->_size);
    unpack<int_audio>(this->_buff.get_write_buffer(), this->_packedInput);
    this->_buff.rotate_write_buffer(true);
}

size_t DataIngestor::step()
{
    /* Return number of samples read */
    _unpack_from_i2s();
    return this->_size;
}

int DataIngestor::buffered()
{
    return this->_buff.buffered();
}

uint8_t* DataIngestor::read()
{
    /* Output buffered data */
    if (is_pcm())
    {
        pack<int_audio>(this->_packedOutput, this->_buff.get_read_buffer());
    }
    if (is_floating_point())
    {
        int_to_float<int_audio, float>(
                reinterpret_cast<std::vector<float>*>(this->_packedOutput),
                this->_buff.get_read_buffer()
            );
    }
    else if (!is_padded())
    {
        return this->_buff.get_read_byte();
    }
    this->_buff.rotate_read_buffer();
    return this->_packedOutput;
}
