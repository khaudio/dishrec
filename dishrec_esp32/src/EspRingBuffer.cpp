#include "EspRingBuffer.h"

// Default WAV file parameters (save/load EEPROM or SD)
WavParameters defaultWavFormat
{
    .sampleRate = SAMPLE_RATE,
    .bitDepth = BIT_DEPTH,
    .isFloatingPoint = IS_FLOAT,
    .numChannels = 1
};

// Rotatable ring buffers to store samples

// In from microphone
RingBuffer<DATATYPE> inBuffer(BUFFER_LENGTH, RING_LENGTH);

// Out to audio output
RingBuffer<DATATYPE> outBuffer(BUFFER_LENGTH, RING_LENGTH);

// File object to save data
EspSDWavFile f(defaultWavFormat);

void write_to_file()
{
    // Write from buffer to file
    static uint8_t* currentBufferPtr;
    currentBufferPtr = inBuffer.get_read_ptr();
    f.write(currentBufferPtr, inBuffer.bytesPerBuffer);
}

bool write_if_buffered()
{
    // Write data to file as soon as it is buffered
    bool written = false;
    while (inBuffer.buffered())
    {
        write_to_file();
        inBuffer.rotateReadBuffer();
        written = true;
    }
    return written;
}

void write_if_buffered_button(void* pvParameter)
{
    bool* recording = reinterpret_cast<bool*>(pvParameter);
    while (true)
    {
        if (*recording && f.is_open())
        {
            if (inBuffer.buffered())
            {
                write_to_file();
                inBuffer.rotateReadBuffer();
            }
        }
        else if (f.is_open())
        {
            #ifdef _DEBUG
            std::cout << "Closing file" << std::endl;
            #endif
            f.close();
        }
    }
}
