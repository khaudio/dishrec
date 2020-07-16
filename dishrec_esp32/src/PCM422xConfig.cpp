// #include "PCM422xConfig.h"

// PCM4220::PCM4220()
// {
//     gpio_config_t gpio_output_conf = {
//             pin_bit_mask : GPIO_OUTPUT_PINS,
//             mode : GPIO_MODE_OUTPUT,
//             pull_up_en : GPIO_PULLUP_DISABLE,
//             pull_down_en : GPIO_PULLDOWN_DISABLE,
//             intr_type : GPIO_PIN_INTR_DISABLE
//         };
//     gpio_config_t gpio_input_conf = {
//             pin_bit_mask : GPIO_INPUT_PINS,
//             mode : GPIO_MODE_INPUT,
//             pull_up_en : GPIO_PULLUP_DISABLE,
//             pull_down_en : GPIO_PULLDOWN_ENABLE,
//             intr_type : GPIO_PIN_INTR_DISABLE
//         };
//     gpio_config(&gpio_output_conf);
//     gpio_config(&gpio_input_conf);

//     gpio_set_level(SM, 1);

//     enable_pcm();
//     set_i2s();
//     set_hpf(1);
//     set_bit_depth(24);
//     set_sample_rate(48000);
// }

// PCM4220::~PCM4220()
// {
// }

// void PCM4220::set_bit_depth(uint16_t bitsPerSample, bool isFloat)
// {
//     switch (bitsPerSample)
//     {
//         case (24):
//             gpio_set_level(OWL0, 0);
//             gpio_set_level(OWL1, 0);
//             break;
//         case (20):
//             gpio_set_level(OWL0, 0);
//             gpio_set_level(OWL1, 1);
//             break;
//         case (18):
//             gpio_set_level(OWL0, 1);
//             gpio_set_level(OWL1, 0);
//             break;
//         case (16):
//             gpio_set_level(OWL0, 1);
//             gpio_set_level(OWL1, 1);
//             break;
//         default:
//             throw INVALID_BIT_DEPTH;
//     }
// }

// void PCM4220::set_sample_rate(uint32_t samplerate)
// {
//     if ((samplerate >= 8000) && (samplerate <= 54000))
//     {
//         gpio_set_level(FS0, 0);
//         gpio_set_level(FS1, 0);
//     }
//     else if ((samplerate > 54000) && (samplerate <= 108000))
//     {
//         gpio_set_level(FS0, 1);
//         gpio_set_level(FS1, 0);
//     }
//     else if ((samplerate > 108000) && (samplerate <= 216000))
//     {
//         gpio_set_level(FS0, 0);
//         gpio_set_level(FS1, 1);
//     }
//     else
//     {
//         throw INVALID_SAMPLERATE;
//     }
// }

// void PCM4222::enable_pcm()
// {
//     // Enable PCM output
//     gpio_set_level(PCMEN, 1);
// }

// void PCM4220::set_i2s()
// {
//     // Set output to I2S
//     gpio_set_level(FMT0, 1);
//     gpio_set_level(FMT1, 0);

//     // Set TDM-related pins low
//     gpio_set_level(SUB0, 0);
//     gpio_set_level(SUB1, 0);
// }

// void PCM4220::set_hpf(bool isEnabled)
// {
//     // Filter out DC in both channels
//     gpio_set_level(HPFDR, isEnabled);
//     gpio_set_level(HPFDL, isEnabled);
// }

// bool PCM4220::get_overflow(pcm422x_channel channel)
// {
//     // Per-channel clipping indicator
//     switch (channel)
//     {
//         case (CHANNEL_RIGHT):
//             return gpio_get_level(OVFR);
//         default:
//             return gpio_get_level(OVFL);
//     }
// }

// PCM4222::PCM4222() :
// PCM4220()
// {
// }

// PCM4222::~PCM4222()
// {
// }

// void PCM4222::enable_pcm()
// {
//     PCM4220::enable_pcm();
    
//     // Disable DSD
//     gpio_set_level(DSDEN, 0);
    
//     // Disable MBM
//     gpio_set_level(MODEN, 0);
// }
