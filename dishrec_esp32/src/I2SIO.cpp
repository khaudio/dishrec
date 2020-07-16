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

void I2S::config(RingBuffer<DATATYPE>* buff, i2s_buffer_type bufferType)
{
    i2s_bits_per_sample_t i2sBitDepth;
    i2s_channel_fmt_t i2sChannelFormat;
    i2s_comm_format_t i2sSampleRate = static_cast<i2s_comm_format_t>(currentFile->sampleRate);
    switch (currentFile->bitDepth)
    {
        case(8):
            i2sBitDepth = I2S_BITS_PER_SAMPLE_8BIT;
            break;
        case (16):
            i2sBitDepth = I2S_BITS_PER_SAMPLE_16BIT;
            break;
        case (24):
            i2sBitDepth = I2S_BITS_PER_SAMPLE_24BIT;
            break;
        default:
            i2sBitDepth = I2S_BITS_PER_SAMPLE_32BIT;
    }
    switch (currentFile->numChannels)
    {
        case (2):
            i2sChannelFormat = I2S_CHANNEL_FMT_RIGHT_LEFT;
            break;
        default:
            i2sChannelFormat = I2S_CHANNEL_FMT_ONLY_LEFT;
    }
    i2s_mode_t i2sMode = static_cast<i2s_mode_t>(
            I2S_MODE_MASTER | ((bufferType == (READ_BUFFER)) ? (I2S_MODE_RX) : (I2S_MODE_TX))
        );
    i2s_config_t i2sConfig = {
            mode : i2sMode,
            sample_rate : i2sSampleRate,
            bits_per_sample : i2sBitDepth,
            channel_format : i2sChannelFormat,
            communication_format : static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            intr_alloc_flags : ((bufferType == (READ_BUFFER)) ? ESP_INTR_FLAG_LEVEL3 : 0),
            dma_buf_count : buff->ringLength,
            dma_buf_len : static_cast<int>(buff->bytesPerBuffer),
            use_apll : true
        };
    i2s_pin_config_t i2sPinConfig = {
            bck_io_num : I2S_BCK_IO,
            ws_io_num : I2S_WS_IO,
            data_out_num : (bufferType == (WRITE_BUFFER)) ? (I2S_DO_IO) : (I2S_PIN_NO_CHANGE),
            data_in_num : (bufferType == (READ_BUFFER)) ? (I2S_DI_IO) : (I2S_PIN_NO_CHANGE)
        };
    i2s_driver_install((bufferType == READ_BUFFER) ? I2S_IN_NUM : I2S_OUT_NUM, &i2sConfig, 0, NULL);
    i2s_set_pin((bufferType == READ_BUFFER) ? I2S_IN_NUM : I2S_OUT_NUM, &i2sPinConfig);
}

void I2S::read_config(RingBuffer<DATATYPE>* buff)
{
    i2s_config_t i2sConfig = {
            mode : static_cast<i2s_mode_t>(I2S_MODE_SLAVE | I2S_MODE_RX),
            sample_rate : static_cast<i2s_comm_format_t>(SAMPLE_RATE),
            bits_per_sample : I2S_BITS_PER_SAMPLE_16BIT,
            channel_format : I2S_CHANNEL_FMT_ONLY_LEFT,
            communication_format : static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
            dma_buf_count : buff->ringLength,
            dma_buf_len : static_cast<int>(buff->bytesPerBuffer),
            use_apll : true
        };
    i2s_pin_config_t i2sPinConfig = {
            bck_io_num : I2S_BCK_IO,
            ws_io_num : I2S_WS_IO,
            data_out_num : I2S_PIN_NO_CHANGE,
            data_in_num : I2S_DI_IO
        };
    i2s_driver_install(I2S_IN_NUM, &i2sConfig, 0, NULL);
    i2s_set_pin(I2S_IN_NUM, &i2sPinConfig);
}

void I2S::read_to_buffer(void* pvParameter)
{
    RingBuffer<DATATYPE>* buff = reinterpret_cast<RingBuffer<DATATYPE>*>(pvParameter);
    while (true)
    {
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
}

void I2S::write_from_buffer(void* pvParameter)
{
    RingBuffer<DATATYPE>* buff = reinterpret_cast<RingBuffer<DATATYPE>*>(pvParameter);
    while (true)
    {
        i2s_write(
                I2S_OUT_NUM,
                buff->get_read_void_ptr(),
                buff->bytesPerBuffer,
                &numBytesWritten,
                portMAX_DELAY
            );
        buff->rotate_read_buffer();
        #ifdef _DEBUG
        i2sWriteCycled = true;
        #endif
    }
}

void I2S::start()
{
    // Set MCK output
    // PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    // REG_WRITE(PIN_CTRL, 0);

    config(&inBuffer, READ_BUFFER);
    // config(&outBuffer, WRITE_BUFFER);
    // read_config(&inBuffer);
    ets_delay_us(500000); // 500ms delay
    xTaskCreate(&read_to_buffer, "i2sIn", 20000, &inBuffer, configMAX_PRIORITIES, NULL);
    // xTaskCreate(&write_from_buffer, "i2sOut", 10000, &outBuffer, 7, NULL);
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
        uint32_t bytesDropped = (buff->bytesPerBuffer - numBytesRead) / currentFile->sampleWidth;
        std::cerr << bytesDropped << " samples from I2S input buffer dropped (" << numBytesRead << " bytes read)" << std::endl;
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
        uint32_t bytesDropped = (buff->bytesPerBuffer - numBytesWritten) / currentFile->sampleWidth;
        std::cerr << bytesDropped << " samples from I2S output buffer dropped (" << numBytesWritten << " bytes written)" << std::endl;
    }
    else
    {
        std::cout << "Wrote to I2S buffer successfully " << std::endl;
    }
    i2sWriteCycled = false;
    #endif
}
