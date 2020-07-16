#include "I2SIO.h"
#include "driver/gpio.h"
#include "BWFiXML.h"
#include "TimecodeBase.h"

#define REC_STOP_BUTTON         (GPIO_NUM_22)

bool* buttonState = new bool(true);

extern std::shared_ptr<EspSDWavFile> currentFile;

BWFiXML::IXML ixml;
TimecodeBase::Clock tc;

void app_main(void);
void setup();
void loop();
void set_button_gpio();
void check_button_state(void* state);
void write_if_buffered_button(void* state);

void app_main(void)
{
    setup();
    loop();
}

void setup()
{
    std::cout << "Initializing" << std::endl;
    std::cout << "Setting timecode parameters" << std::endl;

    tc.set_sample_rate(currentFile->sampleRate);
    tc.set_framerate(23.976, true);

    std::cout << "Timecode parameters set" << std::endl;

    xTaskCreate(
            &check_button_state,
            "buttonStateChecker",
            10000,
            reinterpret_cast<void*>(buttonState),
            4,
            NULL
        );

    std::cout << "Delaying 500ms..." << std::endl;
    ets_delay_us(500000);
    std::cout << "Opening file" << std::endl;

    open_new_file();

    std::cout << "Delaying 500ms..." << std::endl;
    ets_delay_us(500000);
    std::cout << "Starting I2S" << std::endl;

    I2S::start();

    std::cout << "I2S started" << std::endl;
    std::cout << "Starting card write loop" << std::endl;
    
    xTaskCreatePinnedToCore(
            &write_if_buffered_button,
            "cardWriterFromBuffer",
            8192,
            reinterpret_cast<void*>(buttonState),
            configMAX_PRIORITIES - 1,
            NULL,
            1
        );
    
    std::cout << "Card write loop started" << std::endl;

    std::cout << "Initialized" << std::endl;
}

void loop()
{
    #ifdef _DEBUG
    I2S::check_bytes_read(&inBuffer);
    #endif
    feedLoopWDT();
}

void set_button_gpio()
{
    gpio_config_t ioConf;
    ioConf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
    ioConf.mode = GPIO_MODE_INPUT;
    ioConf.pin_bit_mask = (1ULL << REC_STOP_BUTTON);
    ioConf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&ioConf);
}

void check_button_state(void* state)
{
    bool* button = reinterpret_cast<bool*>(state);
    set_button_gpio();
    while (true)
    {
        vTaskDelay(50);
        *button = gpio_get_level(REC_STOP_BUTTON);
    }
}

void write_if_buffered_button(void* state)
{
    bool* buttonIsNotBeingPressed = reinterpret_cast<bool*>(state);
    while (true)
    {
        if (*buttonIsNotBeingPressed && currentFile->is_open())
        {
            int subBuffersAvailable = inBuffer.sub_buffers_full();
            int written = 0;
            while (subBuffersAvailable > 0)
            {
                // #ifdef _DEBUG
                // std::cout << "Sub-buffers available: " << subBuffersAvailable << "\t";
                // #endif
                write_to_file();
                inBuffer.rotate_read_buffer();
                written++;
                // #ifdef _DEBUG
                // std::cout << "Input Buffer has " << inBuffer.sub_buffers_full();
                // std::cout << " sub-buffers waiting to be written to SD\t";
                // std::cout << "Wrote " << (subBuffersAvailable - inBuffer.sub_buffers_full());
                // std::cout << " sub-buffers already\n" << std::endl;
                // #endif
            }
        }
        else if (!*buttonIsNotBeingPressed && currentFile->is_open())
        {
            #ifdef _DEBUG
            std::cout << "Closing file " << currentFile->filename << std::endl;
            #endif
            currentFile->close();
            ets_delay_us(500000); // 500ms debounce
        }
        else if (!*buttonIsNotBeingPressed && !currentFile->is_open())
        {
            open_new_file();
            ets_delay_us(500000); // 500ms debounce
            #ifdef _DEBUG
            std::cout << "Recording " << currentFile->filename << std::endl;
            #endif
        }
        esp_task_wdt_reset();
    }
}
