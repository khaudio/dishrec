#include "EspSD.h"

using namespace EspSD;

SDCard::SDCard() :
_sdInitialized(false)
{
    mount();
}

SDCard::~SDCard()
{
    unmount();
}

bool SDCard::mount()
{
    #ifdef ESP32
    this->_sdInitialized = sd.begin(PIN_NUM_CS, SD_SCK_MHZ(20));
    #else
    this->_sdInitialized = true;
    #endif
    return this->_sdInitialized;
}

void SDCard::unmount()
{
    #ifdef ESP32
    this->sd.end();
    #endif
    this->_sdInitialized = false;
}

bool SDCard::is_mounted() const
{
    return this->_sdInitialized;
}

SDCard::operator bool() const
{
    return this->_sdInitialized;
}

bool SDCard::operator!() const
{
    return !this->_sdInitialized;
}

FileObj::FileObj()
{
}

FileObj::~FileObj()
{
    close();
}

bool FileObj::open(const char* fname)
{
    this->_file = file_t();
    #ifdef ESP32
    this->_file.open(fname, O_RDWR | O_CREAT | O_TRUNC);
    #else
    std::cout << "Opening " << fname << std::endl;
    this->_file.open(fname, (
        std::fstream::out | std::fstream::binary
    ));
    #endif
    return is_open();
}


bool FileObj::open_read(const char* fname)
{
    this->_file = file_t();
    #ifdef ESP32
    this->_file.open(fname, O_RDONLY);
    #else
    this->_file.open(fname, (
        std::fstream::in | std::fstream::binary
    ));
    #endif
    return is_open();
}

bool FileObj::open_append(const char* fname)
{
    this->_file = file_t();
    
    #ifdef ESP32
    this->_file.open(fname, O_RDWR | O_AT_END);
    #else
    this->_file.open(fname, (
        std::fstream::out | std::fstream::binary | std::fstream::ate
    ));
    #endif
    return is_open();
}

void FileObj::close()
{
    this->_file.close();
}

inline void FileObj::write(const char* data, size_t numBytes)
{
    this->_file.write(data, numBytes);
}

inline void FileObj::write(uint8_t* data, size_t numBytes)
{
    this->_file.write(reinterpret_cast<const char*>(data), numBytes);
}

template <typename T>
inline size_t FileObj::write(std::vector<T>* data)
{
    constexpr size_t datatypeSize(sizeof(T));
    size_t numBytes = datatypeSize * data->size();
    this->_file.write(
            reinterpret_cast<const char*>(&data->at(0)),
            numBytes
        );
    return numBytes;
}

inline void FileObj::seek(size_t index)
{
    #ifdef ESP32
    this->_file.seek(index);
    #else
    this->_file.seekp(index);
    #endif
}

inline size_t FileObj::get_position()
{
    #ifdef ESP32
    return this->_file.curPosition();
    #else
    return this->_file.tellp();
    #endif
}

inline bool FileObj::is_open()
{
    #ifdef ESP32
    return this->_file.isOpen();
    #else
    return this->_file.is_open();
    #endif
}

FileObj::operator bool()
{
    return is_open();
}

template <typename T>
void FileObj::operator<<(std::vector<T> data)
{
    write(&data);
}

template size_t FileObj::write(std::vector<int8_t>*);
template size_t FileObj::write(std::vector<uint8_t>*);
template size_t FileObj::write(std::vector<int16_t>*);
template size_t FileObj::write(std::vector<uint16_t>*);
template size_t FileObj::write(std::vector<int32_t>*);
template size_t FileObj::write(std::vector<uint32_t>*);
template size_t FileObj::write(std::vector<int64_t>*);
template size_t FileObj::write(std::vector<uint64_t>*);
template size_t FileObj::write(std::vector<float>*);
template size_t FileObj::write(std::vector<double>*);
template size_t FileObj::write(std::vector<long double>*);

template void FileObj::operator<<(std::vector<int8_t>);
template void FileObj::operator<<(std::vector<uint8_t>);
template void FileObj::operator<<(std::vector<int16_t>);
template void FileObj::operator<<(std::vector<uint16_t>);
template void FileObj::operator<<(std::vector<int32_t>);
template void FileObj::operator<<(std::vector<uint32_t>);
template void FileObj::operator<<(std::vector<int64_t>);
template void FileObj::operator<<(std::vector<uint64_t>);
template void FileObj::operator<<(std::vector<float>);
template void FileObj::operator<<(std::vector<double>);
template void FileObj::operator<<(std::vector<long double>);
