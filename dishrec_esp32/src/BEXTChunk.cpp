#include "BEXTChunk.h"

using namespace BEXT;

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
_rowSize(0)
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
    this->_samplerateLength = strlen(this->_samplerate);
    this->_samplerateSet = true;
}

void CodingHistoryRow::set_bit_rate(uint32_t bitrate)
{
    // Set bit rate in Kb/s per channel for MPEG encoding
    sprintf(this->_bitrate, "B=%u", bitrate);
    this->_bitrateLength = strlen(this->_bitrate);
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
    this->_bitDepthLength = strlen(this->_bitDepth);
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
    uint16_t length = strlen(text) + 2;
    sprintf(this->_text, "T=");
    if (length <= maxTextLength)
    {
        strncpy(this->_text + 2, text, length);
        this->_textLength = length;
        this->_textSet = true;
    }
}

void CodingHistoryRow::append_text(const char* text)
{
    // Truncate and append text
    this->_textLength = strlen(text);
    int16_t remaining = (maxTextLength - 2) - this->_textLength;
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

void CodingHistoryRow::get(char* buff)
{
    /* Prints to buffer
    Ends with newline
    Does not add NULL term */
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
    buff[index] = '\n';
}

uint32_t CodingHistoryRow::size()
{
    this->_rowSize = 0;
    this->_rowSize += (
            _algorithmLength
            + _samplerateLength
            + _bitrateLength
            + _bitDepthLength
            + _modeLength
            + _textLength
        );
    // Add 1 per comma for each included field
    for (bool* setter: this->_setters) this->_rowSize += *setter;
    this->_rowSize += 1; // LF/CR at end
    return this->_rowSize;
}

BEXTChunk::BEXTChunk() :
TimecodeBase::Timestamp(),
Chunk("bext"),
bwfVersion(supportedBWFVersion),
_umidSet(false),
loudnessValue(0x7FFF),
loudnessRange(0x7FFF),
maxTruePeakLevel(0x7FFF),
maxMomentaryLoudness(0x7FFF),
maxShortTermLoudness(0x7FFF)
{
    this->samplesSinceMidnight = 0;
    this->timeReferenceLow = reinterpret_cast<uint32_t*>(&(this->samplesSinceMidnight));
    this->timeReferenceHigh = this->timeReferenceLow + 1;
    clear();
}

BEXTChunk::~BEXTChunk()
{
}

void BEXTChunk::clear()
{
    set_bwf_version(supportedBWFVersion);
    clear_description();
    clear_originator();
    #if AUTO_ORIGINATOR_REF_USID
    _set_random_str();
    #else
    clear_originator_reference();
    #endif
    clear_date();
    clear_time();
    clear_umid();
    clear_timestamp();
    clear_loudness();
    clear_coding_history();
    set_reserved();
}

void BEXTChunk::set_description(const char* newDescription)
{
    /* Per EBU TECH 3285:
    ASCII string (maximum 256 characters) containing a free description of
    the sequence. To help applications which display only a short
    description, it is recommended that a resume of the description is
    contained in the first 64 characters and the last 192 characters are used
    for details. */
    strncpy(this->description, newDescription, 255);
}

void BEXTChunk::clear_description()
{
    memset(this->description, 0, 257);
}

void BEXTChunk::set_originator(const char* newOriginator)
{
    /* Per EBU TECH 3285:
    ASCII string (maximum 32 characters) containing the name of the
    originator/ producer of the audio file. */
    strncpy(this->originator, newOriginator, 31);
}

void BEXTChunk::clear_originator()
{
    memset(this->originator, 0, 33);
}

void BEXTChunk::_set_country_code(const char* code)
{
    /* Per EBU R99-1999:
    Country code: (2 characters) based on
    the ISO 3166 standard */
    #if AUTO_ORIGINATOR_REF_USID
    memcpy(this->originatorReference, code, 2);
    #endif
}

void BEXTChunk::_set_org_code(const char* code)
{
    /* Per EBU R99-1999: Organisation code: (3 characters)
    based on the EBU facility codes. */
    #if AUTO_ORIGINATOR_REF_USID
    memcpy(this->originatorReference + 2, code, 3);
    #endif
}

void BEXTChunk::_set_random_str()
{
    /* Per EBU R99-1999:
    Random Number (9 characters 0-9) Generated locally
    by the recorder using some reasonably random algorithm. */
    get_random_str(this->originatorReference + 23, 9, usidRandomValidChars, 0);
}

void BEXTChunk::set_audio_recorder_serial_number(const char* text)
{
    /* Per EBU R99-1999:
    Serial number: (12 characters extracted from
    the recorder model and serial number)
    This should identify the machine’s type
    and serial number. */
    #if AUTO_ORIGINATOR_REF_USID
    memcpy(this->originatorReference + 5, text, 12);
    #endif
}

void BEXTChunk::set_originator_reference(const char* newReference)
{
    /* Per EBU TECH 3285:
    ASCII string (maximum 32 characters) containing an unambiguous
    reference allocated by the originating organisation. */
    strncpy(this->originatorReference, newReference, 31);
}

void BEXTChunk::clear_originator_reference()
{
    memset(this->originatorReference, 0, 33);
    #if AUTO_ORIGINATOR_REF_USID
    _set_random_str();
    #endif
}

void BEXTChunk::set_date(int16_t year, uint8_t month, uint8_t day)
{
    /* Per EBU TECH 3285:
    10 ASCII characters containing the date of creation of the audio
    sequence. The format shall be « ‘,year’,-,’month,’-‘,day,’»
    with 4 characters for the year and 2 characters per other item. */
    sprintf(this->originationDate, "%04u-%02u-%02u", year, month, day);
}

void BEXTChunk::set_date_str(const char* date)
{
    // Set date string literal directly
    strncpy(this->originationDate, date, 10);
}

void BEXTChunk::clear_date()
{
    strncpy(this->originationDate, "0000-00-00", 11);
}

void BEXTChunk::set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    /* Per EBU TECH 3285:
    8 ASCII characters containing the time of creation of the audio sequence.
    The format shall be « ‘hour’-‘minute’-‘second’» with 2 characters per
    item. */
    sprintf(this->originationTime, "%02u:%02u:%02u", hour, minute, second);
    #if AUTO_ORIGINATOR_REF_USID
    memcpy(this->originatorReference + 17, this->originationTime, 2);
    memcpy(this->originatorReference + 19, this->originationTime + 3, 2);
    memcpy(this->originatorReference + 21, this->originationTime + 6, 2);
    #endif
}

