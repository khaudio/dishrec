#include <string>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <memory>

#include "TimecodeBase.h"
#include "WavHeader.h"
#include "AudioUtils.h"
#include "ErrorEnums.h"

#include <iostream>

#ifndef BEXTCHUNK_H
#define BEXTCHUNK_H

#define BWFVERSION                  2

namespace BEXT
{

class CodingHistoryRow;
class BEXTChunk;

enum bext_coding_history_err
{
    CODING_ALGORITHM_NOT_SET = 80,
    SAMPLE_RATE_NOT_SET = 81,
    BIT_DEPTH_NOT_SET = 82,
    MODE_NOT_SET = 83,
    BIT_RATE_NOT_SET = 84
};

class CodingHistoryRow
{
private:
    bool
        _isMPEG, _algoSet, _samplerateSet, _bitDepthSet,
        _modeSet, _bitrateSet, _textSet, *_setters[6];
    uint16_t
        _algorithmLength, _samplerateLength, _bitrateLength,
        _bitDepthLength, _modeLength, _textLength;
    const static uint16_t _textLengthLimit;
    char
        _codingAlgorithm[16], _samplerate[16], _bitDepth[5],
        _mode[16], _bitrate[8], _text[1024], _internalBuffer[1200];
    uint32_t _rowSize;

public:
    CodingHistoryRow();
    ~CodingHistoryRow();
    void set_analogue();
    void set_pcm();
    void set_mpeg(uint8_t type, uint8_t layer);
    void set_sample_rate(uint32_t samplerate);
    void set_bit_rate(uint32_t bitrate);
    void clear_bit_rate();
    void set_bit_depth(uint16_t bitsPerSample);
    void set_mono();
    void set_stereo();
    void set_dual_mono();
    void set_joint_stereo();
    void set_multi();
    void set_text(const char* text);
    void append_text(const char* text);
    void clear_text();
    void get(char* buff);

    uint32_t size();

    std::string str();
    const char* c_str();
};

class BEXTChunk : virtual public TimecodeBase::Timestamp, public WavMeta::Chunk
{
public:
    BEXTChunk();
    ~BEXTChunk();

    virtual void clear();

    // Chunk size minus ID and size fields
    uint32_t size() override;

    // Size of entire chunk including ID and Size
    size_t total_size() override;

    // Export chunk including ID and size fields
    virtual size_t get(uint8_t* buff);

    // Import chunk
    virtual size_t set(const uint8_t* data);

/*                            Originator                            */

    char originator[33];
    virtual void set_originator(const char* newOriginator);
    virtual void clear_originator();

/*                       Originator Reference                       */

    char originatorReference[33];
    virtual void set_originator_reference(const char* newReference);
    virtual void clear_originator_reference();

/*                           Description                            */

    char description[257];
    virtual void set_description(const char* newDescription);
    virtual void clear_description();

/*                              Date                                */

    char originationDate[11];
    virtual void set_date(int16_t year, uint8_t month, uint8_t day);
    virtual void set_date_str(const char* date);
    virtual void clear_date();

/*                              Time                                */

    char originationTime[9];
    virtual void set_time(uint8_t hour, uint8_t minute, uint8_t second);
    virtual void set_time_str(const char* time);
    virtual void clear_time();

/*                            Timestamp                             */

    virtual void set_timestamp(uint64_t samplesSinceMidnight);
    virtual void set_timestamp(uint32_t low, uint32_t high);
    virtual void clear_timestamp();

/*                           BWF Version                            */

protected:
    void _autoset_bwf_version();

public:
    uint16_t bwfVersion;
    virtual void set_bwf_version(uint16_t versionNumber);

/*                              UMID                                */

    bool _umidSet;
    uint8_t umid[64];
    virtual void set_umid(const uint8_t* newUmid);
    virtual void clear_umid();

/*                            Loudness                              */

protected:
    virtual int _convert_loudness_to_int(double value);

public:
    int16_t
        loudnessValue, loudnessRange,
        maxTruePeakLevel, maxMomentaryLoudness, maxShortTermLoudness;
    virtual void set_loudness_value(double value);
    virtual void set_loudness_range(double range);
    virtual void set_loudness_max_true_peak(double level);
    virtual void set_loudness_max_momentary(double level);
    virtual void set_loudness_max_short_term(double value);
    virtual void clear_loudness();
    virtual bool loudness_is_set();

/*                            Reserved                              */

    uint8_t reserved[180];
    virtual void set_reserved();

/*                         Coding History                           */

    std::string codingHistory;
    virtual void set_coding_history(std::string history);
    virtual void set_coding_history(const char* history);
    virtual void set_coding_history(CodingHistoryRow row);
    virtual void append_to_coding_history(CodingHistoryRow row);
    virtual void clear_coding_history();
};

};

#endif
