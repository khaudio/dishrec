

#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>
#include <cstring>

#include "WavHeader.h"
#include "BEXTChunk.h"
#include "iXML.h"

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
    bext.set_umid((uint8_t*)"01234567890123456789012345678901", 32);

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
    ixml.set_bit_depth(16, false);
    ixml.set_timecode(16, 14, 34, 0);

    ixml.import_bext_chunk(bext);
    ixml.set_reserved();

    ixml.set_project("dishrec test project");
    ixml.set_scene("101A");
    ixml.set_take(1);
    ixml.set_tape("tape name... set date to tape name?");
    ixml.set_note("this is a note for this take");
    ixml.set_circled(true); // Circle this take
    ixml.set_rehearsal(true);
    ixml.set_no_good(true);


    size_t metaBuffSize = bext.total_size();
    uint8_t metaBuff[metaBuffSize];
    bext.copy_to_buffer(metaBuff);
    
    for (size_t i(0); i < metaBuffSize; ++i)
    {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << +metaBuff[i] << " ";
    }
    std::cout << std::endl << std::endl;

    for (size_t i(0); i < metaBuffSize; ++i)
    {
        std::cout << metaBuff[i];
    }
    std::cout << std::endl << std::endl;

    printf("BEXT Total Size: %lu\n", bext.total_size());

    std::cout << std::endl << std::endl;

    return 0;
}


/*



*/