#include "BWFHeader.h"

using namespace BWFHeader;

ChunkSize64::ChunkSize64() :
chunkSize64(0)
{
    this->chunkSize64Low = reinterpret_cast<uint32_t*>(chunkSize64);
    this->chunkSize64High = this->chunkSize64Low + 1;
}

ChunkSize64::ChunkSize64(const char* extensionChunkId) :
chunkSize64(0)
{
    memcpy(this->chunkID, extensionChunkId, 4);
    this->chunkSize64Low = reinterpret_cast<uint32_t*>(chunkSize64);
    this->chunkSize64High = this->chunkSize64Low + 1;
}

void ChunkSize64::set_chunk_id(const char* extensionChunkId)
{
    memcpy(this->chunkID, extensionChunkId, 4);
}

void ChunkSize64::set_size(uint64_t numBytes)
{
    this->chunkSize64 = numBytes;
}

uint64_t ChunkSize64::size()
{
    return this->chunkSize64;
}

DS64Chunk::DS64Chunk() :
Chunk("JUNK")
{
    size();
}

size_t DS64Chunk::get_table_size()
{
    this->tableSize = this->chunkTable.size() * 12;
    return this->tableSize;
}

size_t DS64Chunk::get_table(uint8_t* buff)
{
    size_t index(0);
    int chunkIndex(0);
    std::sort(this->chunkTable.begin(), this->chunkTable.end());
    for (std::shared_ptr<ChunkSize64> chunk: this->chunkTable)
    {
        chunk->_index = chunkIndex++;
        memcpy(buff + index, chunk->chunkID, 4);
        index += 4;
        memcpy(buff + index, &chunk->chunkSize64, 8);
        index += 8;
    }
    return index;
}

uint32_t DS64Chunk::size()
{
    this->tableSize = get_table_size();
    this->chunkSize = 28 + this->tableSize;
    return this->chunkSize;
}

size_t DS64Chunk::get(uint8_t* buff)
{
    constexpr double dummy(0);
    uint32_t tableLength = this->chunkTable.size();
    size_t index(Chunk::get(buff));
    memcpy(buff + index, &this->bw64Size, 8);
    index += 8;
    memcpy(buff + index, &this->dataSize, 8);
    index += 8;
    memcpy(buff + index, &dummy, 8);
    index += 8;
    memcpy(buff + index, &tableLength, 4);
    index += 4;
    index += get_table(buff + index);
    return index;
}

BroadcastWav::BroadcastWav() :
WavMeta::WavHeader(),
iXML::IXML(),
Loudness::Analyzer(),
_maxHeaderSize(8192),
takeNumber(0),
userbits{0, 0, 0, 0},
_bw64(false),
_circled(false)
{
    set_bwf_version(BEXT::supportedBWFVersion);
    unset_wav_64();
}

BroadcastWav::~BroadcastWav()
{
}

void BroadcastWav::clear()
{
    this->bextChunk.clear();
    iXML::IXML::clear();
}

void BroadcastWav::set_sample_rate(uint32_t samplerate)
{
    iXML::IXML::set_sample_rate(samplerate);
    WavHeader::set_sample_rate(this->sampleRate);
    Loudness::Analyzer::set_sample_rate(this->sampleRate);
}

void BroadcastWav::set_bit_depth(uint16_t bitsPerSample)
{
    WavHeader::set_bit_depth(bitsPerSample);
    iXML::IXML::set_bit_depth(bitsPerSample);
    Loudness::Analyzer::set_bit_depth(bitsPerSample);
}

void BroadcastWav::set_channels(uint16_t channels)
{
    WavHeader::set_channels(channels);
    iXML::IXML::set_channels(channels);
    Loudness::Analyzer::set_channels(channels);
}

void BroadcastWav::set_channels()
{
    /* Set number of channels to number
    of tracks found in iXML TrackList */
    set_channels(this->numTracks);
}

void BroadcastWav::set_format_code(uint16_t formatcode)
{
    WavHeader::set_format_code(formatcode);
    Loudness::Analyzer::set_format_code(formatcode);
}

void BroadcastWav::set_timecode(int hr, int min, int sec, int frm)
{
    iXML::IXML::set_timecode(hr, min, sec, frm);
    this->bextChunk.set_timestamp(*this->timeReferenceLow, *this->timeReferenceHigh);
}

void BroadcastWav::set_timecode(std::array<int, 4> tc)
{
    iXML::IXML::set_timecode(tc);
    this->bextChunk.set_timestamp(*this->timeReferenceLow, *this->timeReferenceHigh);
}

void BroadcastWav::set_timecode(int numFrames)
{
    iXML::IXML::set_timecode(numFrames);
    this->bextChunk.set_timestamp(*this->timeReferenceLow, *this->timeReferenceHigh);
}

