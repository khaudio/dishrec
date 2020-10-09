#ifndef TIMECODEBASE_H
#define TIMECODEBASE_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
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
    int *_hours, *_minutes, *_seconds, *_frames, _dropped;
    std::array<int, 4> _value, _divisors, _maximum;
    virtual void _set_value(std::array<int, 4> vals);
    virtual void _check_initialization();
    virtual bool _drop_frame(std::array<int, 4> tc);
    virtual std::array<int, 4> _increment(std::array<int, 4> values, std::array<int, 4> offset);
    std::array<int, 4> _reconcile(std::array<int, 4> tc);
    std::array<int, 4> _reconcile(int hr, int min, int sec, int frm);
    int _to_frames(int hr, int min, int sec, int frm);
    int _to_frames(std::array<int, 4> tc);
    std::array<int, 4> _from_frames(int numFrames);
public:
    Base();
    ~Base();
    virtual void set_framerate(double fps, bool isDropframe);
    virtual void set_framerate(int fps, bool isDropframe);
    virtual void set_timecode(int hr, int min, int sec, int frm);
    virtual void set_timecode(std::array<int, 4> tc);
    virtual void set_timecode(int numFrames);
    virtual void clear_timecode();
    virtual std::array<int, 4> get_timecode();
    virtual int get_frames();
    virtual std::string str();
    friend Base operator-(const Base& b);
    friend Base operator+(Base& b1, const Base& b2);
    friend Base operator-(Base& b1, const Base& b2);
    friend Base operator+(Base& b1, const std::array<int, 4>& tc);
    friend Base operator-(Base& b1, const std::array<int, 4>& tc);
};

class Clock : public Base
{
protected:
    uint32_t _samplesPerFrame;
    void _check_initialization() override;
    void _set_samples_per_frame();
    virtual void _set_timestamp();
    virtual void _set_timestamp(uint64_t numSamples);
    virtual void _set_timestamp(uint32_t ssmLo, uint32_t ssmHi);
public:
    uint64_t samplesSinceMidnight;
    uint32_t *timestampSSMLo, *timestampSSMHi;
    uint32_t sampleRate;
    Clock();
    ~Clock();
    void set_framerate(double fps, bool isDropframe) override;
    void set_framerate(int fps, bool isDropframe) override;
    void set_timecode(int hr, int min, int sec, int frm) override;
    void set_timecode(std::array<int, 4> tc) override;
    void set_timecode(int numFrames) override;
    void tick();
    virtual void set_sample_rate(uint32_t samplerate);
};

};

#endif
