#include "I2SIO.h"

// Records number of bytes read each time i2s_read is called
static size_t numBytesRead(0);

// Records number of bytes written each time i2s_write is called
static size_t numBytesWritten(0);

#ifdef _DEBUG
// Whether an I2S read from DMA has occured since last check
static bool i2sReadCycled = false;

// Whether an I2S write to DMA has occured since last check
static bool i2sWriteCycled = false;
#endif

i2s_channel_fmt_t get_channel_format(uint16_t numChannels)
{
    switch (numChannels)
    {
        case (2):
            return I2S_CHANNEL_FMT_RIGHT_LEFT;
            break;
        default:
            return I2S_CHANNEL_FMT_ONLY_LEFT;
    }
}

i2s_bits_per_sample_t get_bit_depth(uint16_t bitsPerSample)
{
    switch (bitsPerSample)
    {
        case(8):
            return I2S_BITS_PER_SAMPLE_8BIT;
            break;
        case (16):
            return I2S_BITS_PER_SAMPLE_16BIT;
            break;
        case (24):
            return I2S_BITS_PER_SAMPLE_24BIT;
            break;
        default:
            return I2S_BITS_PER_SAMPLE_32BIT;
    }
}

void I2S::config_read(RingBuffer<DATATYPE>* buff)
{
    i2s_config_t i2sConfig = {
            mode : static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
            sample_rate : static_cast<i2s_comm_format_t>(currentWavFormat.sampleRate),
            bits_per_sample : get_bit_depth(currentWavFormat.bitDepth),
            channel_format : get_channel_format(currentWavFormat.numChannels),
            communication_format : static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            intr_alloc_flags : ESP_INTR_FLAG_LEVEL2,
            dma_buf_count : buff->ringLength,
            dma_buf_len : static_cast<int>(buff->bytesPerBuffer),
            use_apll : true
        };
    i2s_pin_config_t i2sPinConfig = {
            bck_io_num : I2S_IN_BCK,
            ws_io_num : I2S_IN_WS,
            data_out_num : I2S_PIN_NO_CHANGE,
            data_in_num : I2S_IN_DI
        };
    i2s_driver_install(I2S_IN_NUM, &i2sConfig, 0, NULL);
    i2s_set_pin(I2S_IN_NUM, &i2sPinConfig);
}

void I2S::config_write(RingBuffer<DATATYPE>* buff)
{
    i2s_config_t i2sConfig = {
            mode : static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX),
            sample_rate : static_cast<i2s_comm_format_t>(currentWavFormat.sampleRate),
            bits_per_sample : get_bit_depth(currentWavFormat.bitDepth),
            channel_format : get_channel_format(currentWavFormat.numChannels),
            communication_format : (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
            dma_buf_count : buff->ringLength,
            dma_buf_len : static_cast<int>(buff->bytesPerBuffer),
            use_apll : true
        };
    i2s_pin_config_t i2sPinConfig = {
            bck_io_num : I2S_OUT_BCK,
            ws_io_num : I2S_OUT_WS,
            data_out_num : I2S_OUT_DO,
            data_in_num : I2S_PIN_NO_CHANGE
        };
    i2s_driver_install(I2S_OUT_NUM, &i2sConfig, 0, NULL);
    i2s_set_pin(I2S_OUT_NUM, &i2sPinConfig);

    // i2s_set_sample_rates(I2S_OUT_NUM, currentWavFormat.sampleRate); 
}

void I2S::read_to_buffer(void* pvParameter)
{
    RingBuffer<DATATYPE>* buff = reinterpret_cast<RingBuffer<DATATYPE>*>(pvParameter);
    i2s_read(
            I2S_IN_NUM,
            reinterpret_cast<uint8_t*>(buff->get_write_ptr()),
            buff->bytesPerBuffer,
            &numBytesRead,
            portMAX_DELAY
        );
    buff->rotate_write_buffer_manual();
    #ifdef _DEBUG
    i2sReadCycled = true;
    #endif
}

void I2S::write_from_buffer(void* pvParameter)
{
    RingBuffer<DATATYPE>* buff = reinterpret_cast<RingBuffer<DATATYPE>*>(pvParameter);
    std::vector<DATATYPE> vec = buff->read();
    const void* buffptr = reinterpret_cast<const void*>(&(vec[0]));
    i2s_write(
            I2S_OUT_NUM,
            buffptr,
            buff->bytesPerBuffer,
            &numBytesWritten,
            portMAX_DELAY
        );
    #ifdef _DEBUG
    i2sWriteCycled = true;
    #endif
}

void I2S::start()
{
    // Set MCK output
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    REG_WRITE(PIN_CTRL, 0);

    // Config I2S input
    config_read(&inBuffer);
    
    // Config I2S output
    config_write(&outBuffer);
}

void I2S::check_bytes_read(RingBuffer<DATATYPE>* buff)
{
    #ifdef _DEBUG
    if (!i2sReadCycled)
    {
        return;
    }
    if (numBytesRead < buff->bytesPerBuffer)
    {
        uint32_t bytesDropped = (
                (buff->bytesPerBuffer - numBytesRead) / currentFile->sampleWidth
            );
        std::cerr << bytesDropped << " samples from I2S input buffer dropped (";
        std::cerr << numBytesRead << " bytes read)" << std::endl;
    }
    else
    {
        std::cout << "Read from I2S buffer successfully " << std::endl;
    }
    i2sReadCycled = false;
    #endif
}

void I2S::check_bytes_written(RingBuffer<DATATYPE>* buff)
{
    #ifdef _DEBUG
    if (!i2sWriteCycled)
    {
        return;
    }
    if (numBytesWritten < buff->bytesPerBuffer)
    {
        uint32_t bytesDropped = (
                (buff->bytesPerBuffer - numBytesWritten) / currentFile->sampleWidth
            );
        std::cerr << bytesDropped << " samples from I2S output buffer dropped (";
        std::cerr << numBytesWritten << " bytes written)" << std::endl;
    }
    else
    {
        std::cout << "Wrote to I2S buffer successfully " << std::endl;
    }
    i2sWriteCycled = false;
    #endif
}
