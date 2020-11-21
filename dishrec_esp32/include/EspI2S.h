#ifndef ESPI2S_H
#define ESPI2S_H

#include <FreeRTOS.h>
#include <driver/i2s.h>

#include <vector>
#include "WavHeader.h"
#include "int_audio.h"

namespace I2S
{

class Bus : virtual public WavMeta::WavFormat
{
protected:
    bool _running;

    // I2S port number (0 or 1; Internal DAC only supported on I2S_NUM_0)
    i2s_port_t _i2sNum;
    
    // I2S pin configuration
    i2s_pin_config_t _pinConfig;

    // I2S configuration
    i2s_config_t _i2sConfig;

    // I2S mode (Master, slave, transmit, receive)
    int _mode;
    
    int _shutdownPin;
    bool _pinsSet, _shutdownPinSet, _shutdownState;
    size_t _bufferLength;

    virtual i2s_bits_per_sample_t _get_i2s_bit_depth() const;
    virtual i2s_channel_fmt_t _get_i2s_channel_format() const;
    virtual void _check_format();
    virtual void _stop();

public:
    Bus();
    Bus(int i2sNum, bool isMaster, bool receives, bool transmits);
    ~Bus();

    virtual void set_i2s_num(int i2sNum);

    virtual void reset_mode();
    virtual void set_master();
    virtual void set_slave();
    virtual void set_transmit();
    virtual void set_receive();

    // DMA buffer length
    virtual void set_buffer_size(size_t bufferSizeInSamples);

    // Physical pins
    virtual void set_pins(int wsPin, int bckPin, int dataInPin, int dataOutPin);
    virtual void set_shutdown_pin(int sdPin = (GPIO_NUM_21), bool activeHigh = true);

    virtual void config();

    virtual void start();
    virtual void stop();
    virtual void reload();
    virtual bool is_running() const;

    virtual bool read(uint8_t* buff, size_t numBytes);

    template <typename T>
    bool read(std::vector<T>* buff);
    
    virtual bool write(const uint8_t* data, size_t numBytes);

    template <typename T>
    bool write(std::vector<T>* data);

    template <typename T>
    void operator<<(std::vector<T>* data);

    template <typename T>
    void operator>>(std::vector<T>* buff);
};

};

#endif
