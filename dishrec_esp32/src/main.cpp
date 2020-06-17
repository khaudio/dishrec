#include "I2SInput.h"
#include "WavHeader.h"

#define REC_STOP_BUTTON     (GPIO_NUM_14)

bool* buttonState = new bool(true);

void setup();
void loop();
int main();
void app_main();

void check_button_state(void* pvParameter);

void setup(void)
{
    std::cout << "Starting" << std::endl;

    pinMode(REC_STOP_BUTTON, INPUT_PULLUP);
    xTaskCreate(&check_button_state, "buttonStateChecker", 1024, reinterpret_cast<void*>(buttonState), 6, NULL);

    std::cout << "Delaying 1s..." << std::endl;
    ets_delay_us(1000000);
    std::cout << "Opening file" << std::endl;

    if (!f.open("/arbitrary_filename_001.wav")) exit(1);

    configI2S();

    std::cout << "Delaying 3s..." << std::endl;
    ets_delay_us(3000000);
    std::cout << "Starting i2s read loop" << std::endl;

    starti2sInputLoop();

    std::cout << "I2s read loop started" << std::endl;
    std::cout << "Starting card write loop" << std::endl;
    xTaskCreate(&write_if_buffered_button, "cardWriterFromBuffer", 2048, reinterpret_cast<void*>(buttonState), 7, NULL);
    std::cout << "Card write loop started" << std::endl;
}

void loop()
{
}

int main()
{
    setup();
    while (true)
    {
        loop();
    }
    return 1;
}

void app_main()
{
    main();
}

void check_button_state(void* state)
{
    bool* button = reinterpret_cast<bool*>(state);
    while (true)
    {
        *button = digitalRead(REC_STOP_BUTTON);
        delay(1000);
        std::cout << "Button is " << (*button ? "HIGH" : "LOW") << std::endl;
    }
}
