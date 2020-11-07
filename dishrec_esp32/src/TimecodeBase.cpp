#include "TimecodeBase.h"

using namespace TimecodeBase;

Timestamp::Timestamp() :
samplesSinceMidnight(0)
{
    this->timeReferenceLow = reinterpret_cast<uint32_t*>(&(this->samplesSinceMidnight));
    this->timeReferenceHigh = this->timeReferenceLow + 1;
}

Timestamp::~Timestamp()
{
}

const char* TimecodeBase::framerate_str(int fps)
{
    switch (fps)
    {
        case (24):
            return framerate24;
        case (25):
            return framerate25;
        case (30):
            return framerate30;
        default:
            throw INVALID_FRAMERATE;
    }
}

const char* TimecodeBase::framerate_str(double fps)
{
    if ((fps == 23.98) || (fps == 23.976)) return framerate2398;
    else if (fps == 29.97) return framerate2997;
    else throw INVALID_FRAMERATE;
}

double TimecodeBase::framerate_num(const char* framerateStr)
{
    if (!(strcmp(framerateStr, framerate2398))) return framerateNum2398;
    else if (!(strcmp(framerateStr, framerate24))) return framerateNum24;
    else if (!(strcmp(framerateStr, framerate25))) return framerateNum25;
    else if (!(strcmp(framerateStr, framerate2997))) return framerateNum2997;
    else if (!(strcmp(framerateStr, framerate30))) return framerateNum30;
    else throw INVALID_FRAMERATE;
}

std::array<int, 4> TimecodeBase::binary_to_tc(uint32_t data)
{
    std::array<int, 4> timecode;
    uint8_t* ptr(reinterpret_cast<uint8_t*>(&data));
    for (uint8_t i(0); i < 4; ++i) timecode[i] = *(ptr + i);
    return timecode;
}

uint32_t TimecodeBase::tc_to_binary(std::array<int, 4> timecode)
{
    uint32_t data;
    uint8_t* ptr(reinterpret_cast<uint8_t*>(&data));
    for (uint8_t i(0); i < 4; ++i) *(ptr + i) = timecode[i];
    return data;
}

void TimecodeBase::get_tc_string(char* buff, std::array<int, 4> values, bool isDropframe)
{
    sprintf(
            buff,
            "%02u:%02u:%02u%c%02u",
            values[0],
            values[1],
            values[2],
            (isDropframe ? ';' : ':'),
            values[3]
        );
    buff[11] = '\0';
}

std::string TimecodeBase::tc_to_string(std::array<int, 4> values, bool isDropframe)
{
    char buff[16];
    get_tc_string(buff, values, isDropframe);
    return std::string(buff);
}

std::array<int, 4> TimecodeBase::string_to_tc(const char* formatted, bool* isDropframe)
{
    if (isDropframe != nullptr) *isDropframe = (formatted[8] == ';');
    std::array<int, 4> timecode;
    const char hrStr[3] = {formatted[0], formatted[1], '\0'};
    const char minStr[3] = {formatted[3], formatted[4], '\0'};
    const char secStr[3] = {formatted[6], formatted[7], '\0'};
    const char frmStr[3] = {formatted[9], formatted[10], '\0'};
    timecode[0] = atoi(hrStr);
    timecode[1] = atoi(minStr);
    timecode[2] = atoi(secStr);
    timecode[3] = atoi(frmStr);
    return timecode;
}

std::array<int, 4> TimecodeBase::string_to_tc(std::string formatted, bool* isDropframe)
{
    #ifdef _DEBUG
    if (formatted.size() != 11) throw INVALID_TC_STRING_LENGTH;
    #endif
    if (isDropframe != nullptr) *isDropframe = formatted.substr(8, 1) == ";";
    std::array<int, 4> timecode = {
            std::stoi(formatted.substr(0, 2)),
            std::stoi(formatted.substr(3, 2)),
            std::stoi(formatted.substr(6, 2)),
            std::stoi(formatted.substr(9, 2)),
        };
    return timecode;
}

