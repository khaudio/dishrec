#ifndef PCM422XCONFIG_H
#define PCM422XCONFIG_H

#include <cstdint>
#include "PCM422xPins.h"
#include "ErrorEnums.h"

/* Classes to interface ESP32 with PCM4220 and PCM4222 */

enum pcm422x_channel
{
    CHANNEL_LEFT = 0,
    CHANNEL_RIGHT = 1
};

enum pcm4222_dsdmode
{
    DSD_OVERSAMPLED_64X = 0,
    DSD_OVERSAMPLED_128X = 1,
};

class PCM4220
{
protected:
    virtual gpio_config_t _gpio_output_conf();
public:
    PCM4220();
    ~PCM4220();
    virtual void set_bit_depth(uint16_t bitsPerSample, bool isFloat);
    virtual void set_sample_rate(uint32_t samplerate);
    virtual void enable_pcm();
    virtual void enable_i2s();
    void set_hpf(bool isEnabled);
    void set_adc_slave(bool adcIsSlave);
    bool get_overflow(pcm422x_channel channel);
};

class PCM4222 : public PCM4220
{
protected:
    gpio_config_t _gpio_output_conf() override;
public:
    PCM4222();
    ~PCM4222();
    void enable_pcm() override;
};

#endif
