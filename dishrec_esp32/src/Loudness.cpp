#include "Loudness.h"

using namespace Loudness;

Analyzer::Analyzer() :
_stateInitialized(false),
_maxShortTerm(-HUGE_VAL), // LUFS
_maxMomentary(-HUGE_VAL), // LUFS
_maxTruePeak(-HUGE_VAL) // float
{
    this->_ebur128Mode = (
            EBUR128_MODE_M
            | EBUR128_MODE_S
            | EBUR128_MODE_I
            | EBUR128_MODE_LRA
            | EBUR128_MODE_TRUE_PEAK
        );
    // this->_ebur128Mode = EBUR128_MODE_I;
    this->sampleRate = 0;
    this->bitDepth = 0;
    this->numChannels = 0;
}

Analyzer::~Analyzer()
{
}

void Analyzer::_check_err(int returnCode)
{
    if (returnCode == EBUR128_SUCCESS) return;
    else throw returnCode;
}

inline void Analyzer::set_channels(uint16_t channels)
{
    WavMeta::WavFormat::set_channels(channels);
    if (is_format_set()) create_state();
}

inline void Analyzer::set_sample_rate(uint32_t samplerate)
{
    WavMeta::WavFormat::set_sample_rate(samplerate);
    if (is_format_set()) create_state();
}

inline void Analyzer::set_bit_depth(uint16_t bitsPerSample)
{
    WavMeta::WavFormat::set_bit_depth(bitsPerSample);
    if (is_format_set()) create_state();
}

inline void Analyzer::set_format_code(uint16_t formatcode)
{
    WavMeta::WavFormat::set_format_code(formatcode);
    if (is_format_set()) create_state();
}

inline bool Analyzer::is_format_set()
{
    return (
            this->sampleRate
            && this->bitDepth
            && this->numChannels
            && this->formatCode
        );
}

inline void Analyzer::create_state()
{
    if (this->_stateInitialized) return;
    if (!is_format_set()) throw FORMAT_NOT_SET;
    this->_state = ebur128_init(
            this->numChannels,
            this->sampleRate,
            this->_ebur128Mode
        );
    for (int i(0); i < this->numChannels; ++i)
    {
        _check_err(ebur128_set_channel(this->_state, i, (i + 1)));
    }
    this->_stateInitialized = true;
}

void Analyzer::clear()
{
    if (!is_format_set()) throw FORMAT_NOT_SET;
    if (this->_stateInitialized) free(this->_state);
    this->_state = ebur128_init(
            this->numChannels,
            this->sampleRate,
            this->_ebur128Mode
        );
    for (int i(0); i < this->numChannels; ++i)
    {
        _check_err(ebur128_set_channel(this->_state, i, (i + 1)));
    }
    this->_stateInitialized = true;
}

ebur128_state* Analyzer::get_state()
{
    return this->_state;
}

void Analyzer::add_frames(std::vector<int16_t>* interleaved)
{
    _check_err(ebur128_add_frames_short(
            this->_state,
            &(interleaved->at(0)),
            interleaved->size()
        ));
}

void Analyzer::add_frames(std::vector<int>* interleaved)
{
    _check_err(ebur128_add_frames_int(
            this->_state,
            &(interleaved->at(0)),
            interleaved->size()
        ));
}

void Analyzer::add_frames(std::vector<float>* interleaved)
{
    _check_err(ebur128_add_frames_float(
            this->_state,
            &(interleaved->at(0)),
            interleaved->size()
        ));
}

void Analyzer::add_frames(std::vector<double>* interleaved)
{
    _check_err(ebur128_add_frames_double(
            this->_state,
            &(interleaved->at(0)),
            interleaved->size()
        ));
}

double Analyzer::get_loudness_global()
{
    double value;
    _check_err(ebur128_loudness_global(this->_state, &value));
    return value;
}

double Analyzer::get_loudness_range()
{
    double value;
    _check_err(ebur128_loudness_range(this->_state, &value));
    return value;
}

double Analyzer::get_loudness_short_term()
{
    double value;
    _check_err(ebur128_loudness_shortterm(this->_state, &value));
    this->_maxShortTerm = ((value > this->_maxShortTerm) ? value : this->_maxShortTerm);
    return value;
}

double Analyzer::get_loudness_momentary()
{
    double value;
    _check_err(ebur128_loudness_momentary(this->_state, &value));
    this->_maxMomentary = ((value > this->_maxMomentary) ? value : this->_maxMomentary);
    return value;
}

double Analyzer::get_loudness_true_peak()
{
    double value;
    for (int i(0); i < this->numChannels; ++i) // 0 indexed or 1 for channel?
    {
        _check_err(ebur128_true_peak(this->_state, i, &value));
        this->_maxTruePeak = ((value > this->_maxTruePeak) ? value : this->_maxTruePeak);
    }
    return value;
}