uint32_t TimecodeBase::get_overcrank_rate(uint32_t samplerate)
{
    switch (samplerate)
    {
        case (48000):
            return overcrank48000;
        case (96000):
            return overcrank96000;
        case (192000):
            return overcrank192000;
        default:
            return std::round(samplerate * overcrankMultiplier);
    }
}

uint32_t TimecodeBase::get_undercrank_rate(uint32_t samplerate)
{
    /* Return standard sample rate from +0.1% rate
    Explicitly calls std::floor to ensure truncation */
    switch (samplerate)
    {
        case (overcrank48000):
            return 48000;
        case (overcrank96000):
            return 96000;
        case (overcrank192000):
            return 192000;
        default:
            return std::floor((samplerate * 1000) / 1001);
    }
}

bool TimecodeBase::is_overcrank_rate(uint32_t samplerate)
{
    /* Checks if sample rate is 0.1% overcranked
    Compatible with sample rates > 1MHz; e.g., DSD */
    return (samplerate != (
            (samplerate < 1000000) ?
            (std::floor(samplerate / 1000) * 1000)
            : (std::floor(samplerate / 1000000) * 1000000)
        ));
}

const char* TimecodeBase::get_overcrank_framerate(const char* framerateStr)
{
    if (!std::strcmp(framerateStr, framerate2398)) return framerate24;
    else if (!std::strcmp(framerateStr, framerate2997)) return framerate30;
    else throw INVALID_FRAMERATE;
}