void BroadcastWav::set_timecode(TimecodeBase::Base& base)
{
    iXML::IXML::set_timecode(base);
    this->bextChunk.set_timestamp(*this->timeReferenceLow, *this->timeReferenceHigh);
}

void BroadcastWav::set_timecode(TimecodeBase::Clock& clock)
{
    iXML::IXML::set_timecode(clock);
    this->bextChunk.set_timestamp(*this->timeReferenceLow, *this->timeReferenceHigh);
}

void BroadcastWav::clear_timecode()
{
    iXML::IXML::clear_timecode();
    this->bextChunk.set_timestamp(*this->timeReferenceLow, *this->timeReferenceHigh);
}

inline void BroadcastWav::set_data_size(size_t numBytes)
{
    if (this->_bw64)
    {
        this->ds64Chunk.dataSize = numBytes;
    }
    else
    {
        this->WavHeader::set_data_size(numBytes);
    }
}

inline size_t BroadcastWav::get_data_size()
{
    return (
            this->_bw64 ?
            this->ds64Chunk.dataSize
            : this->dataChunk.chunkSize
        );
}

void BroadcastWav::set_file_size(size_t numBytes)
{
    if (this->_bw64) this->ds64Chunk.bw64Size = numBytes;
    else this->riffChunk.chunkSize = static_cast<uint32_t>(numBytes);
}

size_t BroadcastWav::get_file_size()
{
    return (
            this->_bw64 ?
            this->ds64Chunk.bw64Size
            : this->riffChunk.chunkSize
        );
}

bool BroadcastWav::is_wav_64()
{
    return this->_bw64;
}

void BroadcastWav::set_wav_64()
{
    this->_bw64 = true;
    memcpy(this->riffChunk.chunkID, "BW64", 4);
    memcpy(this->ds64Chunk.chunkID, "ds64", 4);
    this->riffChunk.chunkSize = 0xFFFFFFFF;
    this->dataChunk.chunkSize = 0xFFFFFFFF;
}

void BroadcastWav::unset_wav_64()
{
    this->dataChunk.chunkSize = get_data_size();
    this->riffChunk.chunkSize = this->_headerSize + this->dataChunk.chunkSize;
    memcpy(this->riffChunk.chunkID, "RIFF", 4);
    memcpy(this->ds64Chunk.chunkID, "JUNK", 4);
    this->_bw64 = false;
}

void BroadcastWav::_autoset_wav_64()
{
    if (this->ds64Chunk.chunkTable.size()) set_wav_64();
    else if (this->_bw64) unset_wav_64();
}

std::shared_ptr<ChunkSize64> BroadcastWav::create_chunk()
{
    this->ds64Chunk.chunkTable.emplace_back(
            std::make_shared<ChunkSize64>(ChunkSize64("data"))
        );
    std::shared_ptr<ChunkSize64> chunk = this->ds64Chunk.chunkTable.back();
    chunk->_index = this->ds64Chunk.chunkTable.size();
    return chunk;
}

std::shared_ptr<ChunkSize64> BroadcastWav::extend_data()
{
    if (!this->_bw64) set_wav_64();
    return create_chunk();
}

void BroadcastWav::destroy_chunk(std::shared_ptr<ChunkSize64> chunk)
{
    this->ds64Chunk.chunkTable.erase(
            this->ds64Chunk.chunkTable.begin() + chunk->_index
        );
}

void BroadcastWav::destroy_chunk(int chunkIndex)
{
    this->ds64Chunk.chunkTable.erase(
            this->ds64Chunk.chunkTable.begin() + chunkIndex
    )   ;
}

void BroadcastWav::set_originator(const char* newOriginator)
{
    this->bextChunk.set_originator(newOriginator);
    iXML::IXML::set_originator(this->bextChunk.originator);
}

void BroadcastWav::clear_originator()
{
    this->bextChunk.clear_originator();
    iXML::IXML::clear_originator();
}

void BroadcastWav::set_country_code(const char* code)
{
    this->bextChunk._set_country_code(code);
}

void BroadcastWav::set_org_code(const char* code)
{
    this->bextChunk._set_org_code(code);
}

void BroadcastWav::set_originator_reference(const char* newReference)
{
    this->bextChunk.set_originator_reference(newReference);
    iXML::IXML::set_originator_reference(this->bextChunk.originatorReference);
}

void BroadcastWav::clear_originator_reference()
{
    this->bextChunk.clear_originator_reference();
    iXML::IXML::clear_originator_reference();
}

void BroadcastWav::set_description(const char* newDescription)
{
    this->bextChunk.set_description(newDescription);
    iXML::IXML::set_description(this->bextChunk.description);
}

void BroadcastWav::clear_description()
{
    this->bextChunk.clear_description();
    iXML::IXML::clear_description();
}

