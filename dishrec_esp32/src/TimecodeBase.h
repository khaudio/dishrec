#include <array>
#include <cstdint>
#include <iomanip>
#include <string>
#include <sstream>
#include <map>
#include "ErrorEnums.h"

namespace TimecodeBase
{

constexpr double _framerate_2398 = 24000/1001;
constexpr double _framerate_2997 = 30000/1001;
constexpr long double _framerate_2398_ld = 24000/1001;
constexpr long double _framerate_2997_ld = 30000/1001;

std::array<int, 4> binary_to_tc(uint32_t data);
uint32_t tc_to_binary(std::array<int, 4> timecode);
std::string tc_to_string(std::array<int, 4> values, bool isDropframe);
std::array<int, 4> string_to_tc(std::string formatted, bool* isDropframe=nullptr);

class Base
{
protected:
    bool _dropframe, _initialized;
    std::string _framerateString;
    double _framerate;
    int _hours, _minutes, _seconds, _frames;
    int _dropped;
    std::array<int*, 4> _value;
    std::array<int, 4> _divisors, _maximum;
    virtual void _check_initialization();
    virtual bool _drop_frame(std::array<int, 4> tc);
    virtual std::array<int, 4> _increment(std::array<int, 4> values, std::array<int, 4> offset);
    virtual int _to_frames(int hr, int min, int sec, int frm);
    virtual std::array<int, 4> _from_frames(int numFrames);
public:
    Base();
    ~Base();
    virtual void set_framerate(double fps, bool isDropframe);
    virtual void set_framerate(int fps, bool isDropframe);
    virtual void set_timecode(int hr, int min, int sec, int frm);
    virtual void set_timecode(std::array<int, 4> tc);
    virtual void set_timecode(int numFrames);
    virtual std::array<int, 4> get_timecode();
    virtual int get_frames();
    virtual std::string str();
};

class Clock : public Base
{
protected:
    uint32_t _samplesPerFrame;
    void _check_initialization() override;
    void _set_samples_per_frame();
    void _set_samples_since_midnight();
    void _set_samples_since_midnight(uint64_t numSamples);
public:
    uint64_t samplesSinceMidnight;
    uint32_t sampleRate;
    Clock();
    ~Clock();
    void set_framerate(double fps, bool isDropframe) override;
    void set_framerate(int fps, bool isDropframe) override;
    void set_timecode(int hr, int min, int sec, int frm) override;
    virtual void set_sample_rate(uint32_t samplerate);
};

};
