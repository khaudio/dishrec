/*----------------------------------------------------------------------------
 * Copyright 2020 K Hughes Production, LLC
 * All Rights Reserved.
 *----------------------------------------------------------------------------

Dishrec configures an external ADC (PCM4220/PCM4222) via pin configs,
receives audio via I2S input from the ADC, writes it to a ring buffer,
then reads from the ring buffer and writes to an SD card
when recording is enabled and armed.

Input:
ADC --(I2S)--> Input Ring Buffer --(SPI)--> SD Card

There is a separate ring buffer for the output.
The ESP32 internal DAC can be used for file playback, if desired.

Output:
SD Card --(SPI)--> Output Ring Buffer --(I2S)--> Internal/Discrete DAC

                            ESP32 pin connections

    Connection                      Input/Output    Pin #               Description

SD Card (SPI)
    CS                              O               5                   SD Chip Select
    MOSI                            O               23                  Data to SD
    MISO                            I               19                  Data from SD
    CLK                             O               18                  SPI Clock

ESP32 I2S
    I2S_IN_WS                       I               32                  I2S input WS
    I2S_IN_BCK                      I               33                  I2S input SCK
    I2S_IN_DI                       I               39                  I2S input data
    I2S_OUT_DO                      O               25                  I2S output data
    I2S_MCK                         O               0                   I2S MCK output
    I2S_SHUTDOWN                    O               21                  I2S shutdown signal

PCM4220/PCM4222
    PCM422X_PCMEN                   O               22                  Pcm output enable
    PCM422X_HPFDR/PCM422X_MOD1      O               2                   Right channel high-pass filter disable/modulator data 1 output
    PCM422X_HPFDL/PCM422X_MOD2      O               15                  Input left channel high-pass filter disable/modulator data 2 output
    PCM422X_FS0/PCM422X_MOD3        O               9                   Sampling mode config 0/modulator data 3 output
    PCM422X_FS1/PCM422X_MOD4        O               10                  Sampling mode config 1/modulator data 4 output
    PCM422X_DF/PCM422X_MOD5         O               27                  Digital decimation filter response/modulator data 5 output
    PCM422X_SUB0/PCM422X_WCKO       O               17                  TDM active sub-frame config 0/modulator left/right word clock output
    PCM422X_SUB1/PCM422X_MCK0       O               1                   TDM active sub-frame config 1/master clock output
    PCM422X_DATA                    I/O             (I2S_IN_DI)         ADC I2S data
    PCM422X_BCK                     I/O             (I2S_IN_BCK)        ADC I2S SCK
    PCM422X_LRCK                    I/O             (I2S_IN_WS)         ADC I2S WS
    PCM422X_MCKI                    O               (I2S_MCK)           ADC I2S MCK
    PCM422X_RST                     O               13                  ADC reset and power-down
    PCM422X_OVFL                    I               34                  Left channel overflow flag
    PCM422X_OVFR                    I               35                  Right channel overflow flag
    PCM422X_SM                      O               26                  ADC I2S slave/master mode
    PCM422X_OWL0                    O               14                  Output word length config 0
    PCM422X_OWL1                    O               12                  Output word length config 1
    PCM422X_FMT0                    O               8                   Audio data format config 0
    PCM422X_FMT1                    O               7                   Audio data format config 1

PCM4222 Only
    PCM422X_DSDEN/PCM422X_MOD6      O               3                   DSD output enable/modulator data 6 output
    PCM422X_MODEN                   O               6                   Multi-bit modulator output enable
    PCM422X_DSDMODE                 O               11                  DSD mode output/rate

Button
    REC_STOP_BUTTON                 I               27                  Button to start/stop record to SD */

#include "AudioDataPad.h"
#include "WavWriter.h"
#include "RingBuffer.h"
#include "Esp32Button.h"
#include "EspI2S.h"
#include "Timer.h"
#include <cstdlib>
#include <thread>

/*                              Macros                              */

// Input word select
#define I2S_WS                      (GPIO_NUM_32)

// Input bit clock
#define I2S_BCK                     (GPIO_NUM_33)

// Input data
#define I2S_DI                      (GPIO_NUM_39)

// Output data
#define I2S_DO                      (GPIO_NUM_25)

// Shutdown signal output
#define I2S_SHUTDOWN                (GPIO_NUM_21)

#define REC_STOP_BUTTON             (GPIO_NUM_27)


/*                          Global variables                        */

// SD Card
EspSD::SDCard sdcard;

// File writer
WavFile::WavWriter wav;

// Record/Stop button
Esp32Button::DualActionButton recStopButton(REC_STOP_BUTTON, 0);