void BroadcastWav::set_date(int16_t year, uint8_t month, uint8_t day)
{
    this->bextChunk.set_date(year, month, day);
    iXML::IXML::set_date_str(this->bextChunk.originationDate);
}

void BroadcastWav::set_date_str(const char* date)
{
    this->bextChunk.set_date_str(date);
    iXML::IXML::set_date_str(this->bextChunk.originationDate);
}

void BroadcastWav::clear_date()
{
    this->bextChunk.clear_date();
    iXML::IXML::clear_date();
}

void BroadcastWav::set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    this->bextChunk.set_time(hour, minute, second);
    iXML::IXML::set_time_str(this->bextChunk.originationTime);
    iXML::IXML::set_originator_reference(this->bextChunk.originatorReference);
}

void BroadcastWav::set_time_str(const char* time)
{
    this->bextChunk.set_time_str(time);
    iXML::IXML::set_time_str(this->bextChunk.originationTime);
    iXML::IXML::set_originator_reference(this->bextChunk.originatorReference);
}

void BroadcastWav::clear_time()
{
    this->bextChunk.clear_time();
    iXML::IXML::clear_time();
    iXML::IXML::set_originator_reference(this->bextChunk.originatorReference);
}

void BroadcastWav::set_bwf_version(uint16_t versionNumber)
{
    this->bextChunk.set_bwf_version(versionNumber);
    iXML::IXML::set_bwf_version(versionNumber);
}

void BroadcastWav::set_umid(const uint8_t* newUmid)
{
    this->bextChunk.set_umid(newUmid);
    iXML::IXML::set_umid(this->bextChunk.umid);
}

void BroadcastWav::clear_umid()
{
    this->bextChunk.clear_umid();
    iXML::IXML::clear_umid();
}

void BroadcastWav::set_loudness_value(double value)
{
    this->bextChunk.set_loudness_value(value);
    iXML::IXML::set_loudness_value(value);
}

void BroadcastWav::set_loudness_range(double value)
{
    this->bextChunk.set_loudness_range(value);
    iXML::IXML::set_loudness_range(value);
}

void BroadcastWav::set_loudness_max_short_term(double value)
{
    this->bextChunk.set_loudness_max_short_term(value);
    iXML::IXML::set_loudness_max_short_term(value);
}

void BroadcastWav::set_loudness_max_momentary(double value)
{
    this->bextChunk.set_loudness_max_momentary(value);
    iXML::IXML::set_loudness_max_momentary(value);
}

void BroadcastWav::set_loudness_max_true_peak(double value)
{
    this->bextChunk.set_loudness_max_true_peak(value);
    iXML::IXML::set_loudness_max_true_peak(value);
}

void BroadcastWav::set_loudness()
{
    #ifdef EBUR128_H_
    double value, range;
    value = Loudness::Analyzer::get_loudness_global();
    range = Loudness::Analyzer::get_loudness_range();
    Loudness::Analyzer::get_loudness_short_term();
    Loudness::Analyzer::get_loudness_momentary();
    Loudness::Analyzer::get_loudness_true_peak();
    set_loudness_value(value);
    set_loudness_range(range);
    set_loudness_max_short_term(this->_maxShortTerm);
    set_loudness_max_momentary(this->_maxMomentary);
    set_loudness_max_true_peak(this->_maxTruePeak);
    #else
    clear_loudness();
    #endif
}

void BroadcastWav::clear_loudness()
{
    Loudness::Analyzer::clear();
    this->bextChunk.clear_loudness();
    iXML::IXML::clear_loudness();
}

void BroadcastWav::set_reserved()
{
    this->bextChunk.set_reserved();
    iXML::IXML::set_reserved(this->bextChunk.reserved);
}

void BroadcastWav::set_coding_history(std::string history)
{
    this->bextChunk.set_coding_history(history);
    iXML::IXML::set_coding_history(this->bextChunk.codingHistory.c_str());
}

void BroadcastWav::set_coding_history(const char* history)
{
    this->bextChunk.set_coding_history(history);
    iXML::IXML::set_coding_history(this->bextChunk.codingHistory.c_str());
}

void BroadcastWav::set_coding_history(BEXT::CodingHistoryRow row)
{
    this->bextChunk.set_coding_history(row);
    iXML::IXML::set_coding_history(this->bextChunk.codingHistory.c_str());
}

void BroadcastWav::append_to_coding_history(BEXT::CodingHistoryRow row)
{
    this->bextChunk.append_to_coding_history(row);
    iXML::IXML::set_coding_history(this->bextChunk.codingHistory.c_str());
}

void BroadcastWav::clear_coding_history()
{
    this->bextChunk.clear_coding_history();
    iXML::IXML::clear_coding_history();
}

void BroadcastWav::set_project(const char* projectNameText)
{
    iXML::IXML::set_project(projectNameText);
}

