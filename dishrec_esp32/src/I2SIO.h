#ifndef I2SIO_H
#define I2SIO_H

#include <FreeRTOS.h>
#include <driver/i2s.h>
#include "EspRingBuffer.h"

/* I2S physical pin assignment */

// I2S port number (Internal DAC only supported on I2S_NUM_0)
#ifndef I2S_NUM
#define I2S_NUM                     (I2S_NUM_0)
#endif

// Input word select
#ifndef I2S_WS
#define I2S_WS                      (GPIO_NUM_32)
#endif

// Input bit clock
#ifndef I2S_BCK
#define I2S_BCK                     (GPIO_NUM_33)
#endif

// Input data
#ifndef I2S_DI
#define I2S_DI                      (GPIO_NUM_39)
#endif

// Output data
#ifndef I2S_DO
#define I2S_DO                      (GPIO_NUM_25)
#endif

// Master clock output (MCK == 256 * BCK; out on GPIO0 only)
#ifndef I2S_MCK
#define I2S_MCK                     (GPIO_NUM_0)
#endif

// Shutdown signal output
#ifndef I2S_SHUTDOWN
#define I2S_SHUTDOWN                (GPIO_NUM_21)
#endif

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

void config(RingBuffer<DATATYPE>* buff);
template <typename T>
void read_to_buffer(RingBuffer<T>*);
template <typename T>
void write_from_buffer(RingBuffer<T>*);
void start();
void stop();
void check_bytes_read(RingBuffer<DATATYPE>* buff);
void check_bytes_written(RingBuffer<DATATYPE>* buff);

};

#endif
