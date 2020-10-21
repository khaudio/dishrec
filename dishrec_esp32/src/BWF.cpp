#include "BWF.h"

using namespace BWF;


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

void DS64Chunk::set_table_size(uint32_t numBytes)
{
    this->tableSize = numBytes;
}

size_t DS64Chunk::get_table_size()
{
    return this->chunkTable.size() * 12;
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
    size_t index(Chunk::get(buff));
    memcpy(buff + index, &this->bw64Size, 8);
    index += 8;
    memcpy(buff + index, &this->dataSize, 8);
    index += 8;
    index += get_table(buff + index);
    return index;
}

BroadcastWav::BroadcastWav() :
WavMeta::WavHeader(),
iXML::IXML(),
_bw64(false)
{
}

BroadcastWav::~BroadcastWav()
{
}

void BroadcastWav::set_sample_rate(uint32_t samplerate)
{
    WavHeader::set_sample_rate(samplerate);
    iXML::IXML::set_sample_rate(samplerate);
}

void BroadcastWav::set_bit_depth(uint16_t bitsPerSample)
{
    WavHeader::set_bit_depth(bitsPerSample);
    iXML::IXML::set_bit_depth(bitsPerSample);
}

void BroadcastWav::set_channels(uint16_t channels)
{
    WavHeader::set_channels(channels);
    iXML::IXML::set_channels(channels);
}

uint16_t BroadcastWav::get_channels()
{
    return this->numChannels;
}

void BroadcastWav::set_data_size(size_t numBytes)
{
    if (this->_bw64)
    {
        this->ds64Chunk.dataSize = numBytes;
    }
    else
    {
        this->WavHeader::set_data_size(numBytes);
    }
    size();
}

size_t BroadcastWav::get_data_size()
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

size_t BroadcastWav::size()
{
    this->_headerSize = 12; // _riffType + Data chunk ID and size
    this->_headerSize += BEXT::BEXTChunk::total_size();
    this->_headerSize += iXML::IXML::total_size();
    this->_headerSize += this->formatChunk.total_size();
    return this->_headerSize;
}

size_t BroadcastWav::total_size()
{
    return size() + 8;
}

size_t BroadcastWav::get(uint8_t* buff)
{
    size();
    size_t index(this->riffChunk.get(buff));
    index += BEXT::BEXTChunk::get(buff + index);
    index += iXML::IXML::get(buff + index);
    index += this->formatChunk.get(buff + index);
    index += this->dataChunk.get(buff + index);
    return index;
}
