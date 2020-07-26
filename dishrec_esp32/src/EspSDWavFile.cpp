#include "EspSDWavFile.h"

std::tuple<std::string, std::string> split_filename(std::string name, const char delimiter)
{
    std::string basename, extension;
    for (int i(name.size()); i >= 0; --i)
    {
        if (name[i] == delimiter)
        {
            basename = name.substr(0, i);
            extension = name.substr(i + 1, name.size() - i);
        }
    }
    return std::make_tuple(basename, extension);
}

bool valid_wav_extension(std::string name)
{
    std::tuple<std::string, std::string> split = split_filename(name);
    std::string extension = std::get<1>(split);
    return ((extension == "wav") || (extension == "bwf"));
}

std::tuple<std::string, std::string> get_path_and_extension(std::string name)
{
    // Return tuple of path and basename
    std::tuple<std::string, std::string> split = split_filename(name, '/');
    std::tuple<std::string, std::string> extensionSplit = split_filename(std::get<1>(split));
    return std::make_tuple(std::get<0>(split), std::get<0>(extensionSplit));
}

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

void EspSDWavFile::set_extension(std::string extension)
{
    this->_fileExtension = extension;
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

void EspSDWavFile::_parse_filename(std::string name)
{
    ;
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
    #ifdef _DEBUG
    std::cout << "Opened file " << this->filename << std::endl;
    #endif
    return true;
}

bool EspSDWavFile::_open_write()
{
    this->file = SD.open(this->filename.c_str(), FILE_WRITE);
    _check_file_open();
    this->_fileIsOpen = true;
    seek(0);
    for (int i(0); i < this->headerSize; ++i) file.write(' ');
    seek(this->headerSize);
    this->_fileMode = FILE_WRITABLE;
    #ifdef _DEBUG
    std::cout << "Opened file " << this->filename << std::endl;
    #endif
    return true;
}

bool EspSDWavFile::open_read()
{
    // Recursively search for wav files on SD card if filename is not set
    _check_sd();
    if (!_filenameSet || !valid_wav_extension(this->filename))
    {
        #ifdef _DEBUG
        std::cout << "No filename specified; searching SD card..." << std::endl;
        #endif
        this->file = SD.open(this->_directory.c_str());
        do
        {
            this->file = this->file.openNextFile();
            if (this->file.isDirectory())
            {
                #ifdef _DEBUG
                std::cout << "Searching directory " << this->file.name() << "..." << std::endl;
                #endif
                set_directory(this->file.name());
                this->file = SD.open(this->filename.c_str());
                continue;
            }
            std::tuple<std::string, std::string> split = split_filename(this->file.name());
            this->_filename = std::get<0>(split);
            this->_fileExtension = std::get<1>(split);
        } while ((this->_fileExtension != "wav") && (this->_fileExtension != "bwf"));
        #ifdef _DEBUG
        std::cout << "Advancing to next file " << this->file.name() << std::endl;
        #endif
        set_filename(this->_filename);
    }
    return _open_read();
}

bool EspSDWavFile::open_write()
{
    _check_sd();
    return _open_write();
}

bool EspSDWavFile::open_read(std::string fileName)
{
    auto split = get_path_and_extension(fileName);
    if (std::get<0>(split).size()) // If path is included
    {
        set_directory(std::get<0>(split));
        set_filename(std::get<1>(split));
    }
    else
    {
        set_filename(fileName);
    }
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

size_t EspSDWavFile::_available()
{
    // Return number of bytes remaining to be read
    _check_readable();
    return this->dataSize - this->_dataIndex;
}

size_t EspSDWavFile::samples_available()
{
    // Return number of samples remaining to be read
    _check_readable();
    return (this->dataSize - this->_dataIndex) / this->sampleWidth;
}

int EspSDWavFile::available()
{
    // Return number of bytes remaining to be read using File::available
    _check_readable();
    int avail = this->file.available();
    return avail;
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
    if (!is_open()) return;
    if (this->_fileMode == FILE_WRITABLE) write_header();
    this->file.close();
    this->_fileIsOpen = false;
    this->_fileMode = FILE_INACCESSIBLE;
    this->_dataIndex = 0;
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
std::vector<T> EspSDWavFile::read(size_t& numSamples)
{
    _check_readable();
    std::vector<T> data;
    data.reserve(numSamples);
    size_t avail = samples_available();
    size_t length = (avail < numSamples) ? avail : numSamples;
    for (uint32_t i(0); i < length; ++i)
    {
        T currentByte;
        this->file.read(reinterpret_cast<uint8_t*>(&currentByte), this->sampleWidth);
        data.emplace_back(currentByte);
    }
    this->_dataIndex += this->sampleWidth * length;
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
    close();
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
    uint8_t* recast = reinterpret_cast<uint8_t*>(&(data->at(0)));
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

template void EspSDWavFile::write<float>(std::vector<float>*);
template void EspSDWavFile::write<double>(std::vector<double>*);
template void EspSDWavFile::write<long double>(std::vector<long double>*);
template void EspSDWavFile::write<int8_t>(std::vector<int8_t>*);
template void EspSDWavFile::write<uint8_t>(std::vector<uint8_t>*);
template void EspSDWavFile::write<int16_t>(std::vector<int16_t>*);
template void EspSDWavFile::write<uint16_t>(std::vector<uint16_t>*);
template void EspSDWavFile::write<int32_t>(std::vector<int32_t>*);
template void EspSDWavFile::write<uint32_t>(std::vector<uint32_t>*);
template void EspSDWavFile::write<int64_t>(std::vector<int64_t>*);
template void EspSDWavFile::write<uint64_t>(std::vector<uint64_t>*);

template std::vector<float> EspSDWavFile::read<float>(size_t&);
template std::vector<double> EspSDWavFile::read<double>(size_t&);
template std::vector<long double> EspSDWavFile::read<long double>(size_t&);
template std::vector<int8_t> EspSDWavFile::read<int8_t>(size_t&);
template std::vector<uint8_t> EspSDWavFile::read<uint8_t>(size_t&);
template std::vector<int16_t> EspSDWavFile::read<int16_t>(size_t&);
template std::vector<uint16_t> EspSDWavFile::read<uint16_t>(size_t&);
template std::vector<int32_t> EspSDWavFile::read<int32_t>(size_t&);
template std::vector<uint32_t> EspSDWavFile::read<uint32_t>(size_t&);
template std::vector<int64_t> EspSDWavFile::read<int64_t>(size_t&);
template std::vector<uint64_t> EspSDWavFile::read<uint64_t>(size_t&);
