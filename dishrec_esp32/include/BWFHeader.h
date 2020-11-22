#ifndef BWF_H
#define BWF_H

#include "BEXTChunk.h"
#include "iXML.h"
#include "Loudness.h"

namespace BWFHeader
{

class FileMeta;
class ChunkSize64;
class DS64Chunk;
class BEXTWrapper;
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
    
    virtual size_t get_table_size();
    virtual size_t get_table(uint8_t* buff);

    uint32_t size() override;

    size_t get(uint8_t* buff) override;

    friend class BroadcastWav;
};

class BEXTWrapper : virtual public BEXT::BEXTChunk
{
    friend class BroadcastWav;
};

class BroadcastWav :
virtual public WavMeta::WavHeader,
virtual public iXML::IXML,
virtual public Loudness::Analyzer
{
protected:
    // Number of bytes to reserve when writing header
    size_t _maxHeaderSize;

public:
    int takeNumber;
    uint8_t userbits[4];

    BroadcastWav();
    ~BroadcastWav();

    void clear() override;

    // File size (Header + Data)
    void set_file_size(size_t numBytes) override;
    size_t get_file_size() override;

    // Header size
    size_t get_header_size() override;

    size_t get(uint8_t* buff) override;
    // size_t set(const uint8_t* data) override;

/*                              Format                              */

public:
    void set_sample_rate(uint32_t samplerate) override;
    void set_bit_depth(uint16_t bitsPerSample) override;
    void set_channels(uint16_t channels) override;
    virtual void set_channels();
    void set_format_code(uint16_t formatcode) override;

/*                             Timecode                             */

public:
    void set_timecode(int hr, int min, int sec, int frm) override;
    void set_timecode(std::array<int, 4> tc) override;
    void set_timecode(int numFrames) override;
    void set_timecode(TimecodeBase::Base& base) override;
    void set_timecode(TimecodeBase::Clock& clock) override;
    void clear_timecode() override;
    
    // Userbits
    void set_ubits(
            uint8_t first,
            uint8_t second,
            uint8_t third,
            uint8_t fourth
        ) override;
    void set_ubits(const char* bits) override;
    const uint8_t* get_ubits() const override;

/*                               Data                               */

public:
    void set_data_size(size_t numBytes) override;
    size_t get_data_size() override;

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
    BEXTWrapper bextChunk;

public:
    // Originator
    void set_originator(const char* newOriginator) override;
    void clear_originator() override;

    // Originator Reference
    void set_country_code(const char* code);
    void set_org_code(const char* code);
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
    void set_loudness_value(double value) override;
    void set_loudness_range(double range) override;
    void set_loudness_max_short_term(double value) override;
    void set_loudness_max_momentary(double level) override;
    void set_loudness_max_true_peak(double level) override;
    virtual void set_loudness();
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
    // Project
    void set_project(const char* projectName) override;

    // Tape/Sound Roll
    void set_tape(const char* tapeName) override;

    // Scene name
    void set_scene(const char* sceneName) override;

    // Take number
    void set_take(int takeNum) override;
    int get_take() override;
    virtual int increment_take();

    // Circle Take
    bool _circled;
    void set_circled(bool isCircled) override;
    bool is_circled() override;

    // Track List
    void _write_track_list() override;

    // User
    void set_audio_recorder_serial_number(const char* text) override;
};

};

#endif
