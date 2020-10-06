#include <string>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <memory>
#include "ErrorEnums.h"

#ifndef BEXTCHUNK_H
#define BEXTCHUNK_H

namespace BEXT
{

template <typename T>
T get_str_length(const char* str, bool includeNull = true);

enum bext_coding_history_err
{
    CODING_ALGORITHM_NOT_SET = 80,
    SAMPLE_RATE_NOT_SET = 81,
    BIT_DEPTH_NOT_SET = 82,
    MODE_NOT_SET = 83,
    BIT_RATE_NOT_SET = 84,
    NOT_EXPORTED = 85
};

enum bext_chunk_err
{
    INVALID_UMID_LENGTH = 90,
    DATE_NOT_SET = 91,
    TIME_NOT_SET = 92,
    LOUDNESS_NOT_SET = 93
};

class CodingHistoryRow
{
private:
    bool
        _isMPEG, _algoSet, _samplerateSet, _bitDepthSet,
        _modeSet, _bitrateSet, _textSet;
    bool* _setters[6];
    uint16_t
        _algorithmLength, _samplerateLength, _bitrateLength,
        _bitDepthLength, _modeLength;
    uint16_t _textLength;
    const static uint16_t _textLengthLimit;
    char
        _codingAlgorithm[16], _samplerate[16], _bitDepth[5],
        _mode[16], _bitrate[8], _text[1024], _internalBuffer[1200];
    uint32_t _size;
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
    void copy_to_buffer(char* buff);
    std::string str();
    const char* c_str();
    uint32_t size();
};

class BEXTChunk
{
protected:
    bool
        _dateSet, _timeSet, _timestampSet, _umidSet, _loudnessSet,
        _loudnessValueSet, _loudnessRangeSet, _maxTruePeakLevelSet,
        _maxMomentaryLoudnessSet, _maxShortTermLoudnessSet;
    uint16_t _descriptionLength;
    uint8_t _originatorLength, _originatorReferenceLength;
    uint32_t _size;
    void _autocorrect_bwf_version();
public:
    char
        _bextChunkID[4], description[256],
        originator[32], originatorReference[32],
        originationDate[10], originationTime[8];
    uint32_t timeReferenceLow, timeReferenceHigh;
    uint8_t umid[64];
    uint16_t
        bwfVersion, loudnessValue, loudnessRange,
        maxTruePeakLevel, maxMomentaryLoudness, maxShortTermLoudness;
    uint8_t reserved[180];
    std::string codingHistory;
    BEXTChunk();
    ~BEXTChunk();
    virtual void set_originator(const char* newOriginator);
    virtual void set_originator_reference(const char* newReference);
    virtual void set_description(const char* newDescription);
    virtual void set_date(int16_t year, uint8_t month, uint8_t day);
    virtual void set_date_str(char date[10]);
    virtual void set_time(uint8_t hour, uint8_t minute, uint8_t second);
    virtual void set_time_str(char time[8]);
    virtual void set_timestamp(uint64_t samplesSinceMidnight);
    virtual void set_timestamp(uint32_t low, uint32_t high);
    virtual void set_bwf_version(uint16_t versionNumber);
    virtual void set_umid(const uint8_t* newUmid, uint8_t length);
    virtual void set_loudness_value(uint16_t value);
    virtual void set_loudness_range(uint16_t range);
    virtual void set_loudness_max_true_peak(uint16_t level);
    virtual void set_loudness_max_momentary(uint16_t level);
    virtual void set_loudness_max_short_term(uint16_t value);
    virtual void set_reserved();
    virtual void set_coding_history(CodingHistoryRow row);
    virtual void append_to_coding_history(CodingHistoryRow row);
    virtual uint32_t size();
    virtual size_t total_size();
    virtual void copy_to_buffer(uint8_t* buff);
    const uint8_t* c_str();
};

};

#endif