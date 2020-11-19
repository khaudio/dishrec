#ifndef ESPSD_H
#define ESPSD_H

/* SdFat datatypes */

#include <vector>
#include <iostream>

#ifdef ESP32
#include <SdFat.h>
typedef SdFs sd_t; // Supports FAT16, FAT32, and exFAT
typedef FsFile file_t;
#else
#include <fstream>
typedef int sd_t; // Dummy value if not using actual SD card
typedef std::fstream file_t;
#endif

/* GPIO pinout for SD card

    SPI:

        CS                          Any
        MOSI                        23
        MISO                        19
        CLK                         18

    SDMMC:

        SDMMC_HOST_SLOT_0           8-bit

            Unusable on WROOM and WROVER modules,
            as it shares pins with SPI flash memory.

        SDMMC_HOST_SLOT_1           4-bit


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

namespace EspSD
{

enum file_err
{
    SD_NOT_INITIALIZED = 170,
    FILE_NOT_OPEN = 171
};

#define PIN_NUM_CS                  (GPIO_NUM_5)

class SDCard
{
protected:
    sd_t sd;
    bool _sdInitialized;

public:
    SDCard();
    ~SDCard();

    bool mount();
    void unmount();
    
    bool is_mounted() const;

    operator bool() const;
    bool operator!() const;
};

/* Simple wrapper around file read/write access

For more advanced features,
use an SdFs or std::fstream object instead. */

class FileObj
{
protected:
    file_t _file;

public:
    FileObj();
    ~FileObj();

    /* Opens file for read/write,
    erasing existing with same name */
    virtual bool open(const char* fname);

    /* Opens file read-only */
    virtual bool open_read(const char* fname);
    
    /* Opens file read/write,
    starting at end of file but allowing
    seeking to any position in file */
    virtual bool open_append(const char* fname);

    virtual void close();

    virtual void write(const char* data, size_t numBytes);
    virtual void write(uint8_t* data, size_t numBytes);

    /* Write vector data and return number of bytes written.
    Returns numBytes to avoid re-calculating size
    in calling or inherited function. */
    template <typename T>
    size_t write(std::vector<T>* data);

    virtual void seek(size_t index);
    virtual size_t get_position();

    virtual bool is_open();
    operator bool();

    template <typename T>
    void operator<<(std::vector<T> data);
};

};

#endif
