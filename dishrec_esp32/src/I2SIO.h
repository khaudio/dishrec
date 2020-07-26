#ifndef I2SIO_H
#define I2SIO_H

#include <FreeRTOS.h>
#include <driver/i2s.h>
#include "EspRingBuffer.h"

/* I2S physical pin assignment */

// I2S port number
#define I2S_IN_NUM                  (I2S_NUM_1)

// Internal DAC only supported on I2S_NUM_0
#define I2S_OUT_NUM                 (I2S_NUM_0)

// Input word select
#define I2S_IN_WS                   (GPIO_NUM_32)

// Input bit clock
#define I2S_IN_BCK                  (GPIO_NUM_33)

// Input data
#define I2S_IN_DI                   (GPIO_NUM_39)

// Output word select
#define I2S_OUT_WS                  (GPIO_NUM_4)

// Output bit clock
#define I2S_OUT_BCK                 (GPIO_NUM_16)

// Output data
#define I2S_OUT_DO                  (GPIO_NUM_25)

// Master clock output (MCK == 256 * BCK; out on GPIO0 only)
#define I2S_MCK                     (GPIO_NUM_0)

// Shutdown signal output
#define I2S_SHUTDOWN                (GPIO_NUM_21)

extern WavParameters currentWavFormat;
extern RingBuffer<DATATYPE> inBuffer;
extern RingBuffer<DATATYPE> outBuffer;
extern std::shared_ptr<EspSDWavFile> currentWriteFile;
extern std::shared_ptr<EspSDWavFile> currentReadFile;

namespace I2S
{

enum i2s_buffer_type
{
    READ_BUFFER = 1,
    WRITE_BUFFER = 2,
};

void config_read(RingBuffer<DATATYPE>* buff);
void config_write(RingBuffer<DATATYPE>* buff);
template <typename T>
void read_to_buffer(RingBuffer<T>*);
template <typename T>
void write_from_buffer(RingBuffer<T>*);
// void read_to_buffer(void* pvParameter);
// void write_from_buffer(void* pvParameter);
void start();
void check_bytes_read(RingBuffer<DATATYPE>* buff);
void check_bytes_written(RingBuffer<DATATYPE>* buff);

};

#endif
