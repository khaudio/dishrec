#include "TimecodeBase.h"

using namespace TimecodeBase;

std::array<int, 4> TimecodeBase::binary_to_tc(uint32_t data)
{
    std::array<int, 4> timecode;
    uint8_t* ptr(reinterpret_cast<uint8_t*>(&data));
    for (uint8_t i(0); i < 4; ++i)
    {
        timecode[i] = *(ptr + i);
    }
    return timecode;
}

uint32_t TimecodeBase::tc_to_binary(std::array<int, 4> timecode)
{
    uint32_t data;
    uint8_t* ptr(reinterpret_cast<uint8_t*>(&data));
    for (uint8_t i(0); i < 4; ++i)
    {
        *(ptr + i) = timecode[i];
    }
    return data;
}

std::string TimecodeBase::tc_to_string(std::array<int, 4> values, bool isDropframe)
{
    std::ostringstream stream;
    stream << std::setfill('0') << std::setw(2);
    stream << +values[0] << ":";
    stream << std::setfill('0') << std::setw(2);
    stream << +values[1] << ":";
    stream << std::setfill('0') << std::setw(2);
    stream << +values[2] << (isDropframe ? ";" : ":");
    stream << std::setfill('0') << std::setw(2);
    stream << +values[3];
    return stream.str();
}

std::array<int, 4> TimecodeBase::string_to_tc(std::string formatted, bool* isDropframe)
{
    #ifdef _DEBUG
    if (formatted.size() != 11)
    {
        throw INVALID_TC_STRING_LENGTH;
    }
    #endif
    std::array<int, 4> timecode = {
            std::stoi(formatted.substr(0, 2)),
            std::stoi(formatted.substr(3, 2)),
            std::stoi(formatted.substr(6, 2)),
            std::stoi(formatted.substr(9, 2)),
        };
    if (isDropframe != nullptr)
    {
        *isDropframe = formatted.substr(8, 1) == ";";
    }
    return timecode;
}

Base::Base() :
_initialized(false),
_dropped(0)
{
    this->_value = {0, 0, 0, 0};
    this->_hours = &(this->_value[0]);
    this->_minutes = &(this->_value[1]);
    this->_seconds = &(this->_value[2]);
    this->_frames = &(this->_value[3]);
    this->_divisors[0] = 24;
    this->_divisors[1] = 60;
    this->_divisors[2] = 60;
}

Base::~Base()
{
}

void Base::_set_value(std::array<int, 4> tc)
{
    for (uint8_t i(0); i < 4; ++i)
    {
        this->_value[i] = tc[i];
    }
}

void Base::_check_initialization()
{
    if (!this->_initialized)
    {
        throw TIMEBASE_UNINITIALIZED;
    }
}

bool Base::_drop_frame(std::array<int, 4> tc)
{
    return (
            this->_dropframe
            && (!tc[3] || (tc[3] == 1))
            && (!tc[2] || (tc[2] == 60))
            && (tc[1] % 10)
        );
}

std::array<int, 4> Base::_increment(std::array<int, 4> values, std::array<int, 4> offset)
{
    std::array<int, 4> output(values);
    for (int8_t i(3); i >= 0; --i)
    {
        output[i] += offset[i];
        while (output[i] > this->_maximum[i])
        {
            if (i > 0)
            {
                output[i - 1] += 1;
                if ((i == 3) && _drop_frame(output))
                {
                    output[i] += 2;
                }
            }
            output[i] -= this->_divisors[i];
        }
        while (output[i] < 0)
        {
            if (i > 0)
            {
                output[i - 1] -= 1;
                if ((i == 3) && _drop_frame(output))
                {
                    output[i] -= 2;
                }
            }
        }
    }
    return output;
}

std::array<int, 4> Base::_reconcile(std::array<int, 4> tc)
{
    static std::array<int, 4> _zeroTC = {0, 0, 0, 0};
    return _increment(_zeroTC, tc);
}

std::array<int, 4> Base::_reconcile(int hr, int min, int sec, int frm)
{
    std::array<int, 4> tc = {hr, min, sec, frm};
    return _reconcile(tc);
}

int Base::_to_frames(int hr, int min, int sec, int frm)
{
    int accumulated(hr * this->_divisors[1]);
    accumulated += min;
    accumulated *= this->_divisors[2];
    accumulated += sec;
    accumulated *= this->_divisors[3];
    accumulated += frm;
    return accumulated;
}

int Base::_to_frames(std::array<int, 4> tc)
{
    return _to_frames(tc[0], tc[1], tc[2], tc[3]);
}

std::array<int, 4> Base::_from_frames(int numFrames)
{
    std::array<int, 4> offset = {0, 0, 0, numFrames};
    return _reconcile(offset);
}

void Base::set_framerate(double fps, bool isDropframe)
{
    this->_dropframe = isDropframe;
    if ((fps == 23.98) || (fps == 23.976))
    {
        this->_framerateString = std::string("24000/1001");
        this->_framerate = _framerate_2398;
        this->_divisors[3] = 24;
    }
    else if (fps == 29.97)
    {
        this->_framerateString = std::string("30000/1001");
        this->_framerate = _framerate_2997;
        this->_divisors[3] = 30;
    }
    else
    {
        throw INVALID_FRAMERATE;
    }
    for (uint8_t i(0); i < 4; ++i)
    {
        this->_maximum[i] = this->_divisors[i] - 1;
    }
    this->_initialized = true;
}

