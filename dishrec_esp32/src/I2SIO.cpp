// #include "I2SIO.h"

// // Records number of bytes read each time i2s_read is called
// size_t i2sNumBytesRead(0);

// // Records number of bytes written each time i2s_write is called
// size_t i2sNumBytesWritten(0);

// #ifdef _DEBUG
// // Whether an I2S read from DMA has occured since last check
// bool i2sReadCycled = false;

// // Whether an I2S write to DMA has occured since last check
// bool i2sWriteCycled = false;
// #endif

// i2s_channel_fmt_t get_channel_format(uint16_t numChannels)
// {
//     switch (numChannels)
//     {
//         case (2):
//             return I2S_CHANNEL_FMT_RIGHT_LEFT;
//             break;
//         default:
//             return I2S_CHANNEL_FMT_ONLY_LEFT;
//     }
// }

// i2s_bits_per_sample_t get_bit_depth(uint16_t bitsPerSample)
// {
//     switch (bitsPerSample)
//     {
//         case(8):
//             return I2S_BITS_PER_SAMPLE_8BIT;
//             break;
//         case (16):
//             return I2S_BITS_PER_SAMPLE_16BIT;
//             break;
//         case (24):
//             return I2S_BITS_PER_SAMPLE_24BIT;
//             break;
//         default:
//             return I2S_BITS_PER_SAMPLE_32BIT;
//     }
// }

// void I2S::config(RingBuffer<DATATYPE>* buff)
// {
//     i2s_config_t i2sConfig = {
//             mode : static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
//             sample_rate : static_cast<i2s_comm_format_t>(currentReadFile->sampleRate),
//             bits_per_sample : get_bit_depth(currentReadFile->bitDepth),
//             channel_format : get_channel_format(currentReadFile->numChannels),
//             communication_format : I2S_COMM_FORMAT_I2S_MSB,
//             intr_alloc_flags : ESP_INTR_FLAG_LEVEL1,
//             dma_buf_count : buff->ringLength,
//             dma_buf_len : static_cast<int>(buff->bufferLength),
//             use_apll : true,
//             tx_desc_auto_clear : true
//         };
//     i2s_pin_config_t i2sPinConfig = {
//             bck_io_num : I2S_BCK,
//             ws_io_num : I2S_WS,
//             data_out_num : I2S_DO,
//             data_in_num : I2S_DI
//         };
//     i2s_driver_install(I2S_NUM, &i2sConfig, 0, NULL);
//     i2s_set_pin(I2S_NUM, &i2sPinConfig);
// }

// template <typename T>
// void I2S::read_to_buffer(RingBuffer<T>* buff)
// {
//     esp_err_t status = i2s_read(
//             I2S_NUM,
//             reinterpret_cast<uint8_t*>(buff->get_write_ptr()),
//             buff->bytesPerBuffer,
//             &i2sNumBytesRead,
//             portMAX_DELAY
//         );
//     if (status == ESP_OK) buff->force_rotate_write_buffer();
//     #ifdef _DEBUG
//     else std::cout << "Error reading from DMA!\t" << status << std::endl;
//     i2sReadCycled = true;
//     #endif
// }

// template <typename T>
// void I2S::write_from_buffer(RingBuffer<T>* buff)
// {
//     // std::vector<T> data = buff->read();
//     // const void* buffptr = reinterpret_cast<const void*>(&(data[0]));
//     i2s_write(
//             I2S_NUM,
//             reinterpret_cast<const void*>(buff->get_read_ptr()),
//             buff->bytesPerBuffer,
//             &i2sNumBytesWritten,
//             portMAX_DELAY
//         );
//     buff->rotate_read_buffer();
//     #ifdef _DEBUG
//     i2sWriteCycled = true;
//     #endif
// }

// void I2S::start()
// {
//     // Config I2S input
//     config(&inBuffer);

//     // Enable MCK output
//     REG_WRITE(PIN_CTRL, 0xFF0); 
// 	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
// }

// void I2S::stop()
// {
//     i2s_driver_uninstall(I2S_NUM);
// }

// void I2S::check_bytes_read(RingBuffer<DATATYPE>* buff)
// {
//     #ifdef _DEBUG
//     if (!i2sReadCycled)
//     {
//         return;
//     }
//     if (i2sNumBytesRead < buff->bytesPerBuffer)
//     {
//         uint32_t bytesDropped = (
//                 (buff->bytesPerBuffer - i2sNumBytesRead) / currentReadFile->sampleWidth
//             );
//         std::cerr << bytesDropped << " samples from I2S input buffer dropped (";
//         std::cerr << i2sNumBytesRead << " bytes read)" << std::endl;
//     }
//     else
//     {
//         std::cout << "Read from I2S buffer successfully " << std::endl;
//     }
//     i2sReadCycled = false;
//     #endif
// }

// void I2S::check_bytes_written(RingBuffer<DATATYPE>* buff)
// {
//     #ifdef _DEBUG
//     if (!i2sWriteCycled)
//     {
//         return;
//     }
//     if (i2sNumBytesWritten < buff->bytesPerBuffer)
//     {
//         uint32_t bytesDropped = (
//                 (buff->bytesPerBuffer - i2sNumBytesWritten) / currentWriteFile->sampleWidth
//             );
//         std::cerr << bytesDropped << " samples from I2S output buffer dropped (";
//         std::cerr << i2sNumBytesWritten << " bytes written)" << std::endl;
//     }
//     else
//     {
//         std::cout << "Wrote to I2S buffer successfully " << std::endl;
//     }
//     i2sWriteCycled = false;
//     #endif
// }

// template void I2S::read_to_buffer<float>(RingBuffer<float>*);
// template void I2S::read_to_buffer<double>(RingBuffer<double>*);
// template void I2S::read_to_buffer<long double>(RingBuffer<long double>*);
// template void I2S::read_to_buffer<int8_t>(RingBuffer<int8_t>*);
// template void I2S::read_to_buffer<uint8_t>(RingBuffer<uint8_t>*);
// template void I2S::read_to_buffer<int16_t>(RingBuffer<int16_t>*);
// template void I2S::read_to_buffer<uint16_t>(RingBuffer<uint16_t>*);
// template void I2S::read_to_buffer<int32_t>(RingBuffer<int32_t>*);
// template void I2S::read_to_buffer<uint32_t>(RingBuffer<uint32_t>*);
// template void I2S::read_to_buffer<int64_t>(RingBuffer<int64_t>*);
// template void I2S::read_to_buffer<uint64_t>(RingBuffer<uint64_t>*);

// template void I2S::write_from_buffer<float>(RingBuffer<float>*);
// template void I2S::write_from_buffer<double>(RingBuffer<double>*);
// template void I2S::write_from_buffer<long double>(RingBuffer<long double>*);
// template void I2S::write_from_buffer<int8_t>(RingBuffer<int8_t>*);
// template void I2S::write_from_buffer<uint8_t>(RingBuffer<uint8_t>*);
// template void I2S::write_from_buffer<int16_t>(RingBuffer<int16_t>*);
// template void I2S::write_from_buffer<uint16_t>(RingBuffer<uint16_t>*);
// template void I2S::write_from_buffer<int32_t>(RingBuffer<int32_t>*);
// template void I2S::write_from_buffer<uint32_t>(RingBuffer<uint32_t>*);
// template void I2S::write_from_buffer<int64_t>(RingBuffer<int64_t>*);
// template void I2S::write_from_buffer<uint64_t>(RingBuffer<uint64_t>*);
