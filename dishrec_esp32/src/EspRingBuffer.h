#ifndef ESPRINGBUFFER_H
#define ESPRINGBUFFER_H

#include "RingBuffer.h"
#include "EspSDWavFile.h"

// Macros
#define SAMPLE_RATE         48000
#define BIT_DEPTH           16

#define BUFFER_LENGTH       48
#define RING_LENGTH         8

#if ((BIT_DEPTH) == 32)
#define DATATYPE            float
#define IS_FLOAT            true
#elif ((BIT_DEPTH) == 16)
#define DATATYPE            int16_t
#define IS_FLOAT            false
#elif ((BIT_DEPTH) == 8)
#define DATATYPE            uint8_t
#define IS_FLOAT            false
#endif

// External template delcarations
extern template class RingBuffer<DATATYPE>;

void write_to_file();
bool write_if_buffered();
void write_if_buffered_button(void* pvParameter);

#endif
