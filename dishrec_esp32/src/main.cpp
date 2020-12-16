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
// #define I2S_DO                      (GPIO_NUM_25)
#define I2S_DO                      (GPIO_NUM_22)





/* Shutdown signal output */
#define I2S_SHUTDOWN                (GPIO_NUM_21)

#define REC_STOP_BUTTON             (GPIO_NUM_27)

/*                          Global variables                        */

/* SD Card */
EspSD::SDCard sdcard;

/* File writer */
WavFile::WavWriter wav;

/* Record/Stop button (active when high) */
Esp32Button::DualActionButton recStopButton(REC_STOP_BUTTON, true);

/* Default audio format */
WavMeta::WavFormatData currentFormat = {
    .sampleRate = 48000,
    .bitDepth = 24,
    .numChannels = 2,
    .sampleWidth = 3,
    .formatCode = WavMeta::FORMAT_PCM
};

/* Audio buffer and I2S ingestor */
DataIngestor ingestor;

/* Prepare to record */
bool armed(false);

/* Write buffer to storage when active */
bool recording(false);

/* Index to keep track of how many samples
have been recorded; used to start/stop recording
on a SMPTE timecode frame boundary */
static size_t sampleIndex(0);

/*                            Prototypes                            */

int main();

/* esp-idf entry point */
void app_main();

/* Arduino entry point */
void setup();

/* Arduino loop */
void loop();

/* Thread loops */
void read_from_i2s(void*);
void save_to_storage(void*);

/* Arms and disarms for recording */
void set_recording_flags();

/* Checks for SMPTE timecode frame boundary */
bool is_frame_boundary();



void setup_i2s();
void read_from_i2s_input(uint8_t* buff, size_t numBytes);
void write_to_i2s_output(uint8_t* buff, size_t numBytes);



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
    std::cout << "Ingestor sample rate set to " << ingestor.sampleRate << std::endl;
    std::cout << "Ingestor bit depth set to " << ingestor.bitDepth << std::endl;
    std::cout << "Ingestor sample width set to " << ingestor.sampleWidth << std::endl;

    ingestor.reset_mode();

    ingestor.set_master();
    ingestor.set_receive();
    ingestor.set_transmit();

    ingestor.set_pins(I2S_WS, I2S_BCK, I2S_DI, I2S_DO);

    wav.set_format(currentFormat);
    wav.set_framerate(23.98);
    wav.set_dropframe(false);
    wav.set_timecode(12, 0, 0, 0);

    std::cout << "Setting buffer size..." << std::endl;

    // /* Set input ring buffer size to 1/4 SMPTE frame length; e.g.,
    // at 48000 sample rate / 24fps == 2000 samples per frame,
    // so length is (2000 / 4) == 500 samples.
    // Maximum size is 1000 samples. */
    size_t length = wav.samples_per_frame() / 4;
    std::cout << "Setting size to " << length << " samples..." << std::endl;
    
    ingestor.set_size(length, 4);

    std::cout << "Set size to " << ingestor.size() << " bytes" << std::endl;
    std::cout << "Ingestor bit depth == " << ingestor.bitDepth << std::endl;

    std::cout << "Setting sample delay..." << std::endl;
    
    /* Timestamp compensation for ring buffer delay */
    size_t numSampleDelay = ingestor.delay_in_samples() / wav.samples_per_frame();
    wav.set_timecode(wav.get_frames() - numSampleDelay);

    std::cout << "Starting I2S..." << std::endl;

    // setup_i2s();

    ingestor.start();

    std::cout << "Setting button..." << std::endl;

    recStopButton.set_debounce_duration_ms(500);

    std::cout << "Starting threads..." << std::endl;

    std::cout << "Starting I2S read thread..." << std::endl;

    /* Pin time-critical data ingestion to ESP32 core 0 */
    xTaskCreatePinnedToCore(
            reinterpret_cast<TaskFunction_t>(read_from_i2s),
            "i2sreader", 10000, NULL, 9, NULL, 0
        );
    
    std::cout << "Initialized" << std::endl;
}

void loop()
{
    // static int takeNumber(1);
    // static char filename[64], timecodeStr[9];
    // static std::array<int, 4> timecodeValues;

    // if (!recording && recStopButton)
    // {
    //     std::cout << "Mounting SD card..." << std::endl;
        
    //     /* Make up to three attempts to mount SD card */
    //     int i(0);
    //     while (!sdcard && (i++ < 3))
    //     {
    //         sdcard.mount();
    //         std::this_thread::sleep_for(std::chrono::milliseconds(200));
    //     }
    //     if (!sdcard)
    //     {
    //         std::cout << "Could not mount SD card" << std::endl;
    //         goto SKIP;
    //     }

    //     /* Save timecode in filename */
    //     timecodeValues = wav.get_timecode();
    //     sprintf(
    //             timecodeStr,
    //             "%02u%02u%02u%02u",
    //             timecodeValues[0],
    //             timecodeValues[1],
    //             timecodeValues[2],
    //             timecodeValues[3]
    //         );
    //     timecodeStr[8] = '\0';

    //     /* Add a three-digit counter to filename */
    //     sprintf(filename, "%s_dishrec_%03d", timecodeStr, takeNumber);

    //     std::cout << "Setting filename..." << std::endl;
    //     wav.set_filename(filename);
        
    //     std::cout << "Opening file " << wav.get_filename() << "..." << std::endl;
        
    //     /* Arm for record */
    //     if (wav.open())
    //     {
    //         armed = true;
    //         ++takeNumber;
    //         std::cout << "File opened" << std::endl;
    //     }
    //     else
    //     {
    //         std::cerr << "Could not open file" << std::endl;
    //     }

    //     /* Unset the button to avoid erroneous rapid file creation */
    //     recStopButton.set(false, true);
    // }
    // else if (sdcard && recStopButton && recording && armed)
    // {
    //     /* Disarm */
    //     std::cout << "Disarming..." << std::endl;
    //     armed = false;
    //     recStopButton.set(false, true);
    // }

    // /* Set recording state if armed */
    // set_recording_flags();

    // /* Skip to here if SD card could not be mounted */
    // SKIP:;

    // /* Read button for change in state */
    // recStopButton.read();

    // /* Reset FreeRTOS watchdog to avoid timeouts */
    reset_wdt_1();
}

