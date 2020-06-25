#ifndef ESPSDWAVFILE_H
#define ESPSDWAVFILE_H

#include "WavHeader.h"
#include <SD.h>

/*
GPIO pinout for SD card

SPI:

    CS          5
    MOSI        23
    MISO        19
    CLK         18

SDMMC:

SDMMC_HOST_SLOT_0
    8-bit
    Unusable on WROOM and WROVER modules (Shares pins with SPI flash memory)
SDMMC_HOST_SLOT_1
    4-bit

    Signal      Slot 0      Slot 1
    CMD         11          15
    CLK         6           14
    D0          7           2
    D1          8           4
    D2          9           12
    D3          10          13
    D4          16
    D5          17
    D6          5
    D7          18
    CD          Any
    WP          Any
*/

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
    bool open(std::string filename);
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