void BEXTChunk::set_time_str(const char* time)
{
    // Set time string literal directly
    strncpy(this->originationTime, time, 8);
    #if AUTO_ORIGINATOR_REF_USID
    memcpy(this->originatorReference + 17, time, 2);
    memcpy(this->originatorReference + 19, time + 3, 2);
    memcpy(this->originatorReference + 21, time + 6, 2);
    #endif
}

void BEXTChunk::clear_time()
{
    strncpy(this->originationTime, "00-00-00", 9);
    #if AUTO_ORIGINATOR_REF_USID
    memcpy(this->originatorReference + 17, "000000", 6);
    #endif
}

void BEXTChunk::set_timestamp(uint64_t samplesSinceMidnight)
{
    uint32_t* cast = reinterpret_cast<uint32_t*>(&samplesSinceMidnight);
    *this->timeReferenceLow = *cast;
    *this->timeReferenceHigh = *(cast + 1);
}

void BEXTChunk::set_timestamp(uint32_t low, uint32_t high)
{
    /* Per EBU TECH 3285:
    These fields shall contain the time-code of the sequence. It is a 64-bit
    value which contains the first sample count since midnight. The number
    of samples per second depends on the sample frequency which is defined
    in the field <nSamplesPerSec> from the <format chunk>. */
    *this->timeReferenceLow = low;
    *this->timeReferenceHigh = high;
}

void BEXTChunk::clear_timestamp()
{
    *this->timeReferenceLow = 0;
    *this->timeReferenceHigh = 0;
}

void BEXTChunk::set_bwf_version(uint16_t versionNumber)
{
    /* Per EBU TECH 3285:
    An unsigned binary number giving the version of the BWF. This number is
    particularly relevant for the carriage of the UMID and loudness
    information. For Version 1 it shall be set to 0001h and for Version 2 it
    shall be set to 0002h. */
    this->bwfVersion = versionNumber;
}

bool BEXTChunk::loudness_is_set()
{
    return (
            this->loudnessValue
            && this->loudnessRange
            && this->maxTruePeakLevel
            && this->maxMomentaryLoudness
            && this->maxShortTermLoudness
        );
}

void BEXTChunk::_autoset_bwf_version()
{
    bool loudnessSet = loudness_is_set();
    if (loudnessSet && this->_umidSet) set_bwf_version(2);
    else if (!loudnessSet && this->_umidSet) set_bwf_version(1);
    else if (this->bwfVersion) set_bwf_version(0);
}

void BEXTChunk::set_umid(const uint8_t* newUmid)
{
    /* Per EBU TECH 3285:
    64 bytes containing a UMID (Unique Material Identifier) to standard
    SMPTE 330M [1]. If only a 32 byte "basic UMID" is used, the last 32 bytes
    should be set to zero. (The length of the UMID is given internally.) */
    strncpy(
            reinterpret_cast<char*>(this->umid),
            reinterpret_cast<const char*>(newUmid),
            64
        );
    this->_umidSet = true;
}

void BEXTChunk::clear_umid()
{
    memset(this->umid, 0, 64);
    this->_umidSet = false;
}

int16_t BEXTChunk::set_loudness_value(double value)
{
    // Returns converted int16_t value per EBU TECH 3285
    this->loudnessValue = convert_loudness_to_int<double>(value);
    return this->loudnessValue;
}

int16_t BEXTChunk::set_loudness_range(double range)
{
    // Returns converted int16_t value per EBU TECH 3285
    this->loudnessRange = convert_loudness_to_int<double>(range);
    return this->loudnessRange;
}

int16_t BEXTChunk::set_loudness_max_short_term(double value)
{
    // Returns converted int16_t value per EBU TECH 3285
    this->maxShortTermLoudness = convert_loudness_to_int<double>(value);
    return this->maxShortTermLoudness;
}