void BroadcastWav::set_tape(const char* tapeNameText)
{
    iXML::IXML::set_tape(tapeNameText);
}

void BroadcastWav::set_scene(const char* sceneNameText)
{
    iXML::IXML::set_scene(sceneNameText);
}

void BroadcastWav::set_take(int takeNum)
{
    this->takeNumber = takeNum;
    iXML::IXML::set_take(takeNum);
}

int BroadcastWav::get_take()
{
    return this->takeNumber;
}

int BroadcastWav::increment_take()
{
    set_take(++this->takeNumber);
    return this->takeNumber;
}

void BroadcastWav::set_circled(bool isCircled)
{
    if (isCircled != this->_circled)
    {
        const char* sceneName(get_scene());
        const size_t length(std::strlen(sceneName) + 2);
        char buff[length];
        buff[0] = '\0';
        if (isCircled && !this->_circled)
        {
            std::strcat(buff, "@");
            std::strcat(buff, sceneName);
        }
        else if (!isCircled && this->_circled && sceneName[0] == '@')
        {
            std::memmove(buff, sceneName + 1, length - 1);
        }
        set_scene(buff);
    }
    this->_circled = isCircled;
    iXML::IXML::set_circled(this->_circled);
}

bool BroadcastWav::is_circled()
{
    return this->_circled;
}

void BroadcastWav::set_ubits(
        uint8_t first,
        uint8_t second,
        uint8_t third,
        uint8_t fourth
    )
{
    this->userbits[0] = first;
    this->userbits[1] = second;
    this->userbits[2] = third;
    this->userbits[3] = fourth;
    iXML::IXML::set_ubits(first, second, third, fourth);
}

void BroadcastWav::set_ubits(const char* bits)
{
    iXML::IXML::set_ubits(bits);
    memcpy(this->userbits, bits, 4);
}

const uint8_t* BroadcastWav::get_ubits() const
{
    return this->userbits;
}

void BroadcastWav::_write_track_list()
{
    iXML::IXML::_write_track_list();
    this->numChannels = this->numTracks;
}

void BroadcastWav::set_audio_recorder_serial_number(const char* text)
{
    this->bextChunk.set_audio_recorder_serial_number(text);
    iXML::IXML::set_audio_recorder_serial_number(text);
}

size_t BroadcastWav::get_header_size()
{
    // File RIFF ID, size, RIFF type, and data chunk ID and size
    this->_headerSize = 20;

    // Format chunk
    this->_headerSize += this->formatChunk.total_size();

    // ds64Chunk
    this->_headerSize += this->ds64Chunk.total_size();
    
    // BEXT chunk
    this->_headerSize += this->bextChunk.total_size();

    /* Add iXML chunk ID and size
    since IXML is inherited, not child chunk */
    this->_headerSize += iXML::IXML::size() + 8;
    
    size_t padBytes = this->_maxHeaderSize - this->_headerSize;
    this->_ixmlChunkSize += padBytes;
    this->_headerSize += padBytes;
    
    /* Returns size of header only;
    does not include size of actual audio data */
    return this->_headerSize;
}

size_t BroadcastWav::get(uint8_t* buff)
{
    /* Exports header
    Adds data pad to ixml chunk */

    size_t index(12);

    // Write format chunk
    index += this->formatChunk.get(buff + index);

    // Write DS64/JUNK chunk
    index += this->ds64Chunk.get(buff + index);

    // Write BEXT chunk
    index += this->bextChunk.get(buff + index);

    // Save index of ixml chunk size location
    size_t ixmlChunkSizeIndex = index + 4;

    // Write IXML
    index += iXML::IXML::get(buff + index);

    /* Get number of pad bytes needed

    Maximum allowed (_maxHeaderSize)
    - current write position (index)
    - data chunk ID and size (8) */
    size_t padBytes = this->_maxHeaderSize - index - 8;

    // Add pad to ixml chunk size
    this->_ixmlChunkSize += padBytes;

    // Re-write corrected ixml chunk size
    memcpy(buff + ixmlChunkSizeIndex, &this->_ixmlChunkSize, 4);

    /* Add pad to index
    Pad bytes are not set, and should
    be zeroed before calling this funciton */
    index += padBytes;

    // Write data chunk ID and size
    index += this->dataChunk.get(buff + index);

    /* Throw an exception if the
    reserved header size is not enough */
    #ifdef _DEBUG
    assert(index <= this->_maxHeaderSize);
    #endif

    // Set size of file contents
    this->_headerSize = index;

    /* Set RIFF chunk size

    Total file size
    - RIFF chunk ID (4)
    - RIFF chunk size (4) */
    this->set_file_size(
            this->_headerSize - 8 + this->dataChunk.size()
        );

    // Write RIFF chunk ID, file size, and RIFF type
    this->riffChunk.get(buff);

    // Return number of bytes written
    return index;
}
