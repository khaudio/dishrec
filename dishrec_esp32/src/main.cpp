// /*----------------------------------------------------------------------------
//  * Copyright 2020 K Hughes Production, LLC
//  * All Rights Reserved.
//  *----------------------------------------------------------------------------

// Dishrec configures an external ADC (PCM4220/PCM4222) via pin configs,
// receives audio via I2S input from the ADC, writes it to a ring buffer,
// then reads from the ring buffer and writes to an SD card
// when recording is enabled and armed.

// Input:
// ADC --(I2S)--> Input Ring Buffer --(SPI)--> SD Card

// There is a separate ring buffer for the output.
// The ESP32 internal DAC can be used for file playback, if desired.

// Output:
// SD Card --(SPI)--> Output Ring Buffer --(I2S)--> Internal/Discrete DAC

//                             ESP32 pin connections

//     Connection                      Input/Output    Pin #               Description

// SD Card (SPI)
//     CS                              O               5                   SD Chip Select
//     MOSI                            O               23                  Data to SD
//     MISO                            I               19                  Data from SD
//     CLK                             O               18                  SPI Clock

// ESP32 I2S
//     I2S_IN_WS                       I               32                  I2S input WS
//     I2S_IN_BCK                      I               33                  I2S input SCK
//     I2S_IN_DI                       I               39                  I2S input data
//     I2S_OUT_DO                      O               25                  I2S output data
//     I2S_MCK                         O               0                   I2S MCK output
//     I2S_SHUTDOWN                    O               21                  I2S shutdown signal

// PCM4220/PCM4222
//     PCM422X_PCMEN                   O               22                  Pcm output enable
//     PCM422X_HPFDR/PCM422X_MOD1      O               2                   Right channel high-pass filter disable/modulator data 1 output
//     PCM422X_HPFDL/PCM422X_MOD2      O               15                  Input left channel high-pass filter disable/modulator data 2 output
//     PCM422X_FS0/PCM422X_MOD3        O               9                   Sampling mode config 0/modulator data 3 output
//     PCM422X_FS1/PCM422X_MOD4        O               10                  Sampling mode config 1/modulator data 4 output
//     PCM422X_DF/PCM422X_MOD5         O               27                  Digital decimation filter response/modulator data 5 output
//     PCM422X_SUB0/PCM422X_WCKO       O               17                  TDM active sub-frame config 0/modulator left/right word clock output
//     PCM422X_SUB1/PCM422X_MCK0       O               1                   TDM active sub-frame config 1/master clock output
//     PCM422X_DATA                    I/O             (I2S_IN_DI)         ADC I2S data
//     PCM422X_BCK                     I/O             (I2S_IN_BCK)        ADC I2S SCK
//     PCM422X_LRCK                    I/O             (I2S_IN_WS)         ADC I2S WS
//     PCM422X_MCKI                    O               (I2S_MCK)           ADC I2S MCK
//     PCM422X_RST                     O               13                  ADC reset and power-down
//     PCM422X_OVFL                    I               34                  Left channel overflow flag
//     PCM422X_OVFR                    I               35                  Right channel overflow flag
//     PCM422X_SM                      O               26                  ADC I2S slave/master mode
//     PCM422X_OWL0                    O               14                  Output word length config 0
//     PCM422X_OWL1                    O               12                  Output word length config 1
//     PCM422X_FMT0                    O               8                   Audio data format config 0
//     PCM422X_FMT1                    O               7                   Audio data format config 1

// PCM4222 Only
//     PCM422X_DSDEN/PCM422X_MOD6      O               3                   DSD output enable/modulator data 6 output
//     PCM422X_MODEN                   O               6                   Multi-bit modulator output enable
//     PCM422X_DSDMODE                 O               11                  DSD mode output/rate

// Button
//     REC_STOP_BUTTON                 I               27                  Button to start/stop record to SD */


// #include <driver/gpio.h>
// #include "I2SIO.h"
// #include "iXML.h"
// #include "TimecodeBase.h"

// /*                              Macros                              */

// // Record/Stop button; active low
// #define REC_STOP_BUTTON             (GPIO_NUM_27)


// /*                          Global variables                        */

// // Button to initiate or stop recording
// std::shared_ptr<bool> buttonState = std::make_shared<bool>(false); // Not recording at boot

