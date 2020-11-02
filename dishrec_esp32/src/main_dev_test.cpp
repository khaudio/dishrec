

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
#include "Loudness.h"
#include "Timer.h"


void print(const uint8_t* buff, size_t buffsize)
{
    for (size_t i(0); i < buffsize; ++i) std::cout << buff[i];
    std::cout << std::endl;
}

void print_hex(const uint8_t* buff, size_t buffsize)
{
    for (size_t i(0); i < buffsize; ++i)
    {
        std::cout << std::setw(2) << std::setfill('0');
        std::cout << std::hex << +buff[i] << " ";
    }
    std::cout << std::endl;
}

int main()
{
    int i(0);
    std::cout << "Working... " << ++i << std::endl;

    BWFHeader::BroadcastWav wav;
    BEXT::CodingHistoryRow row;
    row.set_pcm();
    row.set_sample_rate(48000);
    row.set_bit_depth(16);
    row.set_mono();
    row.set_text("hello world");
    wav.set_coding_history(row);
    const char* descrip = "an informational string";
    const char* orig = "dishrec";
    const char* origref = "some reference thing";
    wav.set_description(descrip);
    wav.set_originator(orig);
    wav.set_originator_reference(origref);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_date(2020, 9, 30);
    wav.set_time(20, 49, 9);
    wav.set_umid((uint8_t*)"01234567890123456789012345678901");
    
    std::cout << "Working... " << ++i << std::endl;

    WavMeta::WavFormat params;
    params.set_sample_rate(48000);
    params.set_bit_depth(16);
    params.set_format_code(WavMeta::FORMAT_PCM);
    params.set_channels(1);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.set_format(params);
    wav.set_data_size(0);
    
    std::cout << "Working... " << ++i << std::endl;
    
    wav.set_framerate(23.98);
    std::cout << "Setting " << "dropframe" << std::endl;
    wav.set_dropframe(false);
    std::cout << "Setting " << "sample rate" << std::endl;
    wav.set_sample_rate(48000);
    std::cout << "Setting " << "bit depth" << std::endl;
    wav.set_bit_depth(16);
    std::cout << "Setting " << "timecode" << std::endl;
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
    
    std::cout << "Working... " << ++i << std::endl;

    wav.create_sync_point(1001);
    std::shared_ptr<iXML::SyncPoint> asyncpoint = wav.create_sync_point(77);
    wav.create_sync_point(6546351);
    wav.create_sync_point(0);
    
    std::cout << "Working... " << ++i << std::endl;

    wav.destroy_sync_point(asyncpoint);
    asyncpoint = nullptr;
    wav.destroy_sync_point(asyncpoint);

    const size_t length(384000);
    std::vector<int16_t> samples;
    std::vector<double> floatVals;
    for (size_t i(0); i < length; ++i)
    {
        floatVals.emplace_back(0);
        samples.emplace_back(0);
    }
    sine<double>(&floatVals, 1000, 48000, nullptr);
    float_to_int<double, int16_t>(&samples, &floatVals);
    int_to_float<int16_t, double>(&floatVals, &samples);

    std::cout << "Adding frames to loudness analyzer" << std::endl;

    wav.add_frames(&floatVals);
    
    std::cout << "Setting loudness" << std::endl;

    wav.set_loudness();
    
    std::cout << "Set loudness " << std::endl;

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

    std::cout << "is " << (wav.is_overcranked() ? "" : "not ") << "overcranked" << std::endl;

    wav.set_circled(false);
    std::cout << wav.get_scene() << std::endl;
    wav.set_circled(true);
    std::cout << wav.get_scene() << std::endl;

    const size_t metaBuffSize = wav.total_size();
    uint8_t metaBuff[metaBuffSize];
    wav.get(metaBuff);
    // print(metaBuff, metaBuffSize);
    printf("\nBroadcastWav Size: %lu\n", wav.size());

    std::cout << std::endl << std::endl;
}
