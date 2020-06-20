#ifndef WAVFILE_H
#define WAVFILE_H

#include "WavHeader.h"
#include <SD.h>

#define SD_CS_PIN           5

class EspSDWavFile : public WavHeader
{
protected:
    bool _sdInitialized = false, _fileIsOpen = false;
    uint32_t _dataIndex;
    bool _initialize_sd();
public:
    File file;
    std::string filename;
    EspSDWavFile(WavParameters parameters);
    EspSDWavFile();
    ~EspSDWavFile();
    virtual void set_filename(std::string filename);
    bool open();
    bool open(std::string filename)
    bool is_open();
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
};

#endif
