#include "BEXTChunk.h"

using namespace BEXT;

const uint16_t CodingHistoryRow::_textLengthLimit = 1024;

template <typename T>
T BEXT::get_str_length(const char* str, bool includeNull)
{
    T i = 0;
    while (str[i] != '\0') ++i;
    return i + includeNull;
}

CodingHistoryRow::CodingHistoryRow() :
_isMPEG(false),
_algoSet(false),
_samplerateSet(false),
_bitDepthSet(false),
_modeSet(false),
_bitrateSet(false),
_textSet(false),
_algorithmLength(0),
_samplerateLength(0),
_bitrateLength(0),
_bitDepthLength(0),
_modeLength(0),
_textLength(0),
_size(0)
{
    this->_setters[0] = &this->_algoSet;
    this->_setters[1] = &this->_samplerateSet;
    this->_setters[2] = &this->_bitDepthSet;
    this->_setters[3] = &this->_modeSet;
    this->_setters[4] = &this->_bitrateSet;
    this->_setters[5] = &this->_textSet;
}

CodingHistoryRow::~CodingHistoryRow()
{
}

void CodingHistoryRow::set_analogue()
{
    this->_isMPEG = false;
    sprintf(this->_codingAlgorithm, "A=ANALOGUE");
    this->_algorithmLength = 10;
    this->_algoSet = true;
}

void CodingHistoryRow::set_pcm()
{
    this->_isMPEG = false;
    sprintf(this->_codingAlgorithm, "A=PCM");
    this->_algorithmLength = 5;
    this->_algoSet = true;
}

void CodingHistoryRow::set_mpeg(uint8_t type, uint8_t layer)
{
    this->_isMPEG = true;
    sprintf(this->_codingAlgorithm, "A=MPEG%uL%u", type, layer);
    this->_algorithmLength = 9;
    this->_algoSet = true;
}

void CodingHistoryRow::set_sample_rate(uint32_t samplerate)
{
    // Set sample rate in Hz
    sprintf(this->_samplerate, "F=%u", samplerate);
    this->_samplerateLength = get_str_length<uint16_t>(this->_samplerate, false);
    this->_samplerateSet = true;
}

void CodingHistoryRow::set_bit_rate(uint32_t bitrate)
{
    // Set bit rate in Kb/s per channel for MPEG encoding
    sprintf(this->_bitrate, "B=%u", bitrate);
    this->_bitrateLength = get_str_length<uint16_t>(this->_bitrate, false);
    this->_bitrateSet = true;
}

void CodingHistoryRow::clear_bit_rate()
{
    strcpy(this->_bitrate, "");
    this->_bitrateLength = 0;
    this->_bitrateSet = false;
}

void CodingHistoryRow::set_bit_depth(uint16_t bitsPerSample)
{
    sprintf(this->_bitDepth, "W=%u", bitsPerSample);
    this->_bitDepthLength = get_str_length<uint16_t>(this->_bitDepth, false);
    this->_bitDepthSet = true;
}

void CodingHistoryRow::set_mono()
{
    sprintf(this->_mode, "M=mono");
    this->_modeLength = 6;
    this->_modeSet = true;
}

void CodingHistoryRow::set_stereo()
{
    sprintf(this->_mode, "M=stereo");
    this->_modeLength = 8;
    this->_modeSet = true;
}

void CodingHistoryRow::set_dual_mono()
{
    sprintf(this->_mode, "M=dual-mono");
    this->_modeLength = 11;
    this->_modeSet = true;
}

void CodingHistoryRow::set_joint_stereo()
{
    sprintf(this->_mode, "M=joint-stereo");
    this->_modeLength = 14;
    this->_modeSet = true;
}

void CodingHistoryRow::set_multi()
{
    sprintf(this->_mode, "M=multi");
    this->_modeLength = 7;
    this->_modeSet = true;
}

void CodingHistoryRow::set_text(const char* text)
{
    uint16_t length = get_str_length<uint16_t>(text, false);
    if (length <= this->_textLengthLimit)
    {
        strncpy(this->_text, text, length);
        this->_textLength = length;
        this->_textSet = true;
    }
}

void CodingHistoryRow::append_text(const char* text)
{
    // Truncate and append text
    uint16_t length = get_str_length<uint16_t>(text, false);
    int16_t remaining = (this->_textLengthLimit - 2) - this->_textLength;
    if (remaining <= 0) return;
    strcat(this->_text, "; ");
    strncat(this->_text, text, remaining);
    this->_textLength += remaining + 2;
    this->_textSet = true;
}

void CodingHistoryRow::clear_text()
{
    strcpy(this->_text, "");
    this->_textLength = 0;
    this->_textSet = false;
}

