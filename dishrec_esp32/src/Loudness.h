
#ifndef EBUR128LOUDNESS_H
#define EBUR128LOUDNESS_H

#include <ebur128.h>
#include "WavHeader.h"
#include "AudioUtils.h"

namespace Loudness
{

enum loudness_err
{
    FORMAT_NOT_SET = 130,
    STATES_NOT_INITIALIZED = 131,
    STATE_NOT_FOUND = 132,
};

class Analyzer : virtual public WavMeta::WavFormat
{
protected:
    int _ebur128Mode;
    bool _stateInitialized;
    ebur128_state* _state;
    double
        _maxShortTerm, 
        _maxMomentary,
        _maxTruePeak;

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

    ebur128_state* get_state();

    void add_frames(std::vector<int16_t>* interleaved);
    void add_frames(std::vector<int>* interleaved);
    void add_frames(std::vector<float>* interleaved);
    void add_frames(std::vector<double>* interleaved);

    double get_loudness_global();
    double get_loudness_range();
    double get_loudness_short_term();
    double get_loudness_momentary();
    double get_loudness_true_peak();
    
    /*virtual void set_loudness_value(double value);
    virtual void set_loudness_range(double range);
    virtual void set_loudness_max_true_peak(double level);
    virtual void set_loudness_max_momentary(double level);
    virtual void set_loudness_max_short_term(double value);
    virtual void clear_loudness();*/
    
};

};

#endif