int16_t BEXTChunk::set_loudness_max_momentary(double level)
{
    // Returns converted int16_t value per EBU TECH 3285
    this->maxMomentaryLoudness = convert_loudness_to_int<double>(level);
    return this->maxMomentaryLoudness;
}

int16_t BEXTChunk::set_loudness_max_true_peak(double level)
{
    // Returns converted int16_t value per EBU TECH 3285
    this->maxTruePeakLevel = convert_loudness_to_int<double>(level);
    return this->maxTruePeakLevel;
}

void BEXTChunk::clear_loudness()
{
    /* Per EBU TECH 3285:
    If any of the loudness parameters are not being used
    then their 16-bit integer values shall be set to
    7FFFh, which is a value outside the range of
    the parameter values. */
    this->loudnessValue = 0x7FFF;
    this->loudnessRange = 0x7FFF;
    this->maxTruePeakLevel = 0x7FFF;
    this->maxMomentaryLoudness = 0x7FFF;
    this->maxShortTermLoudness = 0x7FFF;
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
    const uint32_t rowSize = row.size();
    char buff[rowSize + 1];
    row.get(buff);
    buff[rowSize] = '\0';
    this->codingHistory = std::string(buff);
}

void BEXTChunk::set_coding_history(std::string history)
{
    this->codingHistory = history;
}

void BEXTChunk::set_coding_history(const char* history)
{
    this->codingHistory = std::string(history);
}

void BEXTChunk::append_to_coding_history(CodingHistoryRow row)
{
    const uint32_t rowSize = row.size();
    char buff[rowSize + 1];
    row.get(buff);
    buff[rowSize] = '\0';
    this->codingHistory.append(std::string(buff));
}

void BEXTChunk::clear_coding_history()
{
    this->codingHistory = std::string("");
}

uint32_t BEXTChunk::size()
{
    /* Return size of chunk data only (excludes ChunkID and size blocks)
        Description (256)
        + Originator (32)
        + Originator Reference (32)
        + Date (10)
        + Time (8)
        + Timestamp (8)
        + BWF Version (2)
        + UMID (64)
        + Loudness (10)
        + Reserved (180)
        = 602 + Coding History Size */
    this->chunkSize = 602 + this->codingHistory.size();
    this->chunkSize += (this->chunkSize % 2);
    return this->chunkSize;
}

size_t BEXTChunk::total_size()
{
    return size() + 8; // Chunk + ID and Size fields
}

size_t BEXTChunk::get(uint8_t* buff)
{
    size();
    size_t index(8);
    memcpy(buff, this->chunkID, 4);
    memcpy(buff + 4, &this->chunkSize, 4);
    memcpy(buff + index, this->description, 256);
    index += 256;
    memcpy(buff + index, this->originator, 32);
    index += 32;
    memcpy(buff + index, this->originatorReference, 32);
    index += 32;
    memcpy(buff + index, this->originationDate, 10);
    index += 10;
    memcpy(buff + index, this->originationTime, 8);
    index += 8;
    memcpy(buff + index, &this->samplesSinceMidnight, 8);
    index += 8;
    memcpy(buff + index, &this->bwfVersion, 2);
    index += 2;
    memcpy(buff + index, &this->umid, 64);
    index += 64;
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
    memcpy(buff + index, this->reserved, 180);
    index += 180;
    memcpy(buff + index, this->codingHistory.c_str(), this->codingHistory.size());
    index += this->codingHistory.size();
    // Make sure chunk ends on even-numbered byte
    if (index % 2) buff[index++] = '\0'; // Pad with NULL
    return index;
}

size_t BEXTChunk::set(const uint8_t* data)
{
    size_t index = 0;
    memcpy(&this->chunkID, data + index, 4);
    index += 4;
    size();
    memcpy(&this->chunkSize, data + index, 4);
    index += 4;
    memcpy(this->description, data + index, 256);
    index += 256;
    memcpy(this->originator, data + index, 32);
    index += 32;
    memcpy(this->originatorReference, data + index, 32);
    index += 32;
    memcpy(this->originationDate, data + index, 10);
    index += 10;
    memcpy(this->originationTime, data + index, 8);
    index += 8;
    memcpy(&this->samplesSinceMidnight, data + index, 8);
    index += 8;
    memcpy(&this->bwfVersion, data + index, 2);
    index += 2;
    memcpy(&this->umid, data + index, 64);
    index += 64;
    memcpy(&this->loudnessValue, data + index, 2);
    index += 2;
    memcpy(&this->loudnessRange, data + index, 2);
    index += 2;
    memcpy(&this->maxTruePeakLevel, data + index, 2);
    index += 2;
    memcpy(&this->maxMomentaryLoudness, data + index, 2);
    index += 2;
    memcpy(&this->maxShortTermLoudness, data + index, 2);
    index += 2;
    memcpy(this->reserved, data + index, 180);
    index += 180;
    size_t remaining = this->chunkSize - index;
    char history[remaining];
    memcpy(history, data + index, remaining);
    set_coding_history(history);
    index += remaining;
    return index;
}