void CodingHistoryRow::copy_to_buffer(char* buff)
{
    if (!(this->_algoSet)) throw CODING_ALGORITHM_NOT_SET;
    if (!(this->_samplerateSet)) throw SAMPLE_RATE_NOT_SET;
    if (!(this->_bitDepthSet)) throw BIT_DEPTH_NOT_SET;
    if (!(this->_modeSet)) throw MODE_NOT_SET;
    uint32_t index = 0;
    memcpy(buff + index, &(this->_codingAlgorithm), this->_algorithmLength);
    index += this->_algorithmLength;
    memcpy(buff + index++, ",", 1);
    memcpy(buff + index, &(this->_samplerate), this->_samplerateLength);
    index += this->_samplerateLength;
    memcpy(buff + index++, ",", 1);
    if (this->_isMPEG)
    {
        if (!(this->_bitrateSet)) throw BIT_RATE_NOT_SET;
        memcpy(buff + index, &(this->_bitrate), this->_bitrateLength);
        index += this->_bitrateLength;
        memcpy(buff + index++, ",", 1);
    }
    memcpy(buff + index, &(this->_bitDepth), this->_bitDepthLength);
    index += this->_bitDepthLength;
    memcpy(buff + index++, ",", 1);
    memcpy(buff + index, &(this->_mode), this->_modeLength);
    index += this->_modeLength;
    memcpy(buff + index++, ",", 1);
    if (this->_textSet)
    {
        memcpy(buff + index, &(this->_text), this->_textLength);
        index += this->_textLength;
        memcpy(buff + index++, ",", 1);
    }
    buff[index++] = '\n';
}

uint32_t CodingHistoryRow::size()
{
    this->_size = 0;
    this->_size += (
            _algorithmLength
            + _samplerateLength
            + _bitrateLength
            + _bitDepthLength
            + _modeLength
            + _textLength
        );
    // Add one per comma for each included field
    for (bool* setter: this->_setters) this->_size += *setter;
    this->_size += 1; // LF/CR at end
    return this->_size;
}

std::string CodingHistoryRow::str()
{
    copy_to_buffer(this->_internalBuffer);
    return std::string(this->_internalBuffer);
}

const char* CodingHistoryRow::c_str()
{
    copy_to_buffer(this->_internalBuffer);
    return this->_internalBuffer;
}

BEXTChunk::BEXTChunk() :
_dateSet(false),
_timeSet(false),
_timestampSet(false),
_umidSet(false),
_loudnessSet(false),
_loudnessValueSet(false),
_loudnessRangeSet(false),
_maxTruePeakLevelSet(false),
_maxMomentaryLoudnessSet(false),
_maxShortTermLoudnessSet(false),
_descriptionLength(0),
_originatorLength(0),
_originatorReferenceLength(0),
_size(0),
timeReferenceLow(0),
timeReferenceHigh(0),
bwfVersion(0) // Default to BWF version 0
{
    memcpy(this->_bextChunkID, "bext", 4);
    set_reserved();
}

BEXTChunk::~BEXTChunk()
{
}

void BEXTChunk::set_originator(const char* newOriginator)
{
    /* Per EBU TECH 3285:
    ASCII string (maximum 32 characters) containing the name of the
    originator/ producer of the audio file. */
    uint8_t length = get_str_length<uint8_t>(newOriginator, true); // Include null term in length
    this->_originatorLength = ((length < 32) ? length : 32); // Truncate if too long
    strncpy(this->originator, newOriginator, this->_originatorLength);
}

void BEXTChunk::set_originator_reference(const char* newReference)
{
    /* Per EBU TECH 3285:
    ASCII string (maximum 32 characters) containing an unambiguous
    reference allocated by the originating organisation. */
    uint8_t length = get_str_length<uint8_t>(newReference, true); // Include null term in length
    this->_originatorReferenceLength = ((length < 32) ? length : 32); // Truncate if too long
    strncpy(this->originatorReference, newReference, this->_originatorReferenceLength);
    
}

void BEXTChunk::set_description(const char* newDescription)
{
    /* Per EBU TECH 3285:
    ASCII string (maximum 256 characters) containing a free description of
    the sequence. To help applications which display only a short
    description, it is recommended that a resume of the description is
    contained in the first 64 characters and the last 192 characters are used
    for details. */
    uint16_t length = get_str_length<uint16_t>(newDescription, true); // Include null term in length
    this->_descriptionLength = ((length < 256) ? length : 256); // Truncate if too long
    strncpy(this->description, newDescription, this->_descriptionLength);
}

