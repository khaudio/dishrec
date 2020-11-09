#include "WavWriter.h"

using namespace WavFile;

WavWriter::WavWriter() :
BWFHeader::BroadcastWav()
{
}

WavWriter::
~WavWriter()
{
}

void WavWriter::_set_filename(const char* fname)
{
    size_t length = std::strlen(fname);
    if (length > filenameCharBufferLength)
    {
        throw INVALID_FILEPATH_LENGTH;
    }
    std::strncpy(this->_filename, fname, length);
    std::strncpy(this->_filename + length, ".wav", 5);
}

void WavWriter::_new_file()
{
    this->_filestream = std::fstream();
    this->_filestream.open(this->_filename, (
            std::fstream::out | std::fstream::binary
        ));
    for (size_t i(0); i < numBytesReservedForWavHeader; ++i)
    {
        this->_filestream << " ";
    }
}

void WavWriter::_close_file()
{
    this->_filestream.close();
}

void WavWriter::_append_to_file(const char* fname)
{
    this->_filestream = std::fstream();
    this->_filestream.open(fname, (
            std::fstream::out | std::fstream::binary | std::fstream::ate
        ));
}

void WavWriter::_write_to_file(uint8_t* data, size_t numBytes)
{
    #if _DEBUG
    assert(this->_filestream.is_open());
    #endif
    this->_filestream.write(reinterpret_cast<const char*>(data), numBytes);
}

void WavWriter::_write_header(uint8_t* data, size_t numBytes)
{
    this->_padBytes = numBytesReservedForWavHeader - numBytes;
    std::cout << "PAD BYTES: " << this->_padBytes;
    this->_filestream.seekp(0);
    _write_to_file(data, numBytes);
}
