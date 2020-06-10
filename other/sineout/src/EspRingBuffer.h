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
#endif

// External template delcarations
extern template class RingBuffer<DATATYPE>;

// Default WAV file parameters (save/load EEPROM or SD)
WavParameters defaultWavFormat
{
    .sampleRate = SAMPLE_RATE,
    .bitDepth = BIT_DEPTH,
    .isFloatingPoint = IS_FLOAT,
    .numChannels = 1
};

// Rotatable Ring Buffer to use to store samples
RingBuffer<DATATYPE> buff(BUFFER_LENGTH, RING_LENGTH);

// File object to save data
EspSDWavFile f(defaultWavFormat);

void write_to_file()
{
    // Write from buffer to file
    static uint8_t* currentBufferPtr;
    currentBufferPtr = buff.get_read_ptr();
    f.write(currentBufferPtr, buff.bytesPerBuffer);
}

bool write_if_buffered()
{
    // Write data to file as soon as it is buffered
    bool written = false;
    while (buff.buffered())
    {
        write_to_file();
        buff.rotateReadBuffer();
        written = true;
    }
    return written;
}

#endif
