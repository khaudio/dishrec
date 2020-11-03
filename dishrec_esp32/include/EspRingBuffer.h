// #ifndef ESPRINGBUFFER_H
// #define ESPRINGBUFFER_H

// #include "RingBuffer.h"
// #include "EspSDWavFile.h"
// #include "WDT.h"

// // Temp macros until refactored for dynamic vars
// #define CURRENT_SAMPLE_RATE         48000
// #define CURRENT_BIT_DEPTH           16
// #define CURRENT_NUM_CHANNELS        2

// #define BUFFER_LENGTH               240
// #define RING_LENGTH                 16

// #if ((CURRENT_BIT_DEPTH) == 32)
// #define DATATYPE                    float
// #define CURRENT_IS_FLOAT            true
// #elif ((CURRENT_BIT_DEPTH) == 16)
// #define DATATYPE                    int16_t
// #define CURRENT_IS_FLOAT            false
// #elif ((CURRENT_BIT_DEPTH) == 8)
// #define DATATYPE                    uint8_t
// #define CURRENT_IS_FLOAT            false
// #endif

// // External template delcarations
// extern template class RingBuffer<DATATYPE>;

// void create_buffers(int bufferLength, uint8_t ringSize, uint16_t bitDepth);
// void init_wav_file_objects();
// void open_new_file();
// void write_to_file();
// bool write_if_buffered();
// template<typename T>
// void read_file_to_buffer(RingBuffer<T>* buff);
// template<typename T>
// void buffer_from_sd_card(RingBuffer<T>* buff);
// // void read_file_to_buffer(size_t length = 1);
// void print_buffer();

// #endif
