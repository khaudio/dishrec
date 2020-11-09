#ifndef WAVWRITER_H
#define WAVWRITER_H

#include <iostream>
#include "BWFHeader.h"

// #ifdef ESP32
// #include <SD.h>
// #else
// #include <fstream>
// #endif

#include <fstream>

enum wavfile_err
{
    INVALID_FILEPATH_LENGTH = 160
};

namespace WavFile
{

/* Number of bytes to pad top of wav file with for header */
constexpr size_t numBytesReservedForWavHeader(8192);

/* Max string length for filename of current file,
not including dot and extension */
constexpr int filenameCharBufferLength(4096);

class WavWriter : public BWFHeader::BroadcastWav
{
protected:
public:
    char _filename[filenameCharBufferLength + 5];
    std::fstream _filestream;

    void _new_file();
    void _close_file();
    void _append_to_file(const char* filepath);
    void _write_to_file(uint8_t* data, size_t numBytes);
    void _write_header(uint8_t* data, size_t numBytes);

public:
    void _set_filename(const char* fname);

public:
    WavWriter();
    ~WavWriter();

    int write_to_file(uint8_t* data, size_t length);
    template <typename T>
    int write_to_file(std::vector<T> data);

};

};

#endif
