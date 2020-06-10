#include <Wifi.h>
#include "I2SOutput.h"
#include "exampledata.h"

void setup()
{
    std::cout << "Initializing..." << std::endl;

    WiFi.mode(WIFI_OFF);

    configI2S();

    // Example data
    std::cout << "Creating tone vector" << std::endl;
    std::vector<DATATYPE> tone = sine<DATATYPE>(1.0 / 1000.0, 1000.0, f.sampleRate, 0.5);
    for (int i(0); i < buff.ringLength; ++i)
    {
        std::cout << buff._write(tone, true) << std::endl;
        for (int j(0); j < tone.size() - 1; ++j)
        {
            std::cout << std::setw(2) << +j << " ";
            float lineLength((float)tone[j] / (float)maxValue);
            for (int n(0); n < ((lineLength * 80) + 40); ++n)
            {
                std::cout << " ";
            }
            std::cout << ". " << tone[j] << std::endl;
        }
    }
    std::cout << "Tone vector created" << std::endl;

    std::cout << "Delaying 1s..." << std::endl;
    ets_delay_us(1000000);
    std::cout << "Starting i2s write loop" << std::endl;

    starti2sOutputLoop();

    std::cout << "I2s write loop started" << std::endl;
    std::cout << "Initialized" << std::endl;
}

void loop()
{
}
