#ifndef WAVWRITER_H
#define WAVWRITER_H

#include <iostream>
#include "BWFHeader.h"
#include "EspSD.h"

enum wavfile_err
{
    INVALID_FILEPATH_LENGTH = 160
};

namespace WavFile
{

/* Filepath separator */
constexpr const char* seperator = "/";

class FileMeta
{
protected:
    std::string _directory, _basename, _filename;

    void _set_filename();

public:
    std::string extension;
    
    FileMeta();
    ~FileMeta();

    virtual void set_directory(const char* directoryName = "/");
    virtual void set_directory(std::string directoryName = "/");

    virtual void set_filename(const char* fname);
    virtual void set_filename(std::string fname);

    virtual const char* get_filename() const;
};

class WavWriter :
public FileMeta,
public EspSD::FileObj,
public BWFHeader::BroadcastWav
{
protected:
    bool _new_file();
    void _write_header();

public:
    WavWriter();
    ~WavWriter();

    void set_filename(const char* filename) override;
    void set_filename(std::string filename) override;

    const char* get_filename() const override;

    /* Total file size (header + data) */
    size_t size() override;

    /* Open new file */
    bool open();
    bool open(const char* fname) override;

    /* Open file as read-only */
    bool open_read();
    bool open_read(const char* fname) override;

    /* Open and append to file */
    bool open_append();
    bool open_append(const char* fname) override;

    void write(const char* data, size_t numBytes) override;
    void write(uint8_t* data, size_t numBytes) override;

    /* Write vector data and return number of bytes written */
    template <typename T>
    size_t write(std::vector<T>* data);

    /* Write header and close file */
    void close() override;
};

};

#endif
