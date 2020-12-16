#ifndef DATAINGESTOR_H
#define DATAINGESTOR_H

#include "AudioDataPad.h"
#include "WavWriter.h"
#include "RingBuffer.h"
#include "EspI2S.h"

enum data_ingestor_err
{
    BIT_DEPTH_NOT_SET = 180,
    NOT_IMPLEMENTED = 181
};

class DataIngestor :
virtual public WavMeta::WavFormat,
virtual public DataPad::Packer,
virtual public I2S::Bus
{
protected:
public:
    // Unpacked audio buffer
    Buffer::RingBuffer<int_audio> _buff;
    size_t _size;

    // Packed audio buffers
    uint8_t* _packedInput;
    uint8_t* _packedOutput;

    virtual void _create_packed_buffers();
    
    virtual void _unpack_from_i2s();

public:
    DataIngestor();
    DataIngestor(
            uint32_t bufferSize,
            uint32_t ringSize,
            int i2sNum,
            bool isMaster,
            bool receives,
            bool transmits
        );
    ~DataIngestor();

    virtual size_t size();

    virtual void set_size(int bufferSize, uint8_t ringSize);
    
    virtual size_t delay_in_samples() const;

    void set_bit_depth(uint16_t bitsPerSample) override;
    void set_sample_rate(uint32_t samplerate) override;
    void set_channels(uint16_t channels) override;
    void set_format(const WavFormatData& data) override;

    void set_pcm() override;
    void set_floating_point() override;
    void set_mpeg_1() override;

    virtual void start();
    virtual void stop();
    virtual void reload();
    virtual size_t step();

    int buffered();

    virtual uint8_t* read();
    virtual uint8_t* read_raw();
};

#endif
