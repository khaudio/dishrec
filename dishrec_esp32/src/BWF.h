#ifndef BWF_H
#define BWF_H

#include "WavHeader.h"
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

class BroadcastWav : public WavMeta::WavHeader, public iXML::IXML
{
public:
    BroadcastWav();
    ~BroadcastWav();

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

    uint16_t get_channels() override;


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

/*                               Data                               */
public:
    void set_data_size(size_t numBytes) override;
    size_t get_data_size();

};

};

#endif
