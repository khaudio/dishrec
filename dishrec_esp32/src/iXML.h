#ifndef IXML_H
#define IXML_H

#include "../lib/tinyxml2/tinyxml2.h"

// #include <tinyxml2.h>
#include <string>
#include <iostream>
#include <sstream>
#include "TimecodeBase.h"
#include "BEXTChunk.h"
#include "ErrorEnums.h"

using namespace tinyxml2;

namespace iXML
{

class Base
{
protected:
    XMLElement* _element;
    XMLDocument* ixml;
    XMLNode* root;
    XMLElement* _set_child_element(const char* elementName);
public:
    Base(XMLDocument* xmldoc, const char* elementName);
    ~Base();
    friend class IXML;
};

class TakeType : public Base
{
protected:
    bool
        _default, _no_good, _false_start, _wild_track,
        _pickup, _rehearsal, _announcement, _sound_guide;
    void _apply();
public:
    TakeType(XMLDocument* xmldoc);
    void set_default(bool);
    void no_good(bool);
    void false_start(bool);
    void wild_track(bool);
    void pickup(bool);
    void rehearsal(bool);
    void announcement(bool);
    void sound_guide(bool);
    friend class IXML;
};

class Speed : public Base
{
public:
    XMLElement
        *note, *master_speed, *current_speed,
        *timecode_rate, *timecode_flag,
        *file_sample_rate, *audio_bit_depth,
        *digitizer_sample_rate,
        *timestamp_samples_since_midnight_hi,
        *timestamp_samples_since_midnight_lo,
        *timestamp_sample_rate;
    Speed(XMLDocument* xmldoc);
    friend class IXML;
};

class History : public Base
{
public:
    XMLElement
        *original_filename, *current_filename,
        *parent_filename, *parent_uid;
    History(XMLDocument* xmldoc);
    friend class IXML;
};

class FileSet : public Base
{
public:
    XMLElement
        *total_files, *family_uid,
        *family_name, *file_set_index;
    FileSet(XMLDocument* xmldoc);
    friend class IXML;
};

class Track : public Base
{
public:
    XMLElement
        *channel_index, *interleave_index,
        *name, *function;
    Track(XMLDocument* xmldoc);
    friend class TrackList;
    friend class IXML;
};

class TrackList : public Base
{
public:
    XMLElement *track_count;
    TrackList(XMLDocument* xmldoc);
    friend class IXML;
};

class BEXTElement : public Base
{
public:
    XMLElement
        *bwf_originator, *bwf_originator_reference, *bwf_description,
        *bwf_origination_date, *bwf_origination_time,
        *bwf_time_reference_low, *bwf_time_reference_high,
        *bwf_version, *bwf_umid,
        *bwf_loudness_value, *bwf_loudness_range,
        *bwf_max_true_peak_level, *bwf_max_momentary_loudness,
        *bwf_max_short_term_loudness,
        *bwf_reserved, *bwf_coding_history;
    BEXTElement(XMLDocument* xmldoc);
    friend class IXML;
};

class SyncPoint : public Base
{
public:
    XMLElement
        *sync_point_type, *sync_point_function, *sync_point_comment,
        *sync_point_low, *sync_point_high, *sync_point_event_duration;
    SyncPoint(XMLDocument* xmldoc);
    friend class SyncPointList;
    friend class IXML;
};

class SyncPointList : public Base
{
public:
    XMLElement *sync_point_count;
    SyncPointList(XMLDocument* xmldoc);
    friend class IXML;
};

class Location : public Base
{
public:
    XMLElement
        *location_name, *location_gps, *location_altitude,
        *location_type, *location_time;
    Location(XMLDocument* xmldoc);
    friend class IXML;
};

class User : public Base
{
public:
    XMLElement
        *full_title, *director_name, *production_name,
        *production_address, *production_email,
        *production_phone, *production_note,
        *sound_mixer_name, *sound_mixer_address,
        *sound_mixer_email, *sound_mixer_phone,
        *sound_mixer_note, *audio_recorder_model,
        *audio_recorder_serial_number,
        *audio_recorder_firmware;
    User(XMLDocument* xmldoc);
    friend class IXML;
};

class IXML : public TimecodeBase::Clock, public BEXT::BEXTChunk
{
protected:
    char _ixmlChunkID[4];
    uint32_t _ixmlChunkSize, _ixmlExportedSize;
    uint16_t _ixmlVersionMajor, _ixmlVersionMinor;
    static const char *_ubits_valid_chars, *_xmlEncoding;
    void _set_framerate(const char* fps);
    void _set_timestamp_ixml();
    void _set_timestamp() override;
    void _set_timestamp(uint64_t numSamples) override;
    // virtual void _increment_file_uid();
    const char* _xml_c_str();
public:
    XMLDocument ixml;
    XMLNode* root;
    XMLElement
        *ixml_version, *project, *tape,
        *scene, *take, *circled,
        *file_uid, *ubits, *note;
    TakeType take_type;
    Speed speed;
    History history;
    FileSet file_set;
    TrackList track_list;
    BEXTElement bext;
    SyncPointList sync_point_list;
    Location location;
    User user;
    uint16_t numChannels;
    IXML();
    ~IXML();
    virtual void set_default();
    virtual void set_ixml_version(uint16_t major, uint16_t minor);
    virtual void set_bit_depth(uint16_t bitsPerSample, bool isFloat);
    void set_sample_rate(uint32_t samplerate) override;
    // virtual void set_channels(uint16_t channels);
    void set_framerate(double fps, bool isDropframe) override;
    void set_framerate(int fps, bool isDropframe) override;
    void set_timecode(int hr, int min, int sec, int frm) override;
    void set_timecode(std::array<int, 4> tc) override;
    void set_timecode(int numFrames) override;
    virtual void set_filename(const char* filename);
    // Track* create_track();
    // SyncPoint* create_sync_point();
    // virtual void set_take_type(TakeType* type);
    virtual void set_project(const char* projectName);
    virtual void set_tape(const char* tapeName);
    virtual void set_scene(const char* sceneName);
    virtual void set_take(uint32_t takeNumber);
    virtual void set_circled(bool isCircled);
    virtual void set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);
    virtual void set_ubits(const char* userbits);
    virtual void set_note(const char* message);
    void set_originator(const char* newOriginator) override;
    void set_originator_reference(const char* newReference) override;
    void set_description(const char* newDescription) override;
    void set_date(int16_t year, uint8_t month, uint8_t day) override;
    void set_time(uint8_t hour, uint8_t minute, uint8_t second) override;
    void set_bwf_version(uint16_t versionNumber) override;
    void set_umid(const uint8_t* newUmid, uint8_t length) override;
    void set_loudness_value(uint16_t value) override;
    void set_loudness_range(uint16_t range) override;
    void set_loudness_max_true_peak(uint16_t level) override;
    void set_loudness_max_momentary(uint16_t level) override;
    void set_loudness_max_short_term(uint16_t value) override;
    void set_reserved() override;
    void set_coding_history(BEXT::CodingHistoryRow row) override;
    void append_to_coding_history(BEXT::CodingHistoryRow row) override;
    virtual void import_bext_chunk(BEXT::BEXTChunk& chunk);
    uint32_t size() override;
    size_t total_size() override;
    void copy_to_buffer(uint8_t* buff) override;
};

};

#endif
