#ifndef ESPSDWAVFILE_H
#define ESPSDWAVFILE_H

#include "WavHeader.h"
#include <SD.h>
#include <tuple>

/* GPIO pinout for SD card */

/* SPI:

    CS                              Any
    MOSI                            23
    MISO                            19
    CLK                             18
*/

#define SD_CS_PIN                   (GPIO_NUM_5)

/* SDMMC:

    SDMMC_HOST_SLOT_0

        8-bit

        Unusable on WROOM and WROVER modules,
        as it shares pins with SPI flash memory.

    SDMMC_HOST_SLOT_1

        4-bit

        Signal                      Slot 0      Slot 1

        CMD                         11          15
        CLK                         6           14
        D0                          7           2
        D1                          8           4
        D2                          9           12
        D3                          10          13
        D4                          16
        D5                          17
        D6                          5
        D7                          18
        CD                          Any
        WP                          Any
*/

enum file_open_mode
{
    FILE_READABLE = 1,
    FILE_WRITABLE = 2,
    FILE_INACCESSIBLE = 3
};

std::tuple<std::string, std::string> split_filename(std::string name, const char delimiter = '.');
bool valid_wav_extension(std::string extension);
std::tuple<std::string, std::string> get_path_and_extension(std::string name);

class EspSDWavFile : public WavHeader
{
protected:
    bool _sdInitialized = false, _fileIsOpen = false, _filenameSet = false;
    uint32_t _dataIndex;
    int _fileMode = FILE_INACCESSIBLE;
    bool _initialize_sd();
    std::string _directory, _filename, _fileExtension;
    void _check_sd();
    void _check_file_open();
    void _check_readable();
    void _check_writable();
    void _check_filename();
    bool _open_read();
    bool _open_write();

    void _parse_filename(std::string name);

public:
    File file;
    std::string filename;
    EspSDWavFile(WavParameters parameters);
    EspSDWavFile();
    ~EspSDWavFile();
    virtual void set_directory(std::string directory);
    virtual void set_extension(std::string extension);
    virtual void set_filename(std::string str);
    virtual void append_filename(std::string str);
    bool open_read();
    bool open_write();
    bool open_read(std::string fileName);
    bool open_write(std::string fileName);
    bool is_open();
    size_t _available(); // Index to data size comparison
    size_t samples_available();
    int available(); // File::available()
    void seek(uint32_t position);
    void close();
    void write_header();
    void read_header();
    template <typename T>
    std::vector<T> read(size_t& numSamples);
    void read(uint8_t* buff, size_t numSamples);
    void reinitialize();
    void write(uint8_t* data, uint32_t size);
    template <typename T>
    void write(std::vector<T>& data);
    template <typename T>
    void write(std::vector<T>* data);
};

#endif
