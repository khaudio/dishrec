#include "EspI2S.h"

I2SBus::I2SBus() :
_i2sNum(I2S_NUM_1),
_mode(I2S_MODE_MASTER | I2S_MODE_RX),
_pinsSet(false),
_shutdownPinSet(false),
_bufferLength(0)
{
    this->sampleRate = 0;
    this->bitDepth = 0;
    this->numChannels = 0;
    this->sampleWidth = 0;

    // Default to PCM
    this->formatCode = WavMeta::FORMAT_PCM;
}

I2SBus::I2SBus(int i2sNum, bool isMaster, bool receives, bool transmits) :
I2SBus()
{
    this->_i2sNum = i2sNum ? I2S_NUM_1 : I2S_NUM_0;
    this->_mode = isMaster ? I2S_MODE_MASTER : I2S_MODE_SLAVE;
    if (receives) this->_mode |= I2S_MODE_RX;
    if (transmits) this->_mode |= I2S_MODE_TX;
}

I2SBus::~I2SBus()
{
    shutdown();
}

i2s_bits_per_sample_t I2SBus::_get_i2s_bit_depth() const
{
    switch (this->bitDepth)
    {
        case (8):
            return I2S_BITS_PER_SAMPLE_8BIT;
        case (16):
            return I2S_BITS_PER_SAMPLE_16BIT;
        case (24):
            return I2S_BITS_PER_SAMPLE_24BIT;
        case (32):
            return I2S_BITS_PER_SAMPLE_32BIT;
        default:
            throw std::out_of_range("Unsupported bit depth");
    }
}

i2s_channel_fmt_t I2SBus::_get_i2s_channel_format() const
{
    switch (this->numChannels)
    {
        case (2):
            return I2S_CHANNEL_FMT_RIGHT_LEFT;
        case (1):
            return I2S_CHANNEL_FMT_ONLY_LEFT;
        default:
            throw std::out_of_range("Unsupported number of channels");
    }
}

inline void I2SBus::_check_format()
{
    if (
            !this->sampleRate
            || !this->bitDepth
            || !this->numChannels
            || !this->_bufferLength
            || !this->_pinsSet
        )
    {
        throw WavMeta::FORMAT_NOT_SET;
    }
}

void I2SBus::set_i2s_num(int i2sNum)
{
    this->_i2sNum = static_cast<i2s_port_t>(i2sNum);
}

void I2SBus::reset_mode()
{
    this->_mode = 0;
}

void I2SBus::set_master()
{
    this->_mode |= I2S_MODE_MASTER;
}

void I2SBus::set_slave()
{
    this->_mode |= I2S_MODE_SLAVE;
}

void I2SBus::set_transmit()
{
    this->_mode |= I2S_MODE_TX;
}

void I2SBus::set_receive()
{
    this->_mode |= I2S_MODE_RX;
}

void I2SBus::set_buffer_size(size_t bufferSizeInSamples)
{
    this->_bufferLength = bufferSizeInSamples;
}

void I2SBus::set_pins(int wsPin, int bckPin, int dataInPin, int dataOutPin)
{
    this->_pinConfig.ws_io_num = wsPin;
    this->_pinConfig.bck_io_num = bckPin;
    this->_pinConfig.data_in_num = dataInPin;
    this->_pinConfig.data_out_num = dataOutPin;
    this->_pinsSet = true;
}

void I2SBus::set_shutdown_pin(int sdPin, bool activeHigh)
{
    this->_shutdownPin = sdPin;
    this->_shutdownState = activeHigh;

    this->_shutdownPinSet = true;
}

void I2SBus::config()
{
    #ifdef _DEBUG
    _check_format();
    #endif

    this->_i2sConfig.mode = static_cast<i2s_mode_t>(this->_mode);
    this->_i2sConfig.sample_rate = this->sampleRate;
    this->_i2sConfig.bits_per_sample = _get_i2s_bit_depth();
    this->_i2sConfig.channel_format = _get_i2s_channel_format();
    this->_i2sConfig.communication_format = I2S_COMM_FORMAT_I2S_MSB;
    this->_i2sConfig.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
    this->_i2sConfig.dma_buf_count = 4;
    this->_i2sConfig.dma_buf_len = this->_bufferLength;
    this->_i2sConfig.use_apll = true;
    this->_i2sConfig.tx_desc_auto_clear = true;
}

void I2SBus::start()
{
    #ifdef _DEBUG
    _check_format();
    #endif

    // Start the I2S Bus
    i2s_driver_install(this->_i2sNum, &this->_i2sConfig, 0, NULL);

    // Set GPIO pins
    i2s_set_pin(this->_i2sNum, &this->_pinConfig);

    // Enable MCK output
    REG_WRITE(PIN_CTRL, 0xFF0); 
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
}

inline void I2SBus::stop()
{
    i2s_driver_uninstall(this->_i2sNum);
}

