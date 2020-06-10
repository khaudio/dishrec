#ifndef WAVFILE_H
#define WAVFILE_H

#include "WavHeader.h"
#include <SD.h>

#define SD_CS_PIN           5

class EspSDWavFile : public WavHeader
{
protected:
    bool sdInitialized = false, fileIsOpen = false;
    uint32_t dataIndex;
    bool initialize_sd();
public:
    File file;
    std::string filename;
    EspSDWavFile(WavParameters parameters);
    EspSDWavFile();
    ~EspSDWavFile();
    bool open(std::string filename);
    bool open(const char* filename);
    void seek(uint32_t position);
    void close();
    void write_header();
    uint8_t* get_data();
    void reinitialize();
    void write(uint8_t* data, uint32_t size);
    template <typename T>
    void write(std::vector<T>& data);
    template <typename T>
    void write(std::vector<T>* data);
    bool is_open();

    void reinterpret_sample_rate(uint32_t rate);
    void reinterpret_bit_depth(uint16_t bitsPerSecond, bool isFloat);
};

#endif
