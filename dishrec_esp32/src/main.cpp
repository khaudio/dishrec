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

#include <cstdlib>
#include <thread>
#include "WavWriter.h"
#include "Esp32Button.h"
#include "EspI2S.h"
#include "EspDataIngestor.h"
#include "Timer.h"
#include "WDT.h"

/*                              Macros                              */

/* Input word select */
#define I2S_WS                      (GPIO_NUM_26)

/* Input bit clock */
#define I2S_BCK                     (GPIO_NUM_14)

/* Input data */
#define I2S_DI                      (GPIO_NUM_35)

/* Output data */
#define I2S_DO                      (GPIO_NUM_25)

/* Shutdown signal output */
#define I2S_SHUTDOWN                (GPIO_NUM_21)

#define REC_STOP_BUTTON             (GPIO_NUM_27)

/*                          Global variables                        */

/* SD Card */
EspSD::SDCard sdcard;

/* File writer */
WavFile::WavWriter wav;

/* Record/Stop button */
Esp32Button::DualActionButton recStopButton(REC_STOP_BUTTON, 0);

/* Current format */
WavMeta::WavFormatData currentFormat = {
    .sampleRate = 48000,
    .bitDepth = 16,
    .numChannels = 2,
    .sampleWidth = 2,
    .formatCode = WavMeta::FORMAT_PCM
};

DataIngestor ingestor;

/* Prepare to record */
bool armed(false);

/* Save to storage when true */
bool recording(false);

/* Index to keep track of how many samples
have been recorded; used to stop recording
on a SMPTE TC frame boundary */
static size_t sampleIndex(0);

/* Thread container */
std::vector<std::thread> threads;

/*                            Prototypes                            */

int main();

/* esp-idf entry point */
void app_main();

/* Arduino entry point */
void setup();

/* Arduino loop */
void loop();

/* Set ring buffer size */
void set_buffer_size();

/* Thread loops */
void read_from_i2s();
void save_to_storage();
void set_recording_flags();
void check_rec_button();

/* Checks for SMPTE timecode frame boundary */
bool is_frame_boundary();

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

    std::cout << "Setting ingestor..." << std::endl;
    ingestor.set_format(currentFormat);

    ingestor.set_shutdown_pin(I2S_SHUTDOWN);
    ingestor.set_pins(I2S_WS, I2S_BCK, I2S_DI, I2S_DO);

    wav.set_format(currentFormat);
    wav.set_framerate(23.98);
    wav.set_dropframe(false);
    wav.set_timecode(12, 0, 0, 0);

    std::cout << "Setting buffer size..." << std::endl;

    /* Set input ring buffer to 1/8 SMPTE frame length */
    size_t length = wav.samples_per_frame() / 8;
    std::cout << "Setting size to " << length << " samples..." << std::endl;
    ingestor.set_size(length, 4);
    std::cout << "Set size to " << ingestor.size() << " bytes" << std::endl;

    std::cout << "Ingestor bit depth == " << ingestor.bitDepth << std::endl;

    std::cout << "Setting sample delay..." << std::endl;

    /* Timestamp compensation for ring buffer delay */
    size_t numSampleDelay = ingestor.delay_in_samples() / wav.samples_per_frame();
    wav.set_timecode(wav.get_frames() - numSampleDelay);

    std::cout << "Starting I2S..." << std::endl;

    ingestor.start();

    std::cout << "Setting button..." << std::endl;

    recStopButton.set_hold_duration_ms(10);

    // std::cout << "Starting threads..." << std::endl;

    // threads.reserve(4);
    // std::cout << "Starting I2S read thread..." << std::endl;
    // threads.emplace_back(std::thread(&read_from_i2s));
    // std::cout << "Starting SD write thread..." << std::endl;
    // threads.emplace_back(std::thread(&save_to_storage));
    // std::cout << "Starting recording flag setter thread..." << std::endl;
    // threads.emplace_back(std::thread(&set_recording_flags));
    // std::cout << "Starting rec/stop button watch thread..." << std::endl;
    // threads.emplace_back(std::thread(&check_rec_button));

    std::cout << "Initialized" << std::endl;
}

