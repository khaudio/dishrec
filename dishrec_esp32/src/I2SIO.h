#ifndef I2SIO_H
#define I2SIO_H

#include <FreeRTOS.h>
#include <driver/i2s.h>
#include "EspRingBuffer.h"

/* I2S physical pin assignment */

#define I2S_IN_NUM              (I2S_NUM_0)
#define I2S_OUT_NUM             (I2S_NUM_1)

// Word Select
#define I2S_WS_IO               (GPIO_NUM_32)

// Bit clock
#define I2S_BCK_IO              (GPIO_NUM_33)

// Data output
#define I2S_DO_IO               (GPIO_NUM_12)

// Data input
#define I2S_DI_IO               (GPIO_NUM_25)

// Shutdown signal output
#define I2S_SHUTDOWN            (GPIO_NUM_21)

// Master clock output (MCK == 256 * BCK; out on GPIO0 only)
#define I2S_MCK                 (GPIO_NUM_0)

extern WavParameters defaultWavFormat;
extern RingBuffer<DATATYPE> inBuffer;
extern RingBuffer<DATATYPE> outBuffer;
extern std::shared_ptr<EspSDWavFile> currentFile;

namespace I2S
{

enum i2s_buffer_type
{
    READ_BUFFER = 1,
    WRITE_BUFFER = 2,
};

void config(RingBuffer<DATATYPE>* buff, i2s_buffer_type bufferType);
void read_config(RingBuffer<DATATYPE>* buff);
void read_to_buffer(void* pvParameter);
void write_from_buffer(void* pvParameter);
void start();
void check_bytes_read(RingBuffer<DATATYPE>* buff);
void check_bytes_written(RingBuffer<DATATYPE>* buff);

};

#endif
