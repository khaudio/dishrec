#include "EspSDWavFile.h"

EspSDWavFile::EspSDWavFile(WavParameters parameters) :
WavHeader(parameters),
_directory("/"),
_fileExtension(".wav")
{
}

EspSDWavFile::EspSDWavFile() :
WavHeader(),
_directory("/"),
_fileExtension(".wav")
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

void EspSDWavFile::set_directory(std::string directory)
{
    if (directory.substr(0, 1) != "/")
    {
        throw INAVLID_DIRECTORY;
    }
    this->_directory = directory;
}

void EspSDWavFile::set_filename(std::string str)
{
    this->_filename = str;
    this->filename = this->_directory + this->_filename + this->_fileExtension;
    this->_filenameSet = true;
    #ifdef _DEBUG
    std::cout << "Filename set to " << this->filename << std::endl;
    #endif
}

void EspSDWavFile::append_filename(std::string str)
{
    this->_filename += str;
    set_filename(this->_filename);
}

void EspSDWavFile::_check_sd()
{
    if (!this->_sdInitialized)
    {
        if (!_initialize_sd())
        {
            #ifdef _DEBUG
            std::cerr << "Aborting";
            #endif
        }
    }
}

void EspSDWavFile::_check_file_open()
{
    if (!this->file)
    {
        #ifdef _DEBUG
        std::cerr << "File failed to open" << std::endl;
        #endif
        throw FILE_NOT_OPEN;
    }
}

void EspSDWavFile::_check_readable()
{
    _check_file_open();
    if (this->_fileMode != FILE_READABLE)
    {
        #ifdef _DEBUG
        std::cerr << "File mode incorrect" << std::endl;
        #endif
        throw FILE_MODE_INCORRECT;
    }
}

void EspSDWavFile::_check_writable()
{
    _check_file_open();
    if (this->_fileMode != FILE_WRITABLE)
    {
        #ifdef _DEBUG
        std::cerr << "File mode incorrect" << std::endl;
        #endif
        throw FILE_MODE_INCORRECT;
    }
}

void EspSDWavFile::_check_filename()
{
    if (!this->_filenameSet)
    {
        #ifdef _DEBUG
        std::cerr << "Filename not set" << std::endl;
        #endif
        throw FILENAME_NOT_SET;
    }
}

bool EspSDWavFile::_open_read()
{
    this->file = SD.open(this->filename.c_str(), FILE_READ);
    _check_file_open();
    this->_fileIsOpen = true;
    this->_fileMode = FILE_READABLE;
    return true;
}

bool EspSDWavFile::_open_write()
{
    this->file = SD.open(this->filename.c_str(), FILE_WRITE);
    _check_file_open();
    this->_fileIsOpen = true;
    seek(0);
    for (int i(0); i < this->headerSize; ++i)
    {
        file.write(' ');
    }
    seek(this->headerSize);
    this->_fileMode = FILE_WRITABLE;
    return true;
}

bool EspSDWavFile::open_read()
{
    _check_sd();
    return _open_read();
}

bool EspSDWavFile::open_write()
{
    _check_sd();
    return _open_write();
}

bool EspSDWavFile::open_read(std::string fileName)
{
    set_filename(fileName);
    return open_read();
}

bool EspSDWavFile::open_write(std::string fileName)
{
    set_filename(fileName);
    return open_write();
}

bool EspSDWavFile::is_open()
{
    return this->_fileIsOpen;
}

void EspSDWavFile::seek(uint32_t position)
{
    if (!this->file.seek(position))
    {
        #ifdef _DEBUG
        std::cerr << "Seek error" << std::endl;
        #endif
        throw SEEK_ERROR;
    }
    this->_dataIndex = (
            (position >= this->headerSize)
            ? (position - this->headerSize) : 0
        );
}

void EspSDWavFile::close()
{
    if (!is_open())
    {
        return;
    }
    write_header();
    this->file.close();
    this->_fileIsOpen = false;
    this->_fileMode = FILE_INACCESSIBLE;
}

void EspSDWavFile::write_header()
{
    const uint8_t* header = get_header();
    seek(0);
    this->file.write(header, this->headerSize);
}

void EspSDWavFile::read_header()
{
    uint8_t headerFromFile[this->headerSize];
    this->file.read(headerFromFile, this->headerSize);
    import_header(headerFromFile);
    seek(0);
}

template <typename T>
std::vector<T> EspSDWavFile::read(size_t numSamples)
{
    _check_readable();
    std::vector<T> data;
    data.reserve(numSamples);
    for (uint32_t i(0); i < numSamples; ++i)
    {
        T currentByte;
        this->file.read(&currentByte, this->sampleWidth);
        data.emplace_back(currentByte);
    }
    this->_dataIndex += this->sampleWidth * numSamples;
    return data;
}

void EspSDWavFile::read(uint8_t* buff, size_t numSamples)
{
    _check_readable();
    size_t length = this->sampleWidth * numSamples;
    this->file.read(buff, length);
    this->_dataIndex += length;
}

void EspSDWavFile::reinitialize()
{
    if (is_open())
    {
        this->file.close();
        this->_fileIsOpen = false;
        this->_fileMode = FILE_INACCESSIBLE;
        this->_dataIndex = 0;
    }
    set_size(0);
}

void EspSDWavFile::write(uint8_t* data, uint32_t length)
{
    set_size(this->dataSize + length);
    seek(this->headerSize + this->_dataIndex);
    this->file.write(data, length);
    this->_dataIndex += length;
}

template <typename T>
void EspSDWavFile::write(std::vector<T>& data)
{
    uint32_t length = data.size() * sizeof(T);
    uint8_t* recast = reinterpret_cast<uint8_t*>(&(data[0]));
    write(recast, length);
}

template <typename T>
void EspSDWavFile::write(std::vector<T>* data)
{
    uint32_t length = data->size() * sizeof(T);
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
