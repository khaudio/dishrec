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
std::shared_ptr<EspSDWavFile> currentWriteFile;
std::shared_ptr<EspSDWavFile> currentReadFile = nullptr;
std::string currentWriteFilename("arbitrary_filename");
std::string filenameDelimiter("_");
int currentWriteFileNumber = 0;


void create_buffers(int bufferLength, uint8_t ringSize, uint16_t bitDepth)
{
    switch (bitDepth)
    {
        if (bitDepth == 16)
        {
            std::shared_ptr<RingBuffer<uint16_t>> inBuffer = std::make_shared<RingBuffer<uint16_t>>(bufferLength, ringSize);
            std::shared_ptr<RingBuffer<uint16_t>> outBuffer = std::make_shared<RingBuffer<uint16_t>>(bufferLength, ringSize);
        }
        // else if (bitDepth == 24)
        // {
        //     std::shared_ptr<RingBuffer<int24_t>> inBuffer = std::make_shared<RingBuffer<int24_t>>(bufferLength, ringSize);
        //     std::shared_ptr<RingBuffer<int24_t>> outBuffer = std::make_shared<RingBuffer<int24_t>>(bufferLength, ringSize);
        // }
        else if (bitDepth == 32)
        {
            std::shared_ptr<RingBuffer<float>> inBuffer = std::make_shared<RingBuffer<float>>(bufferLength, ringSize);
            std::shared_ptr<RingBuffer<float>> outBuffer = std::make_shared<RingBuffer<float>>(bufferLength, ringSize);
        }
        else if (bitDepth == 64)
        {
            std::shared_ptr<RingBuffer<double>> inBuffer = std::make_shared<RingBuffer<double>>(bufferLength, ringSize);
            std::shared_ptr<RingBuffer<double>> outBuffer = std::make_shared<RingBuffer<double>>(bufferLength, ringSize);
        }
        else
        {
            throw INVALID_BIT_DEPTH;
        }
    }
}

void init_wav_file_objects()
{
    currentWriteFile = std::make_shared<EspSDWavFile>(currentWavFormat);
    currentReadFile = std::make_shared<EspSDWavFile>(currentWavFormat);
}

void open_new_file()
{
    std::stringstream ss;
    currentWriteFile = std::make_shared<EspSDWavFile>(currentWavFormat);
    ss << currentWriteFilename << filenameDelimiter << std::setw(3);
    ss << std::setfill('0') << ++currentWriteFileNumber;
    currentWriteFile->set_filename(ss.str());
    if (!currentWriteFile->open_write())
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
    currentWriteFile->write(currentReadBufferPtr, inBuffer.bytesPerBuffer);
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

template<typename T>
void read_file_to_buffer(RingBuffer<T>* buff)
{
    if (currentReadFile->is_open())
    {
        size_t bytesAvailableFromFile = currentReadFile->available();
        size_t chunkSize = (
                (bytesAvailableFromFile < buff->bytesPerBuffer)
                ? bytesAvailableFromFile : buff->bytesPerBuffer
            );
        if (buff->writable() && chunkSize)
        {
            std::vector<T> data = currentReadFile->read<T>(chunkSize);
            buff->write(data);
        }
    }
}

template <typename T>
void buffer_from_sd_card(RingBuffer<T>* buff)
{
    if (!currentReadFile->is_open()) return;
    else if ((buff->sub_buffers_full() < buff->ringLength) && currentReadFile->available())
    {
        size_t bytesAvailableFromFile = currentReadFile->available();
        size_t chunkSize = (
                (bytesAvailableFromFile < buff->bytesPerBuffer)
                ? bytesAvailableFromFile : buff->bytesPerBuffer
            ) / currentReadFile->sampleWidth;
        std::vector<T> data = currentReadFile->read<T>(chunkSize);
        buff->write(data);
    }
    else if (!currentReadFile->available())
    {
        #ifdef _DEBUG
        std::cout << "Closing read file..." << std::endl;
        #endif
        currentReadFile->close();
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

template void read_file_to_buffer<float>(RingBuffer<float>*);
template void read_file_to_buffer<double>(RingBuffer<double>*);
template void read_file_to_buffer<long double>(RingBuffer<long double>*);
template void read_file_to_buffer<int8_t>(RingBuffer<int8_t>*);
template void read_file_to_buffer<uint8_t>(RingBuffer<uint8_t>*);
template void read_file_to_buffer<int16_t>(RingBuffer<int16_t>*);
template void read_file_to_buffer<uint16_t>(RingBuffer<uint16_t>*);
template void read_file_to_buffer<int32_t>(RingBuffer<int32_t>*);
template void read_file_to_buffer<uint32_t>(RingBuffer<uint32_t>*);
template void read_file_to_buffer<int64_t>(RingBuffer<int64_t>*);
template void read_file_to_buffer<uint64_t>(RingBuffer<uint64_t>*);

template void buffer_from_sd_card<float>(RingBuffer<float>*);
template void buffer_from_sd_card<double>(RingBuffer<double>*);
template void buffer_from_sd_card<long double>(RingBuffer<long double>*);
template void buffer_from_sd_card<int8_t>(RingBuffer<int8_t>*);
template void buffer_from_sd_card<uint8_t>(RingBuffer<uint8_t>*);
template void buffer_from_sd_card<int16_t>(RingBuffer<int16_t>*);
template void buffer_from_sd_card<uint16_t>(RingBuffer<uint16_t>*);
template void buffer_from_sd_card<int32_t>(RingBuffer<int32_t>*);
template void buffer_from_sd_card<uint32_t>(RingBuffer<uint32_t>*);
template void buffer_from_sd_card<int64_t>(RingBuffer<int64_t>*);
template void buffer_from_sd_card<uint64_t>(RingBuffer<uint64_t>*);
