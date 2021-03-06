#include "WavWriter.h"

using namespace WavFile;

FileMeta::FileMeta() :
_directory(""),
extension(".wav")
{
}

FileMeta::~FileMeta()
{
}

WavWriter::WavWriter() :
FileMeta(),
BWFHeader::BroadcastWav()
{
}

WavWriter::
~WavWriter()
{
    close();
}

inline void FileMeta::_set_filename()
{
    bool omitSeperator = (
            this->_directory.empty()
            || (this->_directory.c_str()[-2] == WavFile::seperator[0])
        );
    this->_filename = std::string(
            this->_directory
            + (omitSeperator ? "" : WavFile::seperator)
            + this->_basename
            + this->extension
        );
}

inline void FileMeta::set_directory(const char* directoryName)
{
    this->_directory = std::string(directoryName);
    _set_filename();
}

inline void FileMeta::set_directory(std::string directoryName)
{
    this->_directory = directoryName;
    _set_filename();
}

inline void FileMeta::set_filename(const char* fname)
{
    this->_basename = std::string(fname);
    _set_filename();
}

inline void FileMeta::set_filename(std::string fname)
{
    this->_basename = fname;
    _set_filename();
}

const char* FileMeta::get_filename() const
{
    return this->_filename.c_str();
}

bool WavWriter::_new_file()
{
    const char* name = FileMeta::get_filename();
    bool opened = EspSD::FileObj::open(name);
    for (size_t i(0); i < this->_maxHeaderSize; ++i)
    {
        // Pad header allocation with NULL
        EspSD::FileObj::write("\0", 1);
    }
    return opened;
}

size_t WavWriter::size()
{
    return this->_maxHeaderSize + get_data_size();
}

void WavWriter::_write_header()
{
    if (!is_open()) return;
    
    /* Get current position */
    size_t position(get_position());

    /* Get WAV header */
    uint8_t* header = new uint8_t[this->_maxHeaderSize];
    std::memset(header, '\0', this->_maxHeaderSize);
    BWFHeader::BroadcastWav::get(header);
    
    /* Return to file start */
    seek(0);

    /* Write header in pre-allocated space */
    EspSD::FileObj::write(header, this->_maxHeaderSize);
    delete header;

    /* Return to previous position.
    If more data is written, header will
    need to be updated with new data size. */
    seek(position);
}

void WavWriter::set_filename(const char* fname)
{
    FileMeta::set_filename(fname);
    iXML::IXML::set_filename(fname);
}

void WavWriter::set_filename(std::string fname)
{
    FileMeta::set_filename(fname);
    iXML::IXML::set_filename(fname.c_str());
}

const char* WavWriter::get_filename() const
{
    return FileMeta::get_filename();
}

bool WavWriter::open()
{
    return _new_file();
}

bool WavWriter::open(const char* fname)
{
    FileMeta::set_filename(fname);
    return _new_file();
}

bool WavWriter::open_read()
{
    return EspSD::FileObj::open_read(FileMeta::get_filename());
}

bool WavWriter::open_read(const char* fname)
{
    FileMeta::set_filename(fname);
    return EspSD::FileObj::open_read(FileMeta::get_filename());
}

bool WavWriter::open_append()
{
    return EspSD::FileObj::open_append(FileMeta::get_filename());
}

bool WavWriter::open_append(const char* fname)
{
    FileMeta::set_filename(fname);
    return EspSD::FileObj::open_append(FileMeta::get_filename());
}

void WavWriter::write(const char* data, size_t numBytes)
{
    EspSD::FileObj::write(data, numBytes);
    set_data_size(get_data_size() + numBytes);
}

void WavWriter::write(uint8_t* data, size_t numBytes)
{
    EspSD::FileObj::write(data, numBytes);
    set_data_size(get_data_size() + numBytes);
}

template <typename T>
size_t WavWriter::write(std::vector<T>* data)
{
    size_t numBytes(EspSD::FileObj::write(data));
    set_data_size(get_data_size() + numBytes);
    return numBytes;
}

void WavWriter::close()
{
    _write_header();
    EspSD::FileObj::close();
}

template size_t WavWriter::write(std::vector<int8_t>*);
template size_t WavWriter::write(std::vector<uint8_t>*);
template size_t WavWriter::write(std::vector<int16_t>*);
template size_t WavWriter::write(std::vector<uint16_t>*);
template size_t WavWriter::write(std::vector<int32_t>*);
template size_t WavWriter::write(std::vector<uint32_t>*);
template size_t WavWriter::write(std::vector<int64_t>*);
template size_t WavWriter::write(std::vector<uint64_t>*);
template size_t WavWriter::write(std::vector<float>*);
template size_t WavWriter::write(std::vector<double>*);
template size_t WavWriter::write(std::vector<long double>*);
