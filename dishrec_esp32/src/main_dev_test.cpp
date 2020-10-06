/* #include <iostream>
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

    ixml.set_coding_history(row);

    const char* descrip = "an informational string";
    const char* orig = "dishrec";
    const char* origref = "some reference thing";
    ixml.set_description(descrip);
    ixml.set_originator(orig);
    ixml.set_originator_reference(origref);

    ixml.set_date(2020, 9, 30);
    ixml.set_time(20, 49, 9);
    ixml.set_timestamp(480000000);
    ixml.set_umid((uint8_t*)"01234567890123456789012345678901", 32);

    // size_t bufflen = bext.size();
    // uint8_t bextbuff[bufflen];
    // bext.copy_to_buffer(bextbuff);

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

    size_t metaBuffSize = ixml.total_size();
    uint8_t metaBuff[metaBuffSize];
    ixml.copy_to_buffer(metaBuff);
    
    for (int i(0); i < metaBuffSize; ++i)
    {
        std::cout << std::setw(2) << std::setfill('0') << std::hex << +metaBuff[i] << " ";
    }
    std::cout << std::endl << std::endl;

    return 0;
} */


/*
iXMLV	
<?xml version="1.0" encoding="UTF-8"?>
<BWFXML>
    <IXML_VERSION>2.10</IXML_VERSION>
    <PROJECT/>
    <TAPE/>
    <SCENE/>
    <TAKE/>
    <CIRCLED>false</CIRCLED>
    <FILE_UID/>
    <UBITS>00000000</UBITS>
    <NOTE/>
    <TAKE_TYPE>DEFAULT</TAKE_TYPE>
    <SPEED>
        <NOTE/>
        <MASTER_SPEED/>
        <CURRENT_SPEED/>
        <TIMECODE_RATE/>
        <TIMECODE_FLAG/>
        <FILE_SAMPLE_RATE/>
        <AUDIO_BIT_DEPTH/>
        <DIGITIZER_SAMPLE_RATE/>
        <TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_HI/>
        <TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_LO/>
        <TIMESTAMP_SAMPLE_RATE/>
    </SPEED>
    <HISTORY>
        <ORIGINAL_FILENAME/>
        <CURRENT_FILENAME/>
        <PARENT_FILENAME/>
        <PARENT_UID/>
    </HISTORY>
    <FILE_SET>
        <TOTAL_FILES/>
        <FAMILY_UID/>
        <FAMILY_NAME/>
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
        <BWF_TIME_REFERENCE_LOW/>
        <BWF_TIME_REFERENCE_HIGH/>
        <BWF_VERSION>0x0001</BWF_VERSION>
        <BWF_UMID>true</BWF_UMID>
        <BWF_RESERVED>true</BWF_RESERVED>
        <BWF_CODING_HISTORY>A=PCM,F=48000,W=16,M=mono,hello world,
</BWF_CODING_HISTORY>
        <BWF_LOUDNESS_VALUE/>
        <BWF_LOUDNESS_RANGE/>
        <BWF_MAX_TRUE_PEAK_LEVEL/>
        <BWF_MAX_MOMENTARY_LOUDNESS/>
        <BWF_MAX_SHORT_TERM_LOUDNESS/>
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

*/