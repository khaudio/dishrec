#include "PCM422xConfig.h"

PCM4220::PCM4220()
{
    /* Defaults to PCM over I2S with ADC in slave mode */
    gpio_config_t gpioOutputConf = _gpio_output_conf();
    gpio_config_t gpioInputConf = {
            pin_bit_mask : PCM422X_GPIO_INPUT_PINS,
            mode : GPIO_MODE_INPUT,
            pull_up_en : GPIO_PULLUP_DISABLE,
            pull_down_en : GPIO_PULLDOWN_ENABLE,
            intr_type : static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE)
        };
    gpio_config(&gpioOutputConf);
    gpio_config(&gpioInputConf);
    set_adc_slave(true);
    enable_pcm();
    enable_i2s();
    set_hpf(true);
    set_bit_depth(24, false);
    set_sample_rate(48000);
}

PCM4220::~PCM4220()
{
}

gpio_config_t PCM4220::_gpio_output_conf()
{
    gpio_config_t gpioOutputConf = {
            pin_bit_mask : PCM4220_GPIO_OUTPUT_PINS,
            mode : GPIO_MODE_OUTPUT,
            pull_up_en : GPIO_PULLUP_DISABLE,
            pull_down_en : GPIO_PULLDOWN_DISABLE,
            intr_type : static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE)
        };
    return gpioOutputConf;
}

void PCM4220::set_bit_depth(uint16_t bitsPerSample, bool isFloat)
{
    switch (bitsPerSample)
    {
        case (24):
            gpio_set_level(PCM422X_OWL0, 0);
            gpio_set_level(PCM422X_OWL1, 0);
            break;
        case (20):
            gpio_set_level(PCM422X_OWL0, 0);
            gpio_set_level(PCM422X_OWL1, 1);
            break;
        case (18):
            gpio_set_level(PCM422X_OWL0, 1);
            gpio_set_level(PCM422X_OWL1, 0);
            break;
        case (16):
            gpio_set_level(PCM422X_OWL0, 1);
            gpio_set_level(PCM422X_OWL1, 1);
            break;
        default:
            throw INVALID_BIT_DEPTH;
    }
}

void PCM4220::set_sample_rate(uint32_t samplerate)
{
    if ((samplerate >= 8000) && (samplerate <= 54000))
    {
        gpio_set_level(PCM422X_FS0, 0);
        gpio_set_level(PCM422X_FS1, 0);
    }
    else if ((samplerate > 54000) && (samplerate <= 108000))
    {
        gpio_set_level(PCM422X_FS0, 1);
        gpio_set_level(PCM422X_FS1, 0);
    }
    else if ((samplerate > 108000) && (samplerate <= 216000))
    {
        gpio_set_level(PCM422X_FS0, 0);
        gpio_set_level(PCM422X_FS1, 1);
    }
    else
    {
        throw INVALID_SAMPLERATE;
    }
}

void PCM4220::enable_pcm()
{
    // Enable PCM output
    gpio_set_level(PCM422X_PCMEN, 1);
}

void PCM4220::enable_i2s()
{
    // Set output to I2S
    gpio_set_level(PCM422X_FMT0, 1);
    gpio_set_level(PCM422X_FMT1, 0);

    // Set TDM-related pins low
    gpio_set_level(PCM422X_SUB0, 0);
    gpio_set_level(PCM422X_SUB1, 0);
}

void PCM4220::set_hpf(bool isEnabled)
{
    // Filter out DC in both channels
    gpio_set_level(PCM422X_HPFDR, isEnabled);
    gpio_set_level(PCM422X_HPFDL, isEnabled);
}

void PCM4220::set_adc_slave(bool adcIsSlave)
{
    // Set ADC as slave device
    gpio_set_level(PCM422X_SM, adcIsSlave);
}

bool PCM4220::get_overflow(pcm422x_channel channel)
{
    // Per-channel clipping indicator
    switch (channel)
    {
        case (CHANNEL_RIGHT):
            return gpio_get_level(PCM422X_OVFR);
        default:
            return gpio_get_level(PCM422X_OVFL);
    }
}

PCM4222::PCM4222() :
PCM4220()
{
}

PCM4222::~PCM4222()
{
}

gpio_config_t PCM4222::_gpio_output_conf()
{
    gpio_config_t gpioOutputConf = {
            pin_bit_mask : PCM4222_GPIO_OUTPUT_PINS,
            mode : GPIO_MODE_OUTPUT,
            pull_up_en : GPIO_PULLUP_DISABLE,
            pull_down_en : GPIO_PULLDOWN_DISABLE,
            intr_type : static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE)
        };
    return gpioOutputConf;
}

void PCM4222::enable_pcm()
{
    PCM4220::enable_pcm();
    
    // Disable DSD
    gpio_set_level(PCM422X_DSDEN, 0);
    
    // Disable MBM
    gpio_set_level(PCM422X_MODEN, 0);
}
