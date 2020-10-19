

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cstring>
#include <map>

#include "WavHeader.h"
#include "BEXTChunk.h"
#include "iXML.h"
#include "AudioDataPad.h"


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
    iXML::IXML ixml;
    BEXT::BEXTChunk bext;
    BEXT::CodingHistoryRow row;
    row.set_pcm();
    row.set_sample_rate(48000);
    row.set_bit_depth(16);
    row.set_mono();
    row.set_text("hello world");

    bext.set_coding_history(row);

    const char* descrip = "an informational string";
    const char* orig = "dishrec";
    const char* origref = "some reference thing";
    bext.set_description(descrip);
    bext.set_originator(orig);
    bext.set_originator_reference(origref);

    bext.set_date(2020, 9, 30);
    bext.set_time(20, 49, 9);
    bext.set_umid((uint8_t*)"01234567890123456789012345678901");
    
    WavParameters params;
    params.sampleRate = 48000;
    params.bitDepth = 16;
    params.isFloatingPoint = false;
    params.numChannels = 1;
    
    int wavhsize = 44;
    uint8_t wavhbuff[wavhsize];
    WavHeader wavheader;
    wavheader.set_format(params);
    wavheader.set_data_size(0);
    wavheader.copy_to_buffer(wavhbuff);

    ixml.set_framerate(23.98, false);
    ixml.set_sample_rate(48000);
    ixml.set_bit_depth(16);
    ixml.set_timecode(16, 14, 34, 0);

    ixml.import_bext_chunk(bext);
    ixml.set_reserved();

    ixml.set_project("dishrec test project");
    ixml.set_scene("101A");
    ixml.set_take(1);
    
    std::shared_ptr<iXML::Track> trackOne = ixml.create_track();
    trackOne->set_name("BOOM1");
    trackOne->set_function("M/S MID");

    std::shared_ptr<iXML::Track> trackTwo = ixml.create_track();
    trackTwo->set_name("BOOM2");
    trackTwo->set_function("M/S SIDE");

    const uint16_t t1idx = trackOne->get_channel_index();
    ixml.disable_track(trackOne);
    
    trackOne = ixml.get_track(t1idx);
    ixml.enable_track(trackOne);
    ixml.disable_track(t1idx);
    ixml.enable_track(t1idx);
    ixml.enable_track(trackOne);

    ixml.set_tape("tape name... set tape name to date?");
    ixml.set_note("this is a note for this take");
    ixml.set_circled(true); // Circle this take
    ixml.set_rehearsal(true);
    ixml.set_no_good(true);
    ixml.set_ubits(0xde, 0xad, 0xbe, 0xef);

    ixml.set_filename("101AT001");
    ixml.set_family_name();
    ixml.set_total_files(1); // File is not one mono stem of multple concurrent recorded files
    ixml.set_file_uid();
    ixml.set_parent_uid("thiscanbethefileuidforthefilethisfilewasderivedfromoranythingelse_YYYYMMDD_00123423");

    ixml.create_sync_point(1001);
    std::shared_ptr<iXML::SyncPoint> asyncpoint = ixml.create_sync_point(77);
    ixml.create_sync_point(6546351);
    ixml.create_sync_point(0);

    ixml.destroy_sync_point(asyncpoint);
    asyncpoint = nullptr;
    ixml.destroy_sync_point(asyncpoint);

    const size_t metaBuffSize = ixml.total_size();
    uint8_t metaBuff[metaBuffSize];
    ixml.copy_to_buffer(metaBuff);

    print(metaBuff, metaBuffSize);

    printf("\nIXML Total Size: %lu\n", ixml.total_size());

    std::cout << std::endl << std::endl;
}
