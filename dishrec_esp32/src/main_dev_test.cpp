

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
    ixml.set_family_name();
    ixml.set_total_files(1); // File is not one mono stem of multple concurrent recorded files
    ixml.set_tape("tape name... set tape name to date?");
    ixml.set_note("this is a note for this take");
    ixml.set_circled(true); // Circle this take
    ixml.set_rehearsal(true);
    ixml.set_no_good(true);
    ixml.set_ubits(0xde, 0xad, 0xbe, 0xef);

    size_t metaBuffSize = ixml.total_size();
    uint8_t metaBuff[metaBuffSize];
    ixml.copy_to_buffer(metaBuff);

    ixml.set_file_uid();
    
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

    printf("IXML Total Size: %lu\n", ixml.total_size());

    std::cout << std::endl << std::endl;

    return 0;
}


/*
69 58 4d 4c ca 0c 00 00 0a 3c 3f 78 6d 6c 20 76 65 72 73 69 6f 6e 3d 22 31 2e 30 22 20 65 6e 63 6f 64 69 6e 67 3d 22 55 54 46 2d 38 22 3f 3e 0a 3c 42 57 46 58 4d 4c 3e 0a 20 20 20 20 3c 49 58 4d 4c 5f 56 45 52 53 49 4f 4e 3e 32 2e 31 30 3c 2f 49 58 4d 4c 5f 56 45 52 53 49 4f 4e 3e 0a 20 20 20 20 3c 50 52 4f 4a 45 43 54 3e 64 69 73 68 72 65 63 20 74 65 73 74 20 70 72 6f 6a 65 63 74 3c 2f 50 52 4f 4a 45 43 54 3e 0a 20 20 20 20 3c 54 41 50 45 3e 74 61 70 65 20 6e 61 6d 65 2e 2e 2e 20 73 65 74 20 74 61 70 65 20 6e 61 6d 65 20 74 6f 20 64 61 74 65 3f 3c 2f 54 41 50 45 3e 0a 20 20 20 20 3c 53 43 45 4e 45 3e 31 30 31 41 3c 2f 53 43 45 4e 45 3e 0a 20 20 20 20 3c 54 41 4b 45 3e 30 30 31 3c 2f 54 41 4b 45 3e 0a 20 20 20 20 3c 43 49 52 43 4c 45 44 3e 74 72 75 65 3c 2f 43 49 52 43 4c 45 44 3e 0a 20 20 20 20 3c 46 49 4c 45 5f 55 49 44 3e 47 5a 55 53 48 42 38 4d 45 56 36 30 48 4c 57 39 45 57 33 4b 44 55 33 4c 45 59 48 34 5a 35 51 4c 3c 2f 46 49 4c 45 5f 55 49 44 3e 0a 20 20 20 20 3c 55 42 49 54 53 3e 64 65 61 64 62 65 65 66 3c 2f 55 42 49 54 53 3e 0a 20 20 20 20 3c 4e 4f 54 45 3e 74 68 69 73 20 69 73 20 61 20 6e 6f 74 65 20 66 6f 72 20 74 68 69 73 20 74 61 6b 65 3c 2f 4e 4f 54 45 3e 0a 20 20 20 20 3c 54 41 4b 45 5f 54 59 50 45 3e 4e 4f 5f 47 4f 4f 44 2c 52 45 48 45 41 52 53 41 4c 3c 2f 54 41 4b 45 5f 54 59 50 45 3e 0a 20 20 20 20 3c 53 50 45 45 44 3e 0a 20 20 20 20 20 20 20 20 3c 4e 4f 54 45 3e 3c 2f 4e 4f 54 45 3e 0a 20 20 20 20 20 20 20 20 3c 4d 41 53 54 45 52 5f 53 50 45 45 44 3e 32 34 30 30 30 2f 31 30 30 31 3c 2f 4d 41 53 54 45 52 5f 53 50 45 45 44 3e 0a 20 20 20 20 20 20 20 20 3c 43 55 52 52 45 4e 54 5f 53 50 45 45 44 3e 32 34 30 30 30 2f 31 30 30 31 3c 2f 43 55 52 52 45 4e 54 5f 53 50 45 45 44 3e 0a 20 20 20 20 20 20 20 20 3c 54 49 4d 45 43 4f 44 45 5f 52 41 54 45 3e 32 34 30 30 30 2f 31 30 30 31 3c 2f 54 49 4d 45 43 4f 44 45 5f 52 41 54 45 3e 0a 20 20 20 20 20 20 20 20 3c 54 49 4d 45 43 4f 44 45 5f 46 4c 41 47 3e 4e 44 46 3c 2f 54 49 4d 45 43 4f 44 45 5f 46 4c 41 47 3e 0a 20 20 20 20 20 20 20 20 3c 46 49 4c 45 5f 53 41 4d 50 4c 45 5f 52 41 54 45 3e 34 38 30 30 30 3c 2f 46 49 4c 45 5f 53 41 4d 50 4c 45 5f 52 41 54 45 3e 0a 20 20 20 20 20 20 20 20 3c 41 55 44 49 4f 5f 42 49 54 5f 44 45 50 54 48 3e 31 36 3c 2f 41 55 44 49 4f 5f 42 49 54 5f 44 45 50 54 48 3e 0a 20 20 20 20 20 20 20 20 3c 44 49 47 49 54 49 5a 45 52 5f 53 41 4d 50 4c 45 5f 52 41 54 45 3e 34 38 30 30 30 3c 2f 44 49 47 49 54 49 5a 45 52 5f 53 41 4d 50 4c 45 5f 52 41 54 45 3e 0a 20 20 20 20 20 20 20 20 3c 54 49 4d 45 53 54 41 4d 50 5f 53 41 4d 50 4c 45 5f 53 49 4e 43 45 5f 4d 49 44 4e 49 47 48 54 5f 48 49 3e 30 3c 2f 54 49 4d 45 53 54 41 4d 50 5f 53 41 4d 50 4c 45 5f 53 49 4e 43 45 5f 4d 49 44 4e 49 47 48 54 5f 48 49 3e 0a 20 20 20 20 20 20 20 20 3c 54 49 4d 45 53 54 41 4d 50 5f 53 41 4d 50 4c 45 5f 53 49 4e 43 45 5f 4d 49 44 4e 49 47 48 54 5f 4c 4f 3e 30 3c 2f 54 49 4d 45 53 54 41 4d 50 5f 53 41 4d 50 4c 45 5f 53 49 4e 43 45 5f 4d 49 44 4e 49 47 48 54 5f 4c 4f 3e 0a 20 20 20 20 20 20 20 20 3c 54 49 4d 45 53 54 41 4d 50 5f 53 41 4d 50 4c 45 5f 52 41 54 45 3e 34 38 30 30 30 3c 2f 54 49 4d 45 53 54 41 4d 50 5f 53 41 4d 50 4c 45 5f 52 41 54 45 3e 0a 20 20 20 20 3c 2f 53 50 45 45 44 3e 0a 20 20 20 20 3c 48 49 53 54 4f 52 59 3e 0a 20 20 20 20 20 20 20 20 3c 4f 52 49 47 49 4e 41 4c 5f 46 49 4c 45 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 43 55 52 52 45 4e 54 5f 46 49 4c 45 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 41 52 45 4e 54 5f 46 49 4c 45 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 41 52 45 4e 54 5f 55 49 44 2f 3e 0a 20 20 20 20 3c 2f 48 49 53 54 4f 52 59 3e 0a 20 20 20 20 3c 46 49 4c 45 5f 53 45 54 3e 0a 20 20 20 20 20 20 20 20 3c 54 4f 54 41 4c 5f 46 49 4c 45 53 2f 3e 0a 20 20 20 20 20 20 20 20 3c 46 41 4d 49 4c 59 5f 55 49 44 3e 36 4a 4b 46 37 5a 4b 34 58 58 5a 55 46 4d 46 42 32 47 59 37 56 43 51 4b 4b 44 36 39 4b 54 5a 57 3c 2f 46 41 4d 49 4c 59 5f 55 49 44 3e 0a 20 20 20 20 20 20 20 20 3c 46 41 4d 49 4c 59 5f 4e 41 4d 45 3e 31 30 31 41 2f 30 30 31 3c 2f 46 41 4d 49 4c 59 5f 4e 41 4d 45 3e 0a 20 20 20 20 20 20 20 20 3c 46 49 4c 45 5f 53 45 54 5f 49 4e 44 45 58 2f 3e 0a 20 20 20 20 3c 2f 46 49 4c 45 5f 53 45 54 3e 0a 20 20 20 20 3c 54 52 41 43 4b 5f 4c 49 53 54 3e 0a 20 20 20 20 20 20 20 20 3c 54 52 41 43 4b 5f 43 4f 55 4e 54 2f 3e 0a 20 20 20 20 3c 2f 54 52 41 43 4b 5f 4c 49 53 54 3e 0a 20 20 20 20 3c 42 45 58 54 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 44 45 53 43 52 49 50 54 49 4f 4e 3e 61 6e 20 69 6e 66 6f 72 6d 61 74 69 6f 6e 61 6c 20 73 74 72 69 6e 67 3c 2f 42 57 46 5f 44 45 53 43 52 49 50 54 49 4f 4e 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4f 52 49 47 49 4e 41 54 4f 52 3e 64 69 73 68 72 65 63 3c 2f 42 57 46 5f 4f 52 49 47 49 4e 41 54 4f 52 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4f 52 49 47 49 4e 41 54 4f 52 5f 52 45 46 45 52 45 4e 43 45 3e 73 6f 6d 65 20 72 65 66 65 72 65 6e 63 65 20 74 68 69 6e 67 3c 2f 42 57 46 5f 4f 52 49 47 49 4e 41 54 4f 52 5f 52 45 46 45 52 45 4e 43 45 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4f 52 49 47 49 4e 41 54 49 4f 4e 5f 44 41 54 45 3e 32 30 32 30 2d 30 39 2d 33 30 3c 2f 42 57 46 5f 4f 52 49 47 49 4e 41 54 49 4f 4e 5f 44 41 54 45 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4f 52 49 47 49 4e 41 54 49 4f 4e 5f 54 49 4d 45 3e 32 30 3a 34 39 3a 30 39 3c 2f 42 57 46 5f 4f 52 49 47 49 4e 41 54 49 4f 4e 5f 54 49 4d 45 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 54 49 4d 45 5f 52 45 46 45 52 45 4e 43 45 5f 4c 4f 57 3e 30 3c 2f 42 57 46 5f 54 49 4d 45 5f 52 45 46 45 52 45 4e 43 45 5f 4c 4f 57 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 54 49 4d 45 5f 52 45 46 45 52 45 4e 43 45 5f 48 49 47 48 3e 30 3c 2f 42 57 46 5f 54 49 4d 45 5f 52 45 46 45 52 45 4e 43 45 5f 48 49 47 48 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 56 45 52 53 49 4f 4e 3e 30 3c 2f 42 57 46 5f 56 45 52 53 49 4f 4e 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 55 4d 49 44 3e 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 3c 2f 42 57 46 5f 55 4d 49 44 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 52 45 53 45 52 56 45 44 3e 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 3c 2f 42 57 46 5f 52 45 53 45 52 56 45 44 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 43 4f 44 49 4e 47 5f 48 49 53 54 4f 52 59 3e 41 3d 50 43 4d 2c 46 3d 34 38 30 30 30 2c 57 3d 31 36 2c 4d 3d 6d 6f 6e 6f 2c 54 3d 68 65 6c 6c 6f 20 77 6f 72 6c 64 2c 0a 3c 2f 42 57 46 5f 43 4f 44 49 4e 47 5f 48 49 53 54 4f 52 59 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4c 4f 55 44 4e 45 53 53 5f 56 41 4c 55 45 3e 3c 2f 42 57 46 5f 4c 4f 55 44 4e 45 53 53 5f 56 41 4c 55 45 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4c 4f 55 44 4e 45 53 53 5f 52 41 4e 47 45 3e 3c 2f 42 57 46 5f 4c 4f 55 44 4e 45 53 53 5f 52 41 4e 47 45 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4d 41 58 5f 54 52 55 45 5f 50 45 41 4b 5f 4c 45 56 45 4c 3e 3c 2f 42 57 46 5f 4d 41 58 5f 54 52 55 45 5f 50 45 41 4b 5f 4c 45 56 45 4c 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4d 41 58 5f 4d 4f 4d 45 4e 54 41 52 59 5f 4c 4f 55 44 4e 45 53 53 3e 3c 2f 42 57 46 5f 4d 41 58 5f 4d 4f 4d 45 4e 54 41 52 59 5f 4c 4f 55 44 4e 45 53 53 3e 0a 20 20 20 20 20 20 20 20 3c 42 57 46 5f 4d 41 58 5f 53 48 4f 52 54 5f 54 45 52 4d 5f 4c 4f 55 44 4e 45 53 53 3e 3c 2f 42 57 46 5f 4d 41 58 5f 53 48 4f 52 54 5f 54 45 52 4d 5f 4c 4f 55 44 4e 45 53 53 3e 0a 20 20 20 20 3c 2f 42 45 58 54 3e 0a 20 20 20 20 3c 53 59 4e 43 5f 50 4f 49 4e 54 5f 4c 49 53 54 3e 0a 20 20 20 20 20 20 20 20 3c 53 59 4e 43 5f 50 4f 49 4e 54 5f 43 4f 55 4e 54 2f 3e 0a 20 20 20 20 3c 2f 53 59 4e 43 5f 50 4f 49 4e 54 5f 4c 49 53 54 3e 0a 20 20 20 20 3c 4c 4f 43 41 54 49 4f 4e 3e 0a 20 20 20 20 20 20 20 20 3c 4c 4f 43 41 54 49 4f 4e 5f 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 4c 4f 43 41 54 49 4f 4e 5f 47 50 53 2f 3e 0a 20 20 20 20 20 20 20 20 3c 4c 4f 43 41 54 49 4f 4e 5f 41 4c 54 49 54 55 44 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 4c 4f 43 41 54 49 4f 4e 5f 54 59 50 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 4c 4f 43 41 54 49 4f 4e 5f 54 59 50 45 2f 3e 0a 20 20 20 20 3c 2f 4c 4f 43 41 54 49 4f 4e 3e 0a 20 20 20 20 3c 55 53 45 52 3e 0a 20 20 20 20 20 20 20 20 3c 46 55 4c 4c 5f 54 49 54 4c 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 44 49 52 45 43 54 4f 52 5f 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 52 4f 44 55 43 54 49 4f 4e 5f 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 52 4f 44 55 43 54 49 4f 4e 5f 41 44 44 52 45 53 53 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 52 4f 44 55 43 54 49 4f 4e 5f 45 4d 41 49 4c 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 52 4f 44 55 43 54 49 4f 4e 5f 50 48 4f 4e 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 50 52 4f 44 55 43 54 49 4f 4e 5f 4e 4f 54 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 53 4f 55 4e 44 5f 4d 49 58 45 52 5f 4e 41 4d 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 53 4f 55 4e 44 5f 4d 49 58 45 52 5f 41 44 44 52 45 53 53 2f 3e 0a 20 20 20 20 20 20 20 20 3c 53 4f 55 4e 44 5f 4d 49 58 45 52 5f 45 4d 41 49 4c 2f 3e 0a 20 20 20 20 20 20 20 20 3c 53 4f 55 4e 44 5f 4d 49 58 45 52 5f 50 48 4f 4e 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 53 4f 55 4e 44 5f 4d 49 58 45 52 5f 4e 4f 54 45 2f 3e 0a 20 20 20 20 20 20 20 20 3c 41 55 44 49 4f 5f 52 45 43 4f 52 44 45 52 5f 4d 4f 44 45 4c 2f 3e 0a 20 20 20 20 20 20 20 20 3c 41 55 44 49 4f 5f 52 45 43 4f 52 44 45 52 5f 53 45 52 49 41 4c 5f 4e 55 4d 42 45 52 2f 3e 0a 20 20 20 20 20 20 20 20 3c 41 55 44 49 4f 5f 52 45 43 4f 52 44 45 52 5f 46 49 52 4d 57 41 52 45 2f 3e 0a 20 20 20 20 3c 2f 55 53 45 52 3e 0a 3c 2f 42 57 46 58 4d 4c 3e 0a 00 

iXML�

<?xml version="1.0" encoding="UTF-8"?>
<BWFXML>
    <IXML_VERSION>2.10</IXML_VERSION>
    <PROJECT>dishrec test project</PROJECT>
    <TAPE>tape name... set tape name to date?</TAPE>
    <SCENE>101A</SCENE>
    <TAKE>001</TAKE>
    <CIRCLED>true</CIRCLED>
    <FILE_UID>GZUSHB8MEV60HLW9EW3KDU3LEYH4Z5QL</FILE_UID>
    <UBITS>deadbeef</UBITS>
    <NOTE>this is a note for this take</NOTE>
    <TAKE_TYPE>NO_GOOD,REHEARSAL</TAKE_TYPE>
    <SPEED>
        <NOTE></NOTE>
        <MASTER_SPEED>24000/1001</MASTER_SPEED>
        <CURRENT_SPEED>24000/1001</CURRENT_SPEED>
        <TIMECODE_RATE>24000/1001</TIMECODE_RATE>
        <TIMECODE_FLAG>NDF</TIMECODE_FLAG>
        <FILE_SAMPLE_RATE>48000</FILE_SAMPLE_RATE>
        <AUDIO_BIT_DEPTH>16</AUDIO_BIT_DEPTH>
        <DIGITIZER_SAMPLE_RATE>48000</DIGITIZER_SAMPLE_RATE>
        <TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_HI>0</TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_HI>
        <TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_LO>0</TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_LO>
        <TIMESTAMP_SAMPLE_RATE>48000</TIMESTAMP_SAMPLE_RATE>
    </SPEED>
    <HISTORY>
        <ORIGINAL_FILENAME/>
        <CURRENT_FILENAME/>
        <PARENT_FILENAME/>
        <PARENT_UID/>
    </HISTORY>
    <FILE_SET>
        <TOTAL_FILES/>
        <FAMILY_UID>6JKF7ZK4XXZUFMFB2GY7VCQKKD69KTZW</FAMILY_UID>
        <FAMILY_NAME>101A/001</FAMILY_NAME>
        <FILE_SET_INDEX/>
    </FILE_SET>
    <TRACK_LIST>
        <TRACK_COUNT/>
    </TRACK_LIST>
    <BEXT>
        <BWF_DESCRIPTION>an informational string</BWF_DESCRIPTION>
        <BWF_ORIGINATOR>dishrec</BWF_ORIGINATOR>
        <BWF_ORIGINATOR_REFERENCE>some reference thing</BWF_ORIGINATOR_REFERENCE>
        <BWF_ORIGINATION_DATE>2020-09-30</BWF_ORIGINATION_DATE>
        <BWF_ORIGINATION_TIME>20:49:09</BWF_ORIGINATION_TIME>
        <BWF_TIME_REFERENCE_LOW>0</BWF_TIME_REFERENCE_LOW>
        <BWF_TIME_REFERENCE_HIGH>0</BWF_TIME_REFERENCE_HIGH>
        <BWF_VERSION>0</BWF_VERSION>
        <BWF_UMID>01234567890123456789012345678901</BWF_UMID>
        <BWF_RESERVED>000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000</BWF_RESERVED>
        <BWF_CODING_HISTORY>A=PCM,F=48000,W=16,M=mono,T=hello world,
</BWF_CODING_HISTORY>
        <BWF_LOUDNESS_VALUE></BWF_LOUDNESS_VALUE>
        <BWF_LOUDNESS_RANGE></BWF_LOUDNESS_RANGE>
        <BWF_MAX_TRUE_PEAK_LEVEL></BWF_MAX_TRUE_PEAK_LEVEL>
        <BWF_MAX_MOMENTARY_LOUDNESS></BWF_MAX_MOMENTARY_LOUDNESS>
        <BWF_MAX_SHORT_TERM_LOUDNESS></BWF_MAX_SHORT_TERM_LOUDNESS>
    </BEXT>
    <SYNC_POINT_LIST>
        <SYNC_POINT_COUNT/>
    </SYNC_POINT_LIST>
    <LOCATION>
        <LOCATION_NAME/>
        <LOCATION_GPS/>
        <LOCATION_ALTITUDE/>
        <LOCATION_TYPE/>
        <LOCATION_TYPE/>
    </LOCATION>
    <USER>
        <FULL_TITLE/>
        <DIRECTOR_NAME/>
        <PRODUCTION_NAME/>
        <PRODUCTION_ADDRESS/>
        <PRODUCTION_EMAIL/>
        <PRODUCTION_PHONE/>
        <PRODUCTION_NOTE/>
        <SOUND_MIXER_NAME/>
        <SOUND_MIXER_ADDRESS/>
        <SOUND_MIXER_EMAIL/>
        <SOUND_MIXER_PHONE/>
        <SOUND_MIXER_NOTE/>
        <AUDIO_RECORDER_MODEL/>
        <AUDIO_RECORDER_SERIAL_NUMBER/>
        <AUDIO_RECORDER_FIRMWARE/>
    </USER>
</BWFXML>


IXML Total Size: 3322


*/