#include "EspSDWavFile.h"

EspSDWavFile::EspSDWavFile(WavParameters parameters) :
WavHeader(parameters)
{
}

EspSDWavFile::EspSDWavFile()
{
}

EspSDWavFile::~EspSDWavFile()
{
    close();
}

bool EspSDWavFile::_initialize_sd()
{
    if (!SD.begin(SD_CS_PIN))
    {
        std::cerr << "SD initialization failed" << std::endl;
        this->_sdInitialized = false;
    }
    this->_sdInitialized = true;
    return this->_sdInitialized;
}

void EspSDWavFile::set_filename()
{
    this->filename = filename;
}

bool EspSDWavFile::open()
{
    if (!this->_sdInitialized)
    {
        if (!_initialize_sd())
        {
            std::cerr << "Aborting";
            return false;
        }
    }
    this->file = SD.open(this->filename.c_str(), FILE_WRITE);
    if (!file)
    {
        std::cerr << "File failed to open" << std::endl;
        return false;
    }
    this->_fileIsOpen = true;
    seek(0);
    for (int i(0); i < this->headerSize; ++i)
    {
        file.write(' ');
    }
    seek(this->headerSize);
    return true;
}

bool EspSDWavFile::open(std::string filename)
{
    set_filename(filename);
    return open();
}

bool EspSDWavFile::is_open()
{
    return this->_fileIsOpen;
}

void EspSDWavFile::seek(uint32_t position)
{
    if (!this->file.seek(position))
    {
        std::cerr << "Seek error" << std::endl;
    }
    this->_dataIndex = (
            (position >= this->headerSize)
            ? (position - this->headerSize) : 0
        );
}

void EspSDWavFile::close()
{
    if (!this->_fileIsOpen)
    {
        return;
    }
    write_header();
    this->file.close();
    this->_fileIsOpen = false;
}

void EspSDWavFile::write_header()
{
    const uint8_t* header = get_header();
    seek(0);
    for (uint32_t i(0); i < this->headerSize; ++i)
    {
        this->file.write(header[i]);
    }
}

void EspSDWavFile::reinitialize()
{
    if (this->_fileIsOpen)
    {
        this->file.close();
        this->_fileIsOpen = false;
    }
    set_size(0);
    seek(0);
}

void EspSDWavFile::write(uint8_t* data, uint32_t length)
{
    set_size(this->dataSize + length);
    seek(this->headerSize + this->_dataIndex);
    for (uint32_t i(0); i < length; ++i)
    {
        this->file.write(data[i]);
    }
    this->_dataIndex += length;
}

template <typename T>
void EspSDWavFile::write(std::vector<T>& data)
{
    uint32_t length(data.size() * sizeof(T));
    uint8_t* recast = reinterpret_cast<uint8_t*>(&(data[0]));
    write(recast, length);
}

template <typename T>
void EspSDWavFile::write(std::vector<T>* data)
{
    uint32_t length(data->size() * sizeof(T));
    uint8_t* recast = reinterpret_cast<uint8_t*>(data[0]);
    write(recast, length);
}

template void EspSDWavFile::write<float>(std::vector<float>&);
template void EspSDWavFile::write<double>(std::vector<double>&);
template void EspSDWavFile::write<long double>(std::vector<long double>&);
template void EspSDWavFile::write<int8_t>(std::vector<int8_t>&);
template void EspSDWavFile::write<uint8_t>(std::vector<uint8_t>&);
template void EspSDWavFile::write<int16_t>(std::vector<int16_t>&);
template void EspSDWavFile::write<uint16_t>(std::vector<uint16_t>&);
template void EspSDWavFile::write<int32_t>(std::vector<int32_t>&);
template void EspSDWavFile::write<uint32_t>(std::vector<uint32_t>&);
template void EspSDWavFile::write<int64_t>(std::vector<int64_t>&);
template void EspSDWavFile::write<uint64_t>(std::vector<uint64_t>&);
