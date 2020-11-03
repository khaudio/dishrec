
#ifndef EBUR128LOUDNESS_H
#define EBUR128LOUDNESS_H


/* Library is too heavy for ESP32 */
#ifndef EBUR128_H_
#ifndef ESP32
#include <ebur128.h>
#endif
#endif

#include "WavHeader.h"
#include "AudioUtils.h"

namespace Loudness
{

enum loudness_err
{
    FORMAT_NOT_SET = 130,
    STATES_NOT_INITIALIZED = 131
};

class Analyzer : virtual public WavMeta::WavFormat
{
protected:
    int _ebur128Mode;
    bool _stateInitialized;
    
    #ifdef EBUR128_H_
    ebur128_state* _state;
    #endif

    double
        _maxShortTerm,          //      LUFS        -inf - 0.0
        _maxMomentary,          //      LUFS        -inf - 0.0
        _maxTruePeak,           //      LUFS        -inf - 0.0
        _maxTruePeakDecimal;    //      Decimal      0.0 - 1.0

    virtual bool is_format_set();
    virtual void create_state();
    
    virtual void _check_err(int returnCode);

public:
    Analyzer();
    ~Analyzer();

    void set_sample_rate(uint32_t samplerate) override;
    void set_bit_depth(uint16_t bitsPerSample) override;
    void set_channels(uint16_t channels) override;
    void set_format_code(uint16_t formatcode) override;
    void clear();

    #ifdef EBUR128_H_
    ebur128_state* get_state();
    #endif

    void add_frames(std::vector<int16_t>* interleaved);
    void add_frames(std::vector<int>* interleaved);
    void add_frames(std::vector<float>* interleaved);
    void add_frames(std::vector<double>* interleaved);

    double get_loudness_global();
    double get_loudness_range();
    double get_loudness_short_term();
    double get_loudness_momentary();
    double get_loudness_true_peak();
    
};

};

#endif
