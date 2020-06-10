#include <algorithm>
#include <atomic>
#include <chrono>
#include "FreeRTOS.h"
#include "soc/sens_reg.h"
#include "esp_timer.h"
#include "driver/i2s.h"
#include "esp_task_wdt.h"
#include "EspRingBuffer.h"

// i2s physical pin assignment
#define I2S_IN_NUM          (I2S_NUM_0)
#define I2S_BCK_IO          (GPIO_NUM_32)
#define I2S_WS_IO           (GPIO_NUM_25)
#define I2S_DO_IO           (I2S_PIN_NO_CHANGE)
#define I2S_DI_IO           (GPIO_NUM_22)

// Number of bytes per i2s buffer
uint32_t i2sBytesPerBuffer = buff.bufferLength * f.sampleWidth;

// Number of buffer rotations per second
uint32_t bufferRate = f.sampleRate / buff.bufferLength;

// Records number of bytes read each time i2s_read is called
size_t numBytesRead(0);

// Semaphore to check bytes read from i2s
bool i2sCycled = false;

void configI2S()
{
    i2s_bits_per_sample_t i2sBitDepth;
    i2s_channel_fmt_t i2sChannelFormat;
    i2s_comm_format_t i2sSampleRate = static_cast<i2s_comm_format_t>(f.sampleRate);
    switch (f.bitDepth)
    {
        case (16):
            i2sBitDepth = I2S_BITS_PER_SAMPLE_16BIT;
            break;
        case (24):
            i2sBitDepth = I2S_BITS_PER_SAMPLE_24BIT;
            break;
        default:
            i2sBitDepth = I2S_BITS_PER_SAMPLE_32BIT;
    }
    switch (f.numChannels)
    {
        case (2):
            i2sChannelFormat = I2S_CHANNEL_FMT_RIGHT_LEFT;
            break;
        default:
            i2sChannelFormat = I2S_CHANNEL_FMT_ONLY_LEFT;
    }
    i2s_config_t i2s_config = {
            .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
            // .mode = static_cast<i2s_mode_t>(I2S_MODE_SLAVE | I2S_MODE_RX),
            .sample_rate = i2sSampleRate,
            .bits_per_sample = i2sBitDepth,
            .channel_format = i2sChannelFormat,
            .communication_format = static_cast<i2s_comm_format_t>(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
            // .intr_alloc_flags = 0, // .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3,
            .dma_buf_count = buff.ringLength,
            .dma_buf_len = static_cast<int>(buff.bytesPerBuffer),
            .use_apll = true
        };
    i2s_pin_config_t pin_config = {
            .bck_io_num = I2S_BCK_IO,
            .ws_io_num = I2S_WS_IO,
            .data_out_num = I2S_DO_IO,
            .data_in_num = I2S_DI_IO
        };
    i2s_driver_install(I2S_IN_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_IN_NUM, &pin_config);
    i2s_start(I2S_IN_NUM);
}

void read_i2s_loop(void* pvParameter)
{
    while (true)
    {
        i2s_read(
                I2S_IN_NUM,
                buff.get_write_void_ptr(),
                buff.bytesPerBuffer,
                &numBytesRead,
                portMAX_DELAY
            );
        buff.rotate_write_buffer_manual();
        i2sCycled = true;
    }
}

void starti2sInputLoop()
{
    xTaskCreate(&read_i2s_loop, "i2sReader", 4096, NULL, 8, NULL);
}

void check_i2s_bytes_read()
{
    if (!i2sCycled)
    {
        return;
    }
    #if DEBUG
    if (numBytesRead < buff.bytesPerBuffer)
    {
        uint32_t bytesDropped = (buff.bytesPerBuffer - numBytesRead) / f.sampleWidth;
        std::cerr << bytesDropped << " samples from i2s input buffer dropped" << std::endl;
    }
    else
    {
        std::cout << "Read i2s buffer successfully " << std::endl;
    }
    i2sCycled = false;
    #endif
}