void I2SBus::shutdown()
{
    if (this->_shutdownPinSet)
    {
        gpio_set_level(
                static_cast<gpio_num_t>(this->_shutdownPin),
                this->_shutdownState
            );
    }
    stop();
    if (this->_shutdownPinSet)
    {
        gpio_set_level(
                static_cast<gpio_num_t>(this->_shutdownPin),
                !this->_shutdownState
            );
    }
}

inline bool I2SBus::read(uint8_t* buff, size_t numBytes)
{
    static size_t numBytesRead(0);
    esp_err_t status = i2s_read(
            this->_i2sNum, buff, numBytes, &numBytesRead, portMAX_DELAY
        );
    #ifdef _DEBUG
    if (status != ESP_OK)
    {
        std::cerr << "Error reading from I2S; ";
        std::cerr << numBytesRead << " of ";
        std::cerr << numBytes << " bytes read" << std::endl;
    }
    #endif
    return (status == ESP_OK);
}

template <typename T>
inline bool I2SBus::read(std::vector<T>* buff)
{
    constexpr size_t datatypeSize(sizeof(T));
    return read(
            reinterpret_cast<uint8_t*>(&buff->at(0)),
            buff->size() * datatypeSize
        );
}

inline bool I2SBus::write(const uint8_t* data, size_t numBytes)
{
    static size_t numBytesWritten(0);
    esp_err_t status = i2s_write(
            this->_i2sNum, data, numBytes, &numBytesWritten, portMAX_DELAY
        );
    #ifdef _DEBUG
    if (status != ESP_OK)
    {
        std::cerr << "Error writing to I2S; ";
        std::cerr << numBytesWritten << " of ";
        std::cerr << numBytes << " bytes written" << std::endl;
    }
    #endif
    return (status == ESP_OK);
}

template <typename T>
inline bool I2SBus::write(std::vector<T>* data)
{
    constexpr size_t datatypeSize(sizeof(T));
    return write(
            reinterpret_cast<uint8_t*>(&data->at(0)),
            data->size() * datatypeSize
        );
}

template <typename T>
void I2SBus::operator<<(std::vector<T>* data)
{
    write(data);
}

template <typename T>
void I2SBus::operator>>(std::vector<T>* buff)
{
    read(buff);
}


template bool I2SBus::read<int8_t>(std::vector<int8_t>*);
template bool I2SBus::read<uint8_t>(std::vector<uint8_t>*);
template bool I2SBus::read<int16_t>(std::vector<int16_t>*);
template bool I2SBus::read<uint16_t>(std::vector<uint16_t>*);
template bool I2SBus::read<int32_t>(std::vector<int32_t>*);
template bool I2SBus::read<uint32_t>(std::vector<uint32_t>*);
template bool I2SBus::read<int64_t>(std::vector<int64_t>*);
template bool I2SBus::read<uint64_t>(std::vector<uint64_t>*);

template bool I2SBus::write<int8_t>(std::vector<int8_t>*);
template bool I2SBus::write<uint8_t>(std::vector<uint8_t>*);
template bool I2SBus::write<int16_t>(std::vector<int16_t>*);
template bool I2SBus::write<uint16_t>(std::vector<uint16_t>*);
template bool I2SBus::write<int32_t>(std::vector<int32_t>*);
template bool I2SBus::write<uint32_t>(std::vector<uint32_t>*);
template bool I2SBus::write<int64_t>(std::vector<int64_t>*);
template bool I2SBus::write<uint64_t>(std::vector<uint64_t>*);

template void I2SBus::operator<<<int8_t>(std::vector<int8_t>*);
template void I2SBus::operator<<<uint8_t>(std::vector<uint8_t>*);
template void I2SBus::operator<<<int16_t>(std::vector<int16_t>*);
template void I2SBus::operator<<<uint16_t>(std::vector<uint16_t>*);
template void I2SBus::operator<<<int32_t>(std::vector<int32_t>*);
template void I2SBus::operator<<<uint32_t>(std::vector<uint32_t>*);
template void I2SBus::operator<<<int64_t>(std::vector<int64_t>*);
template void I2SBus::operator<<<uint64_t>(std::vector<uint64_t>*);

template void I2SBus::operator>><int8_t>(std::vector<int8_t>*);
template void I2SBus::operator>><uint8_t>(std::vector<uint8_t>*);
template void I2SBus::operator>><int16_t>(std::vector<int16_t>*);
template void I2SBus::operator>><uint16_t>(std::vector<uint16_t>*);
template void I2SBus::operator>><int32_t>(std::vector<int32_t>*);
template void I2SBus::operator>><uint32_t>(std::vector<uint32_t>*);
template void I2SBus::operator>><int64_t>(std::vector<int64_t>*);
template void I2SBus::operator>><uint64_t>(std::vector<uint64_t>*);