void BEXTChunk::set_date(int16_t year, uint8_t month, uint8_t day)
{
    /* Per EBU TECH 3285:
    10 ASCII characters containing the date of creation of the audio
    sequence. The format shall be « ‘,year’,-,’month,’-‘,day,’»
    with 4 characters for the year and 2 characters per other item. */
    std::ostringstream stream;
    stream << std::setfill('0');
    stream << std::setw(4) << +year << "-";
    stream << std::setw(2) << +month << "-";
    stream << std::setw(2) << +day;
    strncpy(this->originationDate, stream.str().c_str(), 11);
    this->_dateSet = true;
}

void BEXTChunk::set_date_str(char date[10])
{
    // Set date string literal directly
    memcpy(this->originationDate, date, 10);
    this->_dateSet = true;
}

void BEXTChunk::set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    /* Per EBU TECH 3285:
    8 ASCII characters containing the time of creation of the audio sequence.
    The format shall be « ‘hour’-‘minute’-‘second’» with 2 characters per
    item. */
    std::ostringstream stream;
    stream << std::setfill('0');
    stream << std::setw(2) << +hour << ":";
    stream << std::setw(2) << +minute << ":";
    stream << std::setw(2) << +second;
    strncpy(this->originationTime, stream.str().c_str(), 9);
    this->_timeSet = true;
}

void BEXTChunk::set_time_str(char time[8])
{
    // Set time string literal directly
    memcpy(this->originationTime, time, 8);
    this->_timeSet = true;
}

void BEXTChunk::set_timestamp(uint64_t samplesSinceMidnight)
{
    uint32_t* cast = reinterpret_cast<uint32_t*>(&samplesSinceMidnight);
    this->timeReferenceLow = *cast;
    this->timeReferenceHigh = *(cast + 1);
    this->_timestampSet = true;
}

void BEXTChunk::set_timestamp(uint32_t low, uint32_t high)
{
    /* Per EBU TECH 3285:
    These fields shall contain the time-code of the sequence. It is a 64-bit
    value which contains the first sample count since midnight. The number
    of samples per second depends on the sample frequency which is defined
    in the field <nSamplesPerSec> from the <format chunk>. */
    this->timeReferenceLow = low;
    this->timeReferenceHigh = high;
    this->_timestampSet = true;
}

void BEXTChunk::set_bwf_version(uint16_t versionNumber)
{
    /* Per EBU TECH 3285:
    An unsigned binary number giving the version of the BWF. This number is
    particularly relevant for the carriage of the UMID and loudness
    information. For Version 1 it shall be set to 0001h and for Version 2 it
    shall be set to 0002h. */
    this->bwfVersion = versionNumber;
    if ((this->bwfVersion == 1) || (this->bwfVersion == 2)) set_reserved();
}

void BEXTChunk::_autocorrect_bwf_version()
{
    this->_loudnessSet = (
            this->_loudnessValueSet
            && this->_loudnessRangeSet
            && this->_maxTruePeakLevelSet
            && this->_maxMomentaryLoudnessSet
            && this->_maxShortTermLoudnessSet
        );
    if (this->_loudnessSet && this->_umidSet) set_bwf_version(2);
    else if (!this->_loudnessSet && this->_umidSet) set_bwf_version(1);
    else set_bwf_version(0);
}

void BEXTChunk::set_umid(const uint8_t* newUmid, uint8_t length)
{
    /* Per EBU TECH 3285:
    64 bytes containing a UMID (Unique Material Identifier) to standard
    SMPTE 330M [1]. If only a 32 byte "basic UMID" is used, the last 32 bytes
    should be set to zero. (The length of the UMID is given internally.) */
    memcpy(this->umid, newUmid, length);
    if (length > 32) return;
    for (uint8_t i(32); i < 64; ++i)
    {
        this->umid[i] = 0;
    }
    this->_umidSet = true;
    _autocorrect_bwf_version();
}

void BEXTChunk::set_loudness_value(uint16_t value)
{
    this->loudnessValue = value;
    this->_loudnessValueSet = true;
    _autocorrect_bwf_version();
}

void BEXTChunk::set_loudness_range(uint16_t range)
{
    this->loudnessRange = range;
    this->_loudnessRangeSet = true;
    _autocorrect_bwf_version();
}

void BEXTChunk::set_loudness_max_true_peak(uint16_t level)
{
    this->maxTruePeakLevel = level;
    this->_maxTruePeakLevelSet = true;
    _autocorrect_bwf_version();
}

void BEXTChunk::set_loudness_max_momentary(uint16_t level)
{
    this->maxMomentaryLoudness = level;
    this->_maxMomentaryLoudnessSet = true;
    _autocorrect_bwf_version();
}

