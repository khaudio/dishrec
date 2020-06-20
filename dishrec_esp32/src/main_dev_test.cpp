#include <iostream>
#include "BWFiXML.h"
#include "TimecodeBase.h"

BWFiXML::IXML ixml;
TimecodeBase::Clock tc;

int main()
{
    for (int i(0); i < 4; ++i) std::cout << std::endl;

    tc.set_sample_rate(48000);
    tc.set_framerate(29.97, true);
    tc.set_timecode(00, 04, 07, 17);

    std::cout << "Samples Since Midnight: " << tc.samplesSinceMidnight << std::endl;
    std::cout << tc.str() << std::endl;

    for (int i(0); i < 4; ++i) std::cout << std::endl;

    return 0;
}
