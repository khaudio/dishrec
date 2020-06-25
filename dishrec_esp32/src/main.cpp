#include "I2SIO.h"
// #include "BWFiXML.h"
// #include "TimecodeBase.h"

#define REC_STOP_BUTTON     (GPIO_NUM_14)

bool* buttonState = new bool(true);

// BWFiXML::IXML ixml;
// TimecodeBase::Clock tc;

void setup();
void loop();
void check_button_state(void* state);

void setup(void)
{
    std::cout << "Initializing" << std::endl;

    pinMode(REC_STOP_BUTTON, INPUT_PULLUP);
    xTaskCreate(
            &check_button_state,
            "buttonStateChecker",
            1024,
            reinterpret_cast<void*>(buttonState),
            6,
            NULL
        );

    std::cout << "Delaying 500ms..." << std::endl;
    ets_delay_us(500000);
    std::cout << "Opening file" << std::endl;

    if (!f.open("/arbitrary_filename_001.wav")) exit(1);

    std::cout << "Delaying 500ms..." << std::endl;
    ets_delay_us(500000);
    std::cout << "Starting I2S" << std::endl;

    I2S::start();

    std::cout << "I2S started" << std::endl;
    std::cout << "Starting card write loop" << std::endl;
    
    xTaskCreate(
            &write_if_buffered_button,
            "cardWriterFromBuffer",
            2048,
            reinterpret_cast<void*>(buttonState),
            7,
            NULL
        );
    std::cout << "Card write loop started" << std::endl;

    std::cout << "Initialized" << std::endl;
}

void loop()
{
}

void check_button_state(void* state)
{
    bool* button = reinterpret_cast<bool*>(state);
    while (true)
    {
        *button = digitalRead(REC_STOP_BUTTON);
        ets_delay_us(100000);
        #ifdef _DEBUG
        std::cout << "Button is " << (*button ? "HIGH" : "LOW") << std::endl;
        #endif
    }
}
