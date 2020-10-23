#ifndef TIMECODEBASE_H
#define TIMECODEBASE_H

#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "ErrorEnums.h"
#include "WavHeader.h"

namespace TimecodeBase
{

// Framerate strings
#define FRAMERATE_2398              "24000/1001"
#define FRAMERATE_24                "24/1"
#define FRAMERATE_25                "25/1"
#define FRAMERATE_2997              "30000/1001"
#define FRAMERATE_30                "30/1"

#define FRAMERATE_NUM_2398          23.98
#define FRAMERATE_NUM_24            24
#define FRAMERATE_NUM_25            25
#define FRAMERATE_NUM_2997          29.97
#define FRAMERATE_NUM_30            30


class Base;
class Clock;

const char* framerate_str(int fps);
const char* framerate_str(double fps);
double framerate_num(const char* framerateStr);
std::array<int, 4> binary_to_tc(uint32_t data);
uint32_t tc_to_binary(std::array<int, 4> timecode);
void get_tc_string(char* buff, std::array<int, 4> values, bool isDropframe);
std::array<int, 4> string_to_tc(
        const char* formatted,
        bool* isDropframe = nullptr
    );
std::array<int, 4> string_to_tc(
        std::string formatted,
        bool* isDropframe = nullptr
    );
std::string tc_to_string(
        std::array<int, 4> values,
        bool isDropframe
    );
std::array<int, 4> string_to_tc(std::string formatted);

Base operator-(const Base& b);
Base operator+(Base& b1, const Base& b2);
Base operator-(Base& b1, const Base& b2);
Base operator+(Base& b1, const std::array<int, 4>& tc);
Base operator-(Base& b1, const std::array<int, 4>& tc);

class Timestamp
{
public:
    uint64_t samplesSinceMidnight;
    uint32_t *timeReferenceLow, *timeReferenceHigh;
    Timestamp();
    ~Timestamp();
};

class Base
{
protected:
    bool _dropframe, _initialized;
    char _framerate[16];

    int *_hours, *_minutes, *_seconds, *_frames, _dropped;
    std::array<int, 4> _value, _divisors, _maximum;

    virtual void _set_value(std::array<int, 4> vals);
    virtual void _check_initialization();
    virtual bool _drop_frame(std::array<int, 4> tc);
    virtual std::array<int, 4> _increment(
            std::array<int, 4> values,
            std::array<int, 4> offset
        );
    std::array<int, 4> _reconcile(std::array<int, 4> tc);
    std::array<int, 4> _reconcile(int hr, int min, int sec, int frm);
    int _to_frames(int hr, int min, int sec, int frm);
    int _to_frames(std::array<int, 4> tc);
    std::array<int, 4> _from_frames(int numFrames);

public:
    Base();
    ~Base();

    virtual void set_framerate(const char* fps);
    virtual void set_framerate(double fps);
    virtual void set_framerate(int fps);
    
    virtual void set_dropframe(bool isDropframe);
    virtual bool is_dropframe();

    virtual void set_timecode(int hr, int min, int sec, int frm);
    virtual void set_timecode(std::array<int, 4> tc);
    virtual void set_timecode(int numFrames);
    virtual void set_timecode(const char* tc);

    virtual void set_timecode(Base& base);

    virtual void clear_timecode();

    virtual std::array<int, 4> get_timecode();
    virtual int get_frames();
    
    virtual void get_timecode(char* buff);
    virtual std::string str();

    friend Base operator-(const Base& b);
    friend Base operator+(Base& b1, const Base& b2);
    friend Base operator-(Base& b1, const Base& b2);
    friend Base operator+(Base& b1, const std::array<int, 4>& tc);
    friend Base operator-(Base& b1, const std::array<int, 4>& tc);

    friend Clock;
};

class Clock : virtual public Timestamp, public Base, virtual public WavMeta::WavFormat
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
    uint32_t *timeReferenceLow, *timeReferenceHigh;

    Clock();
    ~Clock();
    
    virtual void set_sample_rate(uint32_t samplerate);

    void set_framerate(const char* fps) override;
    void set_framerate(double fps) override;
    void set_framerate(int fps) override;

    void set_dropframe(bool isDropframe) override;
    
    void set_timecode(int hr, int min, int sec, int frm) override;
    void set_timecode(std::array<int, 4> tc) override;
    void set_timecode(int numFrames) override;
    void set_timecode(const char* tc) override;

    virtual void set_timecode(Clock& clock);

    virtual uint64_t get_timestamp();
    virtual void tick();
    
};

};

#endif
