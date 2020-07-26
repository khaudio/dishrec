#include "EspRingBuffer.h"

// Default WAV file parameters (save/load EEPROM and/or SD)
WavParameters currentWavFormat
{
    sampleRate : CURRENT_SAMPLE_RATE,
    bitDepth : CURRENT_BIT_DEPTH,
    isFloatingPoint : CURRENT_IS_FLOAT,
    numChannels : CURRENT_NUM_CHANNELS
};

/* Rotatable ring buffers to store samples */

// In from microphone
RingBuffer<DATATYPE> inBuffer(BUFFER_LENGTH, RING_LENGTH);

// Out to audio output
RingBuffer<DATATYPE> outBuffer(BUFFER_LENGTH, RING_LENGTH);

// File object to save data
std::shared_ptr<EspSDWavFile> currentFile;
std::string currentFilename("arbitrary_filename");
std::string filenameDelimiter("_");
int currentFilenumber = 0;

void open_new_file()
{
    std::stringstream ss;
    currentFile = std::make_shared<EspSDWavFile>(currentWavFormat);
    ss << currentFilename << filenameDelimiter << std::setw(3);
    ss << std::setfill('0') << ++currentFilenumber;
    currentFile->set_filename(ss.str());
    if (!currentFile->open_write())
    {
        std::cerr << "Could not open new file" << std::endl;
        exit(1);
    }
    else
    {
        std::cout << "New file opened" << std::endl;
    }
}

void write_to_file()
{
    // Write from buffer to file
    uint8_t* currentReadBufferPtr = inBuffer.get_read_ptr();
    currentFile->write(currentReadBufferPtr, inBuffer.bytesPerBuffer);
}

bool write_if_buffered()
{
    // Write data to file as soon as it is buffered
    bool written = false;
    while (inBuffer.buffered())
    {
        write_to_file();
        inBuffer.rotate_read_buffer();
        written = true;
        esp_task_wdt_reset();
    }
    return written;
}

void read_file_to_buffer(size_t length)
{
    while (currentFile->is_open() && length)
    {
        if (outBuffer.writable())
        {
            size_t iterationSize = (
                    (length >= outBuffer.bytesPerBuffer)
                    ? outBuffer.bytesPerBuffer : length
                );
            std::vector<DATATYPE> data = currentFile->read<DATATYPE>(iterationSize);
            outBuffer.write(data);
            length -= iterationSize;
        }
    }
}

void print_buffer()
{
    uint8_t* readPtr = inBuffer.get_read_ptr();
    std::cout << "Buffer contents: ";
    for (int i(0); i < inBuffer.bufferLength; ++i)
    {
        std::cout << +(readPtr[i]);
    }
    std::cout << std::endl;
}
