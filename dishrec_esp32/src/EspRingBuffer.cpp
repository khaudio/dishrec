#include "EspRingBuffer.h"

void write_to_file()
{
    // Write from buffer to file
    static uint8_t* currentBufferPtr;
    currentBufferPtr = buff.get_read_ptr();
    f.write(currentBufferPtr, buff.bytesPerBuffer);
}

bool write_if_buffered()
{
    // Write data to file as soon as it is buffered
    bool written = false;
    while (buff.buffered())
    {
        write_to_file();
        buff.rotateReadBuffer();
        written = true;
    }
    return written;
}

void write_if_buffered_button(void* pvParameter)
{
    bool* recording = reinterpret_cast<bool*>(pvParameter);
    while (true)
    {
        if (*recording && f.is_open())
        {
            if (buff.buffered())
            {
                write_to_file();
                buff.rotateReadBuffer();
            }
        }
        else if (f.is_open())
        {
            std::cout << "Closing file" << std::endl;
            f.close();
        }
    }
}
