// #include "BWFiXML.h"
// #include "TimecodeBase.h"

// BWFiXML::IXML ixml;
// TimecodeBase::Clock tc;

// int main()
// {
//     for (int i(0); i < 4; ++i) std::cout << std::endl;

//     tc.set_sample_rate(48000);
//     tc.set_framerate(29.97, true);
//     tc.set_timecode(10, 10, 10, 10);

//     std::array<int, 4> offset = {0, 0, 0, 4};

//     std::cout << tc.str() << " + ";
//     std::cout << TimecodeBase::tc_to_string(offset, false) << " = ";
//     std::cout << (tc + offset).str() << std::endl;

//     for (int i(0); i < 4; ++i) std::cout << std::endl;

//     return 0;
// }
