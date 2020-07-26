#ifndef ESPRINGBUFFER_H
#define ESPRINGBUFFER_H

#include "RingBuffer.h"
#include "EspSDWavFile.h"
#include "WDT.h"

// Macros
#define CURRENT_SAMPLE_RATE         48000
#define CURRENT_BIT_DEPTH           16
#define CURRENT_NUM_CHANNELS        1

#define BUFFER_LENGTH               240
#define RING_LENGTH                 16

#if ((CURRENT_BIT_DEPTH) == 32)
#define DATATYPE                    float
#define CURRENT_IS_FLOAT            true
#elif ((CURRENT_BIT_DEPTH) == 16)
#define DATATYPE                    int16_t
#define CURRENT_IS_FLOAT            false
#elif ((CURRENT_BIT_DEPTH) == 8)
#define DATATYPE                    uint8_t
#define CURRENT_IS_FLOAT            false
#endif

// External template delcarations
extern template class RingBuffer<DATATYPE>;

void init_wav_file_objects();
void open_new_file();
void write_to_file();
bool write_if_buffered();
void read_to_buffer();
void print_buffer();

#endif