void read_from_i2s(void* pvParameters)
{
    // static const size_t ingestorSize(ingestor.size());

    static const size_t ingestorSize = (wav.samples_per_frame() / 4) * 4;
    uint8_t buff[ingestorSize], *ptr;
    memset(buff, 0, ingestorSize);

    while (true)
    {
        /* Blocks while waiting for I2S input buffer */
        sampleIndex += ingestor.step();
        
        /* Write buffered samples to card */
        // if (recording) wav.write(ingestor.read_raw(), ingestorSize);
        
        ptr = ingestor.read();
        ingestor.write(ptr, ingestorSize);
        // visualize<int_audio>(ptr, ingestorSize, 3, 70, false);

        // read_from_i2s_input(buff, ingestorSize);
        // write_to_i2s_input(ptr, ingestorSize);
        // if (recording) wav.write(buff, ingestorSize);

        // /* This check is not needed if external
        // timecode clock is keeping time (e.g. dish.tc) */
        // if (is_frame_boundary()) wav.tick();

        /* Reset FreeRTOS watchdog to avoid timeouts */
        reset_wdt_0();
    }
}

// void save_to_storage(void* pvParameters)
// {
//     static const size_t ingestorSize(ingestor.size());

//     while (true)
//     {
//         /* This check is not needed if external
//         timecode clock is ticking (e.g. dish.tc) */
//         if (is_frame_boundary()) wav.tick();

//         /* Check if samples are buffered */
//         if (!recording || !ingestor.buffered())
//         {
//             std::this_thread::yield();
//             continue;
//         };

//         /* Write buffered samples to card */
//         wav.write(ingestor.read(), ingestorSize);
//         std::this_thread::yield();
//     }
// }

inline void set_recording_flags()
{
    /* Start recording */
    if (!recording && armed)
    {
        /* Wait for frame boundary */
        // while (!is_frame_boundary()) std::this_thread::yield();

        recording = true;
        std::cout << "Started record..." << std::endl;
    }

    /* Stop recording */
    else if (recording && !armed)
    {
        /* Wait for frame boundary */
        // while (!is_frame_boundary()) std::this_thread::yield();

        std::cout << "Stopping record..." << std::endl;
        
        recording = false;

        std::cout << "Closing file..." << std::endl;

        /* Close file */
        wav.close();
        
        std::cout << "Closed file " << wav.get_filename() << std::endl;
        std::cout << "File size: " << wav.get_file_size() << std::endl;
        std::cout << "Unmounting SD card..." << std::endl;
        
        /* Unmount SD card when not recording */
        sdcard.unmount();

        std::cout << "Unmounted SD card" << std::endl;
    }
}

inline bool is_frame_boundary()
{
    /* Will never return true if samples per frame
    is not a multiple of I2S input buffer size */
    return (!(sampleIndex % wav.samples_per_frame()));
}









void setup_i2s()
{
    i2s_config_t config;
    config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX);
    config.sample_rate = currentFormat.sampleRate;
    config.bits_per_sample = (i2s_bits_per_sample_t)(32);
    config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    config.communication_format = I2S_COMM_FORMAT_I2S_MSB; // 0x01 or 0x03
    config.intr_alloc_flags = 0;
    config.dma_buf_count = 8;
    config.dma_buf_len = (wav.samples_per_frame() / 8) * 2; // double the ring buffer length
    config.use_apll = true;
    config.tx_desc_auto_clear = true;

    i2s_pin_config_t pinconfig;
    pinconfig.ws_io_num = (GPIO_NUM_26);
    pinconfig.bck_io_num = (GPIO_NUM_14);
    pinconfig.data_in_num = (GPIO_NUM_35);
    pinconfig.data_out_num = (GPIO_NUM_22);

    i2s_driver_install(I2S_NUM_0, &config, 0, NULL);

    i2s_set_pin(I2S_NUM_0, &pinconfig);

    REG_WRITE(PIN_CTRL, 0xFF0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
}

inline void read_from_i2s_input(uint8_t* buff, size_t numBytes)
{
    static size_t numBytesRead(0);
    esp_err_t status = i2s_read(
            I2S_NUM_0, buff, numBytes, &numBytesRead, portMAX_DELAY
        );
    if ((status != ESP_OK) || (numBytesRead != numBytes))
    {
        std::cerr << "Error reading from I2S; " << numBytesRead;
        std::cerr << " of " << numBytes << " bytes read!" << std::endl;
    }
}

inline void write_to_i2s_output(uint8_t* buff, size_t numBytes)
{
    static size_t numBytesWritten(0);
    esp_err_t status = i2s_write(
            I2S_NUM_0, buff, numBytes, &numBytesWritten, portMAX_DELAY
        );
    if ((status != ESP_OK) || (numBytesWritten != numBytes))
    {
        std::cerr << "Error writing to I2S; " << numBytesWritten;
        std::cerr << " of " << numBytes << " bytes read!" << std::endl;
    }
}