void BEXTChunk::set_loudness_max_short_term(uint16_t value)
{
    this->maxShortTermLoudness = value;
    this->_maxShortTermLoudnessSet = true;
    _autocorrect_bwf_version();
}

void BEXTChunk::set_reserved()
{
    /* Per EBU TECH 3285:
    180 bytes reserved for extension. If the Version field is set to 0001h or
    0002h, these 180 bytes shall be set to a NULL (zero) value. */
    memset(this->reserved, '\0', 180);
}

void BEXTChunk::set_coding_history(CodingHistoryRow row)
{
    /* Per EBU TECH 3285:
    Unrestricted ASCII characters containing a collection of strings
    terminated by CR/LF. Each string shall contain a description of a coding
    process applied to the audio data. Each new coding application shall add
    a new string with the appropriate information. */
    char buff[row.size()];
    row.copy_to_buffer(buff);
    this->codingHistory = std::string(buff);
}

void BEXTChunk::append_to_coding_history(CodingHistoryRow row)
{
    char buff[row.size()];
    row.copy_to_buffer(buff);
    this->codingHistory.append(std::string(buff));
}

uint32_t BEXTChunk::size()
{
    // Return size of chunk data only (excludes ChunkID and size blocks)
    this->_size = (
            182 // BWF Version + Reserved
            + this->_descriptionLength
            + this->_originatorLength
            + this->_originatorReferenceLength
            + this->codingHistory.size()
        );
    this->_size += (this->_dateSet ? 10 : 0);
    this->_size += (this->_timeSet ? 8 : 0);
    this->_size += (this->_timestampSet ? 8 : 0);
    this->_size += (this->_umidSet ? 64 : 0);
    this->_size += (this->_loudnessSet ? 10 : 0);
    return this->_size;
}

size_t BEXTChunk::total_size()
{
    return size() + 8; // Chunk + ID and Size indicators
}

void BEXTChunk::copy_to_buffer(uint8_t* buff)
{
    if (!this->_dateSet) throw DATE_NOT_SET;
    if (!this->_timeSet) throw TIME_NOT_SET;
    uint32_t index = 0;
    memcpy(buff + index, &(this->_bextChunkID), 4);
    index += 4;
    size();
    memcpy(buff + index, &(this->_size), 4);
    index += 4;
    memcpy(buff + index, this->description, this->_descriptionLength);
    index += this->_descriptionLength;
    memcpy(buff + index, this->originator, this->_originatorLength);
    index += this->_originatorLength;
    memcpy(buff + index, this->originatorReference, this->_originatorReferenceLength);
    index += this->_originatorReferenceLength;
    memcpy(buff + index, this->originationDate, 10);
    index += 10;
    memcpy(buff + index, this->originationTime, 8);
    index += 8;
    if (this->_timestampSet)
    {
        memcpy(buff + index, &this->timeReferenceLow, 4);
        memcpy(buff + index + 4, &this->timeReferenceHigh, 4);
        index += 8;
    }
    memcpy(buff + index, &this->bwfVersion, 2);
    index += 2;
    if (this->_umidSet)
    {
        memcpy(buff + index, &this->umid, 64);
        index += 64;
    }
    if (this->_loudnessSet)
    {
        memcpy(buff + index, &this->loudnessValue, 2);
        index += 2;
        memcpy(buff + index, &this->loudnessRange, 2);
        index += 2;
        memcpy(buff + index, &this->maxTruePeakLevel, 2);
        index += 2;
        memcpy(buff + index, &this->maxMomentaryLoudness, 2);
        index += 2;
        memcpy(buff + index, &this->maxShortTermLoudness, 2);
        index += 2;
    }
    memcpy(buff + index, this->reserved, 180);
    index += 180;
    memcpy(buff + index, this->codingHistory.c_str(), this->codingHistory.size());
    index += this->codingHistory.size();
    // Make sure chunk ends on even-numbered byte
    if (index % 2) buff[index++] = '\n';
    buff[index] = '\0';
}

template int8_t BEXT::get_str_length(const char* str, bool includeNull);
template uint8_t BEXT::get_str_length(const char* str, bool includeNull);
template int16_t BEXT::get_str_length(const char* str, bool includeNull);
template uint16_t BEXT::get_str_length(const char* str, bool includeNull);
template int32_t BEXT::get_str_length(const char* str, bool includeNull);
template uint32_t BEXT::get_str_length(const char* str, bool includeNull);
template int64_t BEXT::get_str_length(const char* str, bool includeNull);
template uint64_t BEXT::get_str_length(const char* str, bool includeNull);