// // Default wav file parameters
// extern WavParameters currentWavFormat;

// // Input ring buffer
// extern RingBuffer<DATATYPE> inBuffer;

// // Output ring buffer
// extern RingBuffer<DATATYPE> outBuffer;

// // Wav file currently open for read or write
// extern std::shared_ptr<EspSDWavFile> currentWriteFile;
// extern std::shared_ptr<EspSDWavFile> currentReadFile;

// // Filename of current wav file
// extern std::string currentWriteFilename;

// // Delimiter between filename scene and take
// extern std::string filenameDelimiter;

// // Take number for current wav file
// extern int currentWriteFileNumber;

// // iXML data written to or read from broadcast wav
// iXML::IXML ixml;

// // Timecode value of file
// TimecodeBase::Clock tc;


// /*                              Prototypes                          */

// // esp-idf entry point
// void app_main(void);

// // Arduino entry point
// void setup();

// // Arduino loop
// void loop();

// void set_button_gpio();
// void open_file();
// void close_file();
// void write_if_buffered_button(std::shared_ptr<bool> state);


// /*                          Definitions                             */

// void app_main(void)
// {
//     setup();
//     while (true)
//     {
//         loop();
//     }
// }

// void setup()
// {
//     std::cout << "Initializing" << std::endl;

//     std::cout << "Setting timecode parameters" << std::endl;

//     tc.set_sample_rate(currentWavFormat.sampleRate);
//     tc.set_framerate(23.976, true);

//     std::cout << "Timecode parameters set" << std::endl;
//     std::cout << "Setting record button GPIO" << std::endl;
    
//     set_button_gpio();
    
//     std::cout << "Record button GPIO set" << std::endl;
//     std::cout << "Creating file objects..." << std::endl;

//     init_wav_file_objects();
//     currentReadFile->open_read("readtest");

//     std::cout << "File objects created" << std::endl;
//     std::cout << "Filling output buffer from file..." << std::endl;

//     for (int i(0); i < outBuffer.ringLength; ++i) read_file_to_buffer<DATATYPE>(&outBuffer);

//     std::cout << "Output buffer filled" << std::endl;
//     std::cout << "Delaying 500ms..." << std::endl;
//     ets_delay_us(500000);
//     std::cout << "Starting I2S" << std::endl;

//     I2S::start();

//     std::cout << "I2S started" << std::endl;
//     std::cout << "Initialized" << std::endl;
// }

// void loop()
// {
//     I2S::read_to_buffer(&inBuffer);
//     *buttonState = gpio_get_level(REC_STOP_BUTTON);
//     write_if_buffered_button(buttonState);
// }

// void set_button_gpio()
// {
//     gpio_config_t buttonGpioConf = {
//             pin_bit_mask : (1ULL << REC_STOP_BUTTON),
//             mode : GPIO_MODE_INPUT,
//             pull_up_en : GPIO_PULLUP_ENABLE,
//             pull_down_en : GPIO_PULLDOWN_DISABLE,
//             intr_type : static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE)
//         };
//     gpio_config(&buttonGpioConf);
// }

// void open_file()
// {
//     open_new_file();
//     ets_delay_us(500000); // 500ms debounce
//     #ifdef _DEBUG
//     std::cout << "Recording " << currentWriteFile->filename << "..." << std::endl;
//     #endif
// }

// void close_file()
// {
//     #ifdef _DEBUG
//     std::cout << "Closing file " << currentWriteFile->filename << "..." << std::endl;
//     #endif
//     currentWriteFile->close();
//     std::cout << "Closed file" << currentWriteFile->filename << std::endl;
//     ets_delay_us(500000); // 500ms debounce
// }

// void write_if_buffered_button(std::shared_ptr<bool> state)
// {
//     if (*state && currentWriteFile->is_open())
//     {
//         while (inBuffer.sub_buffers_full() > 0)
//         {
//             write_to_file();
//             inBuffer.rotate_read_buffer();
//         }
//     }
//     else if (!*state && currentWriteFile->is_open())
//     {
//         close_file();
//     }
//     else if (!*state && !currentWriteFile->is_open())
//     {
//         open_file();
//     }
// }

int main(); void setup() {main();} void loop() {}
