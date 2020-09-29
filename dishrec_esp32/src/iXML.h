#ifndef IXML_H
#define IXML_H

#include <tinyxml2.h>
#include <string>
#include <iostream>
#include <sstream>
#include "TimecodeBase.h"
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
};

class History : public Base
{
public:
    XMLElement
        *original_filename, *current_filename,
        *parent_filename, *parent_uid;
    History(XMLDocument* xmldoc);
};

class FileSet : public Base
{
public:
    XMLElement
        *total_files, *family_uid,
        *family_name, *file_set_index;
    FileSet(XMLDocument* xmldoc);
};

class Track : public Base
{
public:
    XMLElement
        *channel_index, *interleave_index,
        *name, *function;
    Track(XMLDocument* xmldoc);
    friend class TrackList;
};

class TrackList : public Base
{
public:
    XMLElement *track_count;
    TrackList(XMLDocument* xmldoc);
};

class BEXT : public Base
{
public:
    XMLElement
        *bwf_description, *bwf_originator,
        *bwf_originator_reference,
        *bwf_origination_date, *bwf_origination_time,
        *bwf_time_reference_low,
        *bwf_time_reference_high,
        *bwf_version, *bwf_umid, *bwf_reserved,
        *bwf_coding_history,
        *bwf_loudness_value, *bwf_loudness_range,
        *bwf_max_true_peak_level, *bwf_max_momentary_loudness,
        *bwf_max_short_term_loudness;
    BEXT(XMLDocument* xmldoc);
};

class SyncPoint : public Base
{
public:
    XMLElement
        *sync_point_type, *sync_point_function, *sync_point_comment,
        *sync_point_low, *sync_point_high, *sync_point_event_duration;
    SyncPoint(XMLDocument* xmldoc);
    friend class SyncPointList;
};

class SyncPointList : public Base
{
public:
    XMLElement *sync_point_count;
    SyncPointList(XMLDocument* xmldoc);
};

class Location : public Base
{
public:
    XMLElement
        *location_name, *location_gps, *location_altitude,
        *location_type, *location_time;
    Location(XMLDocument* xmldoc);
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
};

class IXML : public TimecodeBase::Clock
{
protected:
    static const char* _ubits_valid_chars;
    void _set_framerate(const char* fps);
    void _set_ixml_samples_since_midnight();
    void _set_samples_since_midnight() override;
    void _set_samples_since_midnight(uint64_t numSamples) override;
    // virtual void _increment_file_uid();
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
    BEXT bext;
    SyncPointList sync_point_list;
    Location location;
    User user;
    uint16_t numChannels;
    IXML();
    ~IXML();
    virtual void set_default();
    const char* c_str();
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
    // void set_date(int16_t year, uint8_t month, uint8_t day);
    // void set_time(uint8_t hour, uint8_t minute, uint8_t second);
    virtual void set_project(const char* projectName);
    virtual void set_tape(const char* tapeName);
    virtual void set_scene(const char* sceneName);
    virtual void set_take(uint32_t takeNumber);
    virtual void set_circled(bool isCircled);
    virtual void set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);
    virtual void set_ubits(const char* userbits);
    virtual void set_note(const char* message);
};

};

#endif