const char* TimecodeBase::get_undercrank_framerate(const char* framerateStr)
{
    if (!std::strcmp(framerateStr, framerate24)) return framerate2398;
    else if (!std::strcmp(framerateStr, framerate30)) return framerate2997;
    else throw INVALID_FRAMERATE;
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

inline void Base::_set_value(std::array<int, 4> tc)
{
    for (uint8_t i(0); i < 4; ++i) this->_value[i] = tc[i];
}

inline void Base::_check_initialization()
{
    if (!this->_initialized) throw TIMEBASE_UNINITIALIZED;
}

inline bool Base::_drop_frame(std::array<int, 4> tc)
{
    return (
            this->_dropframe
            && (!tc[3] || (tc[3] == 1))
            && (!tc[2] || (tc[2] == 60))
            && (tc[1] % 10)
        );
}

inline std::array<int, 4> Base::_increment(std::array<int, 4> values, std::array<int, 4> offset)
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

inline std::array<int, 4> Base::_reconcile(std::array<int, 4> tc)
{
    static std::array<int, 4> _zeroTC = {0, 0, 0, 0};
    return _increment(_zeroTC, tc);
}

inline std::array<int, 4> Base::_reconcile(int hr, int min, int sec, int frm)
{
    std::array<int, 4> tc = {hr, min, sec, frm};
    return _reconcile(tc);
}

inline int Base::_to_frames(int hr, int min, int sec, int frm)
{
    int accumulated(hr * this->_divisors[1]);
    accumulated += min;
    accumulated *= this->_divisors[2];
    accumulated += sec;
    accumulated *= this->_divisors[3];
    accumulated += frm;
    return accumulated;
}

inline int Base::_to_frames(std::array<int, 4> tc)
{
    return _to_frames(tc[0], tc[1], tc[2], tc[3]);
}

inline std::array<int, 4> Base::_from_frames(int numFrames)
{
    std::array<int, 4> offset = {0, 0, 0, numFrames};
    return _reconcile(offset);
}

void Base::set_framerate(const char* fps)
{
    strncpy(this->_framerate, fps, 16);
}

void Base::set_framerate(double fps)
{
    if ((fps == 23.98) || (fps == 23.976))
    {
        strncpy(this->_framerate, framerate2398, 11);
        this->_divisors[3] = 24;
    }
    else if (fps == 29.97)
    {
        strncpy(this->_framerate, framerate2997, 11);
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

void Base::set_framerate(int fps)
{
    switch (fps)
    {
        case (24):
            strncpy(this->_framerate, framerate24, 5);
            this->_divisors[3] = 24;
            break;
        case (25):
            strncpy(this->_framerate, framerate25, 5);
            this->_divisors[3] = 25;
            break;
        case (30):
            strncpy(this->_framerate, framerate30, 5);
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

void Base::set_dropframe(bool isDropframe)
{
    this->_dropframe = isDropframe;
}

bool Base::is_dropframe()
{
    return this->_dropframe;
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
    set_timecode(0, 0, 0, numFrames);
    // std::array<int, 4> tc = _from_frames(numFrames);
    // set_timecode(tc[0], tc[1], tc[2], tc[3]);
}

void Base::set_timecode(const char* timecodeStr)
{
    std::array<int, 4> tc = string_to_tc(timecodeStr, &this->_dropframe);
    set_timecode(tc);
}

void Base::clear_timecode()
{
    set_timecode(0, 0, 0, 0);
}

void Base::set_timecode(Base& base)
{
    set_framerate(base._framerate);
    set_dropframe(base._dropframe);
    set_timecode(base._value);
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

inline int Base::get_frames()
{
    return _to_frames(
            *(this->_hours),
            *(this->_minutes),
            *(this->_seconds),
            *(this->_frames)
        );
}

void Base::get_timecode(char* buff)
{
    get_tc_string(buff, this->_value, this->_dropframe);
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
Timestamp(),
Base()
{
    this->sampleRate = 0;
    this->samplesSinceMidnight = 0;
    this->timeReferenceLow = reinterpret_cast<uint32_t*>(&(this->samplesSinceMidnight));
    this->timeReferenceHigh = this->timeReferenceLow + 1;
}

Clock::~Clock()
{
}

inline void Clock::_check_initialization()
{
    Base::_check_initialization();
    if (!this->sampleRate) throw SAMPLERATE_UNINITIALIZED;
}

inline void Clock::_set_timestamp()
{
    // Calculate samples since midnight from current timecode
    this->samplesSinceMidnight = get_frames() * this->_samplesPerFrame;
}

inline void Clock::_set_timestamp(uint64_t numSamples)
{
    // Modify current timecode to reflect numSamples
    this->samplesSinceMidnight = numSamples;
    Base::set_timecode(numSamples / this->_samplesPerFrame);
}

inline void Clock::_set_timestamp(uint32_t ssmLo, uint32_t ssmHi)
{
    *this->timeReferenceLow = ssmLo;
    *this->timeReferenceHigh = ssmHi;
}

inline void Clock::_set_samples_per_frame()
{
    this->_samplesPerFrame = this->sampleRate / this->_divisors[3];
}

void Clock::set_framerate(const char* fps)
{
    Base::set_framerate(fps);
    if (this->sampleRate) _set_samples_per_frame();
}


void Clock::set_framerate(double fps)
{
    Base::set_framerate(fps);
    if (this->sampleRate) _set_samples_per_frame();
}

void Clock::set_framerate(int fps)
{
    Base::set_framerate(fps);
    if (this->sampleRate) _set_samples_per_frame();
}

void Clock::set_dropframe(bool isDropframe)
{
    Base::set_dropframe(isDropframe);
}

void Clock::set_timecode(int hr, int min, int sec, int frm)
{
    Base::set_timecode(hr, min, sec, frm);
    _set_timestamp();
}

void Clock::set_timecode(std::array<int, 4> tc)
{
    set_timecode(tc[0], tc[1], tc[2], tc[3]);
}

void Clock::set_timecode(int numFrames)
{
    Base::set_timecode(numFrames);
    this->samplesSinceMidnight = numFrames * this->_samplesPerFrame;
}

void Clock::set_timecode(const char* timecodeStr)
{
    Base::set_timecode(timecodeStr);
    _set_timestamp();
}

void Clock::set_timecode(Clock& clock)
{
    this->sampleRate = clock.sampleRate;
    Base::set_timecode(clock);
}

uint64_t Clock::get_timestamp()
{
    return this->samplesSinceMidnight;
}

void Clock::tick()
{
    int numFrames = get_frames() + 1;
    set_timecode(numFrames);
}

void Clock::set_sample_rate(uint32_t samplerate)
{
    WavMeta::WavFormat::set_sample_rate(samplerate);
    if (this->_initialized) _set_samples_per_frame();
}
