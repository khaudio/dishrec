

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cstring>
#include <exception>
#include <map>

#include "AudioUtils.h"
#include "WavHeader.h"
#include "BEXTChunk.h"
#include "iXML.h"
#include "AudioDataPad.h"
#include "BWFHeader.h"
#include "Timer.h"
#include "int_audio.h"
#include "RingBuffer.h"
#include "WavWriter.h"
#include "EspSD.h"
#include "Esp32Button.h"

#ifdef ESP32
#include "EspI2S.h"
#include "WDT.h"
#endif

int main_dev_test()
{
    int i(0);
    std::cout << "Working... " << ++i << std::endl;

    std::cout << "Mounting SD card" << std::endl;

    EspSD::SDCard card;
    WavFile::WavWriter wav;

    std::cout << "Creating file" << std::endl;
    wav.set_directory("");
    wav.set_directory("/");
    wav.set_directory("");
    wav.open("wavfiletest");
    wav.set_directory("/");
    wav.set_directory("");
    std::cout << "File created" << std::endl;

    BEXT::CodingHistoryRow row;
    row.set_pcm();
    row.set_sample_rate(48000);
    row.set_bit_depth(24);
    row.set_mono();
    row.set_text("hello world");
    row.set_text("1ch");
    wav.set_coding_history(row);

    const char* descrip = "an informational string";
    const char* orig = "dishrec";
    // const char* origref = "some reference thing";
    wav.set_description(descrip);
    wav.set_originator(orig);
    // wav.set_originator_reference(origref);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_date(2020, 9, 30);
    wav.set_time(20, 49, 9);
    // wav.set_umid((uint8_t*)"01234567890123456789012345678901");
    
    std::cout << "Working... " << ++i << std::endl;

    WavMeta::WavFormat params;
    params.set_sample_rate(48000);
    params.set_bit_depth(24);
    params.set_format_code(WavMeta::FORMAT_PCM);
    params.set_channels(1);

    wav.set_format(params);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_format(params);
    wav.set_data_size(0);

    std::cout << "Working... " << ++i << std::endl;
    
    wav.set_framerate(23.98);
    std::cout << "Setting dropframe" << std::endl;
    wav.set_dropframe(false);
    // std::cout << "Setting sample rate" << std::endl;
    // wav.set_sample_rate(48000);
    // std::cout << "Setting bit depth" << std::endl;
    // wav.set_bit_depth(24);
    std::cout << "Setting timecode" << std::endl;
    wav.set_timecode(16, 14, 34, 0);
    
    std::cout << "ssm: " << wav.samplesSinceMidnight << std::endl;
    std::cout << "ssm low: " << *wav.timeReferenceLow << std::endl;
    std::cout << "ssm high: " << *wav.timeReferenceHigh << std::endl;

    std::cout << "Working... " << ++i << std::endl;

    wav.set_reserved();

    wav.set_project("dishrec test project");
    wav.set_scene("101A");
    wav.set_take(1);
    
    std::cout << "Working... " << ++i << std::endl;

    std::shared_ptr<iXML::Track> trackOne;
    
    try
    {
        trackOne = wav.get_track(1);
    }
    catch(const std::exception& e)
    {
        trackOne = wav.create_track();
    }

    trackOne->set_name("BOOM1");
    trackOne->set_function("M/S MID");
    
    std::cout << "Working... " << ++i << std::endl;

    std::shared_ptr<iXML::Track> trackTwo = wav.create_track();
    trackTwo->set_name("BOOM2");
    trackTwo->set_function("M/S SIDE");
    
    std::cout << "Working... " << ++i << std::endl;

    const uint16_t t1idx = trackOne->get_channel_index();
    wav.disable_track(trackOne);
    
    trackOne = wav.get_track(t1idx);
    wav.enable_track(trackOne);
    wav.disable_track(t1idx);
    wav.enable_track(t1idx);
    wav.enable_track(trackOne);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_channels(5);
    wav.set_channels(1);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_tape("tape name... set tape name to date?");
    wav.set_note("this is a note for this take");
    wav.set_circled(true);
    wav.set_rehearsal(true);
    wav.set_no_good(true);
    wav.set_ubits(0xde, 0xad, 0xbe, 0xef);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_filename("101AT001");
    wav.set_family_name();
    wav.set_total_files(1); // File is not one mono stem of multple concurrent recorded files
    wav.set_file_uid();
    wav.set_file_set_index("A");
    wav.set_parent_uid("thiscanbethefileuidforthefilethisfilewasderivedfromoranythingelse_YYYYMMDD_00123423");
    wav.create_sync_point(1001);
    std::shared_ptr<iXML::SyncPoint> asyncpoint = wav.create_sync_point(77);
    wav.create_sync_point(0);
    wav.destroy_sync_point(asyncpoint);
    asyncpoint = nullptr;
    wav.destroy_sync_point(asyncpoint);

    const size_t length(48);
    std::vector<float>* floatVals = new std::vector<float>;
    std::vector<int_audio>* scaled24 = new std::vector<int_audio>;

    for (size_t i(0); i < length; ++i)
    {
        floatVals->emplace_back(0);
        scaled24->emplace_back(0);
    }

    sine<float>(floatVals, 1000, 48000, nullptr);
    float_to_int<float, int_audio>(scaled24, floatVals);

    delete floatVals;

    wav.set_country_code("US");
    wav.set_org_code("DSR");
    wav.set_audio_recorder_serial_number("DR0135792468");

    wav.set_location_name("a mountain");
    wav.set_location_gps(-37.235623, -21.6930463);
    wav.set_location_altitude(165413);
    wav.set_location_time_evening(true);
    wav.set_location_time_night();
    wav.set_location_time_evening(false);
    wav.set_location_time_evening(true);
    wav.set_interior();
    wav.set_exterior();

    wav.set_sample_rate(48000);
    std::cout << "This file is " << (wav.is_overcranked() ? "" : "not ") << "overcranked" << std::endl;

    wav.set_circled(false);
    std::cout << wav.get_scene() << std::endl;
    wav.set_circled(true);
    std::cout << wav.get_scene() << std::endl;

    // Packing int for I/O
    Packer packer;
    packer.set_bit_depth(24);

    std::vector<int_audio> padded;
    size_t numBytes = length * packer.get_usable_width();
    uint8_t packedInt[numBytes];
    std::memset(packedInt, 0, numBytes);
    for (size_t i(0); i < scaled24->size(); ++i)
    {
        padded.emplace_back(scaled24->at(i));
    }

    packer.pack<int_audio>(packedInt, &padded);

    Buffer::RingBuffer<int_audio> ringbuff(48, 2);
    ringbuff.write(padded, true);
    ringbuff.write(padded, true);

    wav.set_sample_rate(48000);
    wav.set_bit_depth(24);

    for (int i(0); i < 1000; ++i)
    {
        wav.write(packedInt, numBytes);
        #ifdef ESP32
        force_reset_wdt_1();
        #endif
    }

    printf("\nBroadcastWav header total size: %u\n", wav.total_size());

    wav.close();

    std::cout << "Creating button" << std::endl;

    bool buttonTrigger(false);
    Esp32Button::DualActionButton button(27);
    button.set_trigger(&buttonTrigger);
    buttonTrigger = !buttonTrigger;
    std::cout << "Button is " << (button.read() ? "active" : "inactive") << std::endl;

    #ifdef ESP32
    std::cout << "Starting I2S" << std::endl;

    I2SBus bus(0, true, true, true);
    bus.set_sample_rate(48000);
    bus.set_bit_depth(24);
    bus.set_channels(1);
    bus.set_buffer_size(128);
    bus.set_pins(
            GPIO_NUM_32, // Word (LR) Select
            GPIO_NUM_33, // Serial (bit) clock
            GPIO_NUM_39, // Data Input
            GPIO_NUM_25  // Data Output
        );
    bus.config();
    bus.start();

    std::cout << "I2S Started" << std::endl;
    #endif

    #ifdef ESP32
    bus.shutdown();
    #endif

    std::cout << "Done" << std::endl;

    std::cout << std::endl << std::endl;

    return 0;
}

