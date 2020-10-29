#ifndef BWF_H
#define BWF_H

#include "BEXTChunk.h"
#include "iXML.h"

namespace BWF
{

class ChunkSize64;
class DS64Chunk;
class BroadcastWav;

class ChunkSize64
{
protected:
    int _index;
    uint64_t chunkSize64;
    uint32_t *chunkSize64Low, *chunkSize64High;

public:
    char chunkID[4];
    
    ChunkSize64();
    ChunkSize64(const char* extensionChunkId);
    
    virtual void set_chunk_id(const char* extensionChunkId);

    virtual void set_size(uint64_t numBytes);
    virtual uint64_t size();
    
    friend class DS64Chunk;
    friend class BroadcastWav;
};

class DS64Chunk : public WavMeta::Chunk
{
protected:
    uint64_t bw64Size, dataSize;
    uint32_t
        *bw64SizeLow, *bw64SizeHigh,
        *dataSizeLow, *dataSizeHigh,
        *dummyLow, *dummyHigh,
        tableSize;
    std::vector<std::shared_ptr<ChunkSize64>> chunkTable;

public:
    DS64Chunk();
    
    virtual void set_table_size(uint32_t numBytes);
    virtual size_t get_table_size();

    virtual size_t get_table(uint8_t* buff);

    uint32_t size() override;

    size_t get(uint8_t* buff) override;

    friend class BroadcastWav;
};

class BroadcastWav : virtual public WavMeta::WavHeader, virtual public iXML::IXML
{
public:
    int takeNumber;
    uint8_t userbits[4];

    BroadcastWav();
    ~BroadcastWav();

    void clear() override;

    // File size (Header + Data)
    void set_file_size(size_t numBytes) override;
    size_t get_file_size() override;

    // Header size (excluding RIFF ID + Size)
    size_t size() override;
    
    // Total header size including RIFF header
    size_t total_size() override;

    size_t get(uint8_t* buff) override;
    // size_t set(const uint8_t* data) override;

/*                              Format                              */

public:
    void set_sample_rate(uint32_t samplerate) override;
    void set_bit_depth(uint16_t bitsPerSample) override;
    void set_channels(uint16_t channels) override;
    virtual void set_channels();

/*                             Timecode                             */

public:
    void set_timecode(int hr, int min, int sec, int frm);
    void set_timecode(std::array<int, 4> tc);
    void set_timecode(int numFrames);
    void set_timecode(TimecodeBase::Base& base) override;
    void set_timecode(TimecodeBase::Clock& clock) override;
    void clear_timecode();
    
    // Userbits
    virtual void set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);
    virtual void set_ubits(const char* bits);
    virtual uint8_t* get_ubits();

/*                               Data                               */

public:
    void set_data_size(size_t numBytes) override;
    size_t get_data_size();

/*                            64-bit WAV                            */

protected:
    // JUNK chunk until transformed
    DS64Chunk ds64Chunk;

    // BW64 enabled flag
    bool _bw64;

    virtual void _autoset_wav_64();

public:
    virtual void set_wav_64();
    virtual void unset_wav_64();
    virtual bool is_wav_64();
    
    // Create an extension chunk
    virtual std::shared_ptr<ChunkSize64> create_chunk();
    virtual std::shared_ptr<ChunkSize64> extend_data();

    // Destroy an extension chunk
    virtual void destroy_chunk(std::shared_ptr<ChunkSize64> chunk);
    virtual void destroy_chunk(int chunkIndex);

/*                               BEXT                               */

protected:
    BEXT::BEXTChunk bextChunk;

public:
    // Originator
    void set_originator(const char* newOriginator) override;
    void clear_originator() override;

    // Originator Reference
    void set_originator_reference(const char* newReference) override;
    void clear_originator_reference() override;

    // Description
    void set_description(const char* newDescription) override;
    void clear_description() override;

    // Origination Date
    virtual void set_date(int16_t year, uint8_t month, uint8_t day);
    void set_date_str(const char* date) override;
    void clear_date() override;

    // Origination Time
    virtual void set_time(uint8_t hour, uint8_t minute, uint8_t second);
    void set_time_str(const char* time) override;
    void clear_time() override;

    // BWF Version
    void set_bwf_version(uint16_t versionNumber) override;

    // UMID
    void set_umid(const uint8_t* newUmid) override;
    void clear_umid() override;

    // Loudness
    void set_loudness_value(uint16_t value) override;
    void set_loudness_range(uint16_t range) override;
    void set_loudness_max_true_peak(uint16_t level) override;
    void set_loudness_max_momentary(uint16_t level) override;
    void set_loudness_max_short_term(uint16_t value) override;
    void clear_loudness() override;

    // Reserved
    virtual void set_reserved();

    // Coding History
    void set_coding_history(std::string history) override;
    void set_coding_history(const char* history) override;
    virtual void set_coding_history(BEXT::CodingHistoryRow row);
    virtual void append_to_coding_history(BEXT::CodingHistoryRow row);
    void clear_coding_history() override;

/*                               iXML                               */

public:
    // iXML version
    virtual void set_ixml_version(uint16_t major, uint16_t minor);
    virtual std::pair<const uint16_t, const uint16_t> get_ixml_version();

    // Project
    virtual void set_project(const char* projectName);
    virtual const char* get_project();

    // Tape/Sound Roll
    virtual void set_tape(const char* tapeName);
    virtual const char* get_tape();

    // Scene name
    virtual void set_scene(const char* sceneName);
    virtual const char* get_scene();

    // Take number
    virtual void set_take(int takeNum);
    virtual int get_take();

    // Circle Take
    virtual void set_circled(bool isCircled);
    virtual bool is_circled();

    // File UID
    virtual void set_file_uid();
    virtual const char* get_file_uid();
    
    // Take note
    virtual void set_note(const char* message);
    virtual const char* get_note();

    // Take type
    virtual void set_default_take_type();
    virtual bool is_default_take_type();

    virtual void set_no_good(bool flagged);
    virtual bool is_no_good();

    virtual void set_false_start(bool flagged);
    virtual bool is_false_start();

    virtual void set_wild_track(bool flagged);
    virtual bool is_wild_track();

    virtual void set_pickup(bool flagged);
    virtual bool is_pickup();

    virtual void set_rehearsal(bool flagged);
    virtual bool is_rehearsal();

    virtual void set_announcement(bool flagged);
    virtual bool is_announcement();

    virtual void set_sound_guide(bool flagged);
    virtual bool is_sound_guide();

//     // BEXT
// public:
//     void import_bext_chunk(BEXT::BEXTChunk& chunk);

};

};

#endif