void loop()
{
    static int takeNumber(0);
    static char filename[32];

    if (!recording && recStopButton)
    {
        int i(0);
        std::cout << "Mounting SD card..." << std::endl;
        while (!sdcard && (i++ < 3)) sdcard.mount();
        if (!sdcard)
        {
            std::cout << "Could not mount SD card" << std::endl;
            goto SKIP;
        }
        sprintf(filename, "wavefile_%03d", takeNumber++);

        std::cout << "Setting filename..." << std::endl;
        wav.set_filename(filename);
        std::cout << "Opening file..." << std::endl;
        wav.open();
        armed = true;
        std::cout << "File opened" << std::endl;
    }
    else if (sdcard && recStopButton && recording && armed)
    {
        std::cout << "Disarming..." << std::endl;
        armed = false;
    }

    {
        /* Blocks while waiting for I2S input buffer */
        // sampleIndex += ingestor.step();

        /* This check is not needed if externally connected
        timecode clock is keeping time (e.g. dish.tc) */
        // if (is_frame_boundary()) wav.tick();
    }

    {
        // if (recording && ingestor.buffered() > 0)
        if (recording)
        {
            static const size_t ingestorSize(ingestor.size());
            // wav.write(ingestor.read(), ingestorSize);
            wav.write(ingestor._read_raw(), ingestorSize);
        }
        // else if (recording) std::cerr << "Ring buffer is empty!" << std::endl;
    }

    {
        if (!recording && armed)
        {
            /* Wait for frame boundary */
            // while (!is_frame_boundary()) vTaskDelay(1);
            recording = true;
            std::cout << "Started record..." << std::endl;
        }
        else if (recording && !armed)
        {
            /* Wait for frame boundary */
            // while (!is_frame_boundary()) vTaskDelay(1);
            std::cout << "Stopping record..." << std::endl;
            recording = false;
            wav.close();
            std::cout << "Unmounting SD card..." << std::endl;
            sdcard.unmount();
            std::cout << "Unmounted SD card" << std::endl;
        }
    }

    SKIP:;

    {
        if (recStopButton.read())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

}

void read_from_i2s()
{
    while (true)
    {
        /* Blocks while waiting for I2S input buffer */
        sampleIndex += ingestor.step();

        /* This check is not needed if external
        timecode clock is ticking (e.g. dish.tc) */
        if (is_frame_boundary()) wav.tick();
        
        // vTaskDelay(10);
        std::this_thread::yield();
    }
}

void save_to_storage()
{
    while (true)
    {
        // vTaskDelay(1);
        std::this_thread::yield();
        if (!recording) continue;
        wav.write(ingestor.read(), ingestor.size());
    }
}

void set_recording_flags()
{
    while (true)
    {
        if (!recording && armed)
        {
            /* Wait for frame boundary */
            // while (!is_frame_boundary()) vTaskDelay(1);
            recording = true;
            std::cout << "Started record..." << std::endl;
        }
        else if (recording && !armed)
        {
            /* Wait for frame boundary */
            // while (!is_frame_boundary()) vTaskDelay(1);
            std::cout << "Stopping record..." << std::endl;
            recording = false;
            wav.close();
            // vTaskDelay(10);
            std::this_thread::yield();
            std::cout << "Unmounting SD card..." << std::endl;
            sdcard.unmount();
            std::cout << "Unmounted SD card" << std::endl;
        }
        // vTaskDelay(50);
        std::this_thread::yield();
    }
}

void check_rec_button()
{
    while (true)
    {
        /* Soft debounce when button is pressed */
        if (recStopButton.read())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        // vTaskDelay(100);
        std::this_thread::yield();
    }
}

inline bool is_frame_boundary()
{
    /* Will never return true if samples per frame
    is not a multiple of I2S input buffer */
    return (!(sampleIndex % wav.samples_per_frame()));
}