// Current format
WavMeta::WavFormatData currentFormat = {
    .sampleRate = 48000,
    .bitDepth = 24,
    .numChannels = 1,
    .sampleWidth = 3,
    .formatCode = WavMeta::FORMAT_PCM
};

// I2S Input
I2SBus i2sIn;

Buffer::RingBuffer<int_audio> inBuffer(64, 4);

const size_t bytesPerBuffer(inBuffer.bufferLength * wav.sampleWidth);

Packer packer;

// Prepare to record
bool armed(false);

// Save to storage when true
bool recording(false);

/*                            Prototypes                            */

int main();

// esp-idf entry point
void app_main();

// Arduino entry point
void setup();

// Arduino loop
void loop();

/*                            Definitions                           */

int main()
{
    setup();
    while (true)
    {
        loop();
    }
    return 0;
}

void app_main()
{
    main();
}

void setup()
{
    std::cout << "Initializing..." << std::endl;

    wav.set_format(currentFormat);

    packer.set_bit_depth(24);

    i2sIn.set_shutdown_pin(I2S_SHUTDOWN);
    i2sIn.set_sample_rate(wav.sampleRate);
    i2sIn.set_bit_depth(wav.bitDepth);
    i2sIn.set_channels(wav.numChannels);
    i2sIn.set_buffer_size(inBuffer.bufferLength);
    i2sIn.set_pins(I2S_WS, I2S_BCK, I2S_DI, I2S_DO);
    i2sIn.config();
    i2sIn.start();

    wav.set_framerate(23.98);
    wav.set_dropframe(false);
    wav.set_timecode(12, 0, 0, 0);

    recStopButton.set_hold_duration_ms(5);

    std::cout << "Initialized" << std::endl;
}

void loop()
{
    static int takeNumber(0);
    static char filename[32];
    // static bool recording(false), armed(false);
    
    // uint8_t packedFromI2S[bytesPerBuffer];
    // uint8_t packedToCard[bytesPerBuffer];
    // std::vector<int_audio>* inBufferReadPtr = &inBuffer.ring[inBuffer.readIndex];
    // std::vector<int_audio>* inBufferWritePtr = &inBuffer.ring[inBuffer.writeIndex];

    // i2sIn.read(packedFromI2S, bytesPerBuffer);
    // packer.unpack<int_audio>(inBufferWritePtr, packedFromI2S);
    // inBuffer.force_rotate_write_buffer();

    if (!recording && recStopButton)
    {
        int i(0);
        while (!sdcard && (i++ < 3)) sdcard.mount();
        if (!sdcard) goto SKIP;
        std::cout << "Starting record" << std::endl;
        sprintf(filename, "wavefile_%03d", takeNumber++);
        wav.set_filename(filename);
        wav.open();
        armed = true;
    }
    else if (sdcard && recStopButton && recording)
    {
        std::cout << "Stopping record" << std::endl;
        wav.close();
        armed = false;
        recording = false;
        std::cout << "Stopped record" << std::endl;
        std::cout << "Unmounting SD card..." << std::endl;
        sdcard.unmount();
        std::cout << "Unmounted SD card" << std::endl;
    }
    else if (recording)
    {
        packer.pack<int_audio>(packedToCard, inBufferReadPtr);
        wav.write(packedToCard, bytesPerBuffer);
        inBuffer.rotate_read_buffer();
    }
    else if (!recording && armed)
    {
        // Wait for frame boundary here
        recording = true;
        std::cout << "Started record" << std::endl;
    }

    SKIP:

    // Read and debounce button if active
    if (recStopButton.read()) std::this_thread::sleep_for(
            std::chrono::milliseconds(10)
        );
}

void i2s_to_buffer()
{
    uint8_t packedFromI2S[bytesPerBuffer];
    std::vector<int_audio>* inBufferWritePtr = &inBuffer.ring[inBuffer.writeIndex];
    while (true)
    {
        i2sIn.read(packedFromI2S, bytesPerBuffer);
        packer.unpack<int_audio>(inBufferWritePtr, packedFromI2S);
        inBuffer.force_rotate_write_buffer();
    }
}

void buffer_to_storage()
{
    uint8_t packedToCard[bytesPerBuffer];
    std::vector<int_audio>* inBufferReadPtr = &inBuffer.ring[inBuffer.readIndex];
    while (true)
    {
        if (recording)
        {
            packer.pack<int_audio>(packedToCard, inBufferReadPtr);
            wav.write(packedToCard, bytesPerBuffer);
        }
        inBuffer.rotate_read_buffer();
    }
}

void check_rec_button()
{
    while (true)
    {
        if (recStopButton.read()) std::this_thread::sleep_for(
                std::chrono::milliseconds(10)
            );
    }
}