void Base::set_framerate(int fps, bool isDropframe)
{
    this->_dropframe = isDropframe;
    switch (fps)
    {
        case (24):
            this->_framerateString = std::string("24/1");
            this->_framerate = 24;
            this->_divisors[3] = 24;
            break;
        case (25):
            this->_framerateString = std::string("25/1");
            this->_framerate = 25;
            this->_divisors[3] = 25;
            break;
        case (30):
            this->_framerateString = std::string("30/1");
            this->_framerate = 30;
            this->_divisors[3] = 30;
            break;
        default:
            throw INVALID_FRAMERATE;
    }
    for (uint8_t i(0); i < 4; ++i)
    {
        this->_maximum[i] = this->_divisors[i] - 1;
    }
    this->_initialized = true;
}

void Base::set_timecode(int hr, int min, int sec, int frm)
{
    #ifdef _DEBUG
    _check_initialization();
    #endif
    _set_value(_reconcile(hr, min, sec, frm));
}

void Base::set_timecode(std::array<int, 4> tc)
{
    set_timecode(tc[0], tc[1], tc[2], tc[3]);
}

void Base::set_timecode(int numFrames)
{
    std::array<int, 4> tc = _from_frames(numFrames);
    set_timecode(tc[0], tc[1], tc[2], tc[3]);
}

std::array<int, 4> Base::get_timecode()
{
    #ifdef _DEBUG
    _check_initialization();
    #endif
    std::array<int, 4> output;
    for (uint8_t i(0); i < 4; ++i)
    {
        output[i] = this->_value[i];
    }
    return output;
}

int Base::get_frames()
{
    return _to_frames(
            *(this->_hours),
            *(this->_minutes),
            *(this->_seconds),
            *(this->_frames)
        );
}

std::string Base::str()
{
    return tc_to_string(get_timecode(), this->_dropframe);
}


Base TimecodeBase::operator-(const Base& b)
{
    Base output(b);
    for (uint8_t i(0); i < 4; ++i)
    {
        output._value[i] *= -1;
    }
    return output;
}

Base TimecodeBase::operator+(Base& b1, const Base& b2)
{
    Base output(b1);
    std::array<int, 4> sum = output._increment(b1._value, b2._value);
    output.set_timecode(sum);
    return output;
}

Base TimecodeBase::operator-(Base& b1, const Base& b2)
{
    Base output(b1);
    std::array<int, 4> sum = output._increment(b1._value, (-b2)._value);
    output.set_timecode(sum);
    return output;
}

Base TimecodeBase::operator+(Base& b1, const std::array<int, 4>& tc)
{
    Base output(b1);
    std::array<int, 4> sum = output._increment(b1._value, tc);
    output.set_timecode(sum);
    return output;
}

Base TimecodeBase::operator-(Base& b1, const std::array<int, 4>& tc)
{
    Base output(b1);
    std::array<int, 4> negative(tc);
    for (uint8_t i(0); i < 4; ++i)
    {
        negative[i] *= -1;
    }
    std::array<int, 4> sum = output._increment(b1._value, negative);
    output.set_timecode(sum);
    return output;
}

Clock::Clock() :
Base(),
sampleRate(0)
{
}

Clock::~Clock()
{
}

void Clock::_set_samples_since_midnight()
{
    // Calculate samples since midnight from current timecode
    this->samplesSinceMidnight = get_frames() * this->_samplesPerFrame;
}

void Clock::_set_samples_since_midnight(uint64_t numSamples)
{
    // Modify current timecode to reflect numSamples
    this->samplesSinceMidnight = numSamples;
    Base::set_timecode(numSamples / this->_samplesPerFrame);
}

void Clock::_set_samples_per_frame()
{
    this->_samplesPerFrame = this->sampleRate / this->_maximum[3];
}

void Clock::set_framerate(double fps, bool isDropframe)
{
    Base::set_framerate(fps, isDropframe);
    if (this->sampleRate)
    {
        _set_samples_per_frame();
    }
}

void Clock::set_framerate(int fps, bool isDropframe)
{
    Base::set_framerate(fps, isDropframe);
    if (this->sampleRate)
    {
        _set_samples_per_frame();
    }
}

void Clock::_check_initialization()
{
    Base::_check_initialization();
    if (!this->sampleRate)
    {
        throw SAMPLERATE_UNINITIALIZED;
    }
}

void Clock::set_timecode(int hr, int min, int sec, int frm)
{
    Base::set_timecode(hr, min, sec, frm);
    _set_samples_since_midnight();
}

void Clock::set_timecode(std::array<int, 4> tc)
{
    set_timecode(tc[0], tc[1], tc[2], tc[3]);
}

void Clock::set_timecode(int numFrames)
{
    Base::set_timecode(numFrames);
    _set_samples_since_midnight();
}

void Clock::set_sample_rate(uint32_t samplerate)
{
    this->sampleRate = samplerate;
    if (this->_initialized)
    {
        _set_samples_per_frame();
    }
}
