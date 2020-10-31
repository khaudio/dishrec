#ifndef IXML_H
#define IXML_H

#include <string>
#include <map>
#include <vector>

#include <tinyxml2.h>

#include "TimecodeBase.h"
#include "BEXTChunk.h"
#include "ErrorEnums.h"
#include "AudioUtils.h"

#define IXML_VERSION_MAJOR          2
#define IXML_VERSION_MINOR          10

#ifndef MAX_TRACK_STR_LENGTH
#define MAX_TRACK_STR_LENGTH        100
#endif

using namespace tinyxml2;

namespace iXML
{

class Base;
class TakeType;
class Speed;
class History;
class FileSet;
class Track;
class TrackList;
class BEXTElement;
class SyncPoint;
class SyncPointList;
class Location;
class User;
class IXML;

enum ixml_err
{
    TRACK_COUNT_MISMATCH = 120,
    TRACK_NOT_FOUND = 121,
    INVALID_UID_LENGTH = 122,
    SYNC_POINT_TIMESTAMP_NOT_SET = 123,
    SYNC_POINT_NOT_FOUND = 124,
    ITEM_NOT_FOUND = 125
};

void _get_flag_str(
        char* buff,
        bool& initialized,
        bool flag,
        const char* str
    );

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
    void _apply();

public:
    bool
        defaultTake, no_good, false_start, wild_track,
        pickup, rehearsal, announcement, sound_guide;
    TakeType(XMLDocument* xmldoc);
    void set_default();
    void set_no_good(bool flagged);
    void set_false_start(bool flagged);
    void set_wild_track(bool flagged);
    void set_pickup(bool flagged);
    void set_rehearsal(bool flagged);
    void set_announcement(bool flagged);
    void set_sound_guide(bool flagged);
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
        *original_filename, *parent_filename, *parent_uid;
    History(XMLDocument* xmldoc);
    friend class IXML;
};

class LoudnessElement : public Base
{
public:
    XMLElement
        *loudness_value,
        *loudness_range,
        *max_true_peak_level,
        *max_momentary_loudness,
        *max_short_term_loudness;
    LoudnessElement(XMLDocument* xmldoc);
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
protected:
    bool _active;
    uint16_t _index, _interleaveIndex;
    char _name[MAX_TRACK_STR_LENGTH], _function[MAX_TRACK_STR_LENGTH];
public:
    Track(XMLDocument* xmldoc);
    void set_channel_index(uint16_t channelIndex);
    void set_interleave_index(uint16_t interleaveIndex);
    void set_name(const char* trackName);
    void set_function(const char* trackFunction);
    uint16_t get_channel_index();
    const char* get_name() const;
    const char* get_function() const;
    friend class TrackList;
    friend class IXML;
};

class TrackList : public Base
{
public:
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
protected:
    bool _timestampSet;
    uint64_t _timestamp;
    void _set_timestamp();
public:
    XMLElement
        *sync_point_type, *sync_point_function, *sync_point_comment,
        *sync_point_low, *sync_point_high, *sync_point_event_duration;
    SyncPoint(XMLDocument* xmldoc);
    void set_type(const char* syncPointType);
    void set_function(const char* syncPointFunction);
    void set_comment(const char* comment);
    void set_timestamp(uint64_t numSamples);
    void set_timestamp(const TimecodeBase::Clock& clock);
    uint64_t get_timestamp() const;
    void set_duration(uint64_t duration);
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

class IXML : virtual public TimecodeBase::Clock
{
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
    LoudnessElement loudness;
    FileSet file_set;
    TrackList track_list;
    BEXTElement bext;
    SyncPointList sync_point_list;
    Location location;
    User user;

    uint16_t numTracks, numSyncPoints;
    // static const char *_uidValidChars;
    static constexpr const char* _uidValidChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

protected:
    char _ixmlChunkID[4];
    size_t _ixmlChunkSize;
    // static const char *_ubitsValidChars, *_xmlEncoding;
    static constexpr const char* _ubitsValidChars = "0123456789abcdef";
    static constexpr const char* _xmlEncoding = "\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    std::map<const uint16_t, std::shared_ptr<Track>> tracks;
    std::vector<std::shared_ptr<SyncPoint>> syncPoints;
    std::vector<XMLElement*> _trackElements;

public:
    IXML();
    ~IXML();
    
    virtual void clear();

    // Size of iXML chunk
    virtual size_t size();

    // Print XML only to const char*
    const char* _xml_c_str();
    
    // Copy chunk to buffer
    virtual size_t get(uint8_t* buff);

/*                        Top Level Metadata                        */

public:
    // iXML version
    virtual void set_ixml_version(uint16_t major, uint16_t minor);
    virtual std::pair<const uint16_t, const uint16_t> get_ixml_version();

    // Project
    virtual void set_project(const char* projectName);
    virtual const char* get_project();

    // Tape/Sound Roll
    virtual void set_tape(const char* tapeName);
    virtual const char* get_tape();

    // Scene name
    virtual void set_scene(const char* sceneName);
    virtual const char* get_scene();

    // Take number
    virtual void set_take(int takeNum);
    virtual int get_take();

/*                             Take type                            */

public:
    // Circle Take
    virtual void set_circled(bool isCircled);
    virtual bool is_circled();

    // File UID
    virtual void set_file_uid();
    virtual const char* get_file_uid();
    
    // Userbits
    static void _assert_valid_ubits(const char* bits);
    virtual void set_ubits(
            uint8_t first,
            uint8_t second,
            uint8_t third,
            uint8_t fourth
        );
    virtual void set_ubits(const char* bits);
    virtual const uint8_t* get_ubits() const;

    // Take note
    virtual void set_note(const char* message);
    virtual const char* get_note();

    // Take type
    virtual void set_default_take_type();
    virtual bool is_default_take_type();

    virtual void set_no_good(bool flagged);
    virtual bool is_no_good();

    virtual void set_false_start(bool flagged);
    virtual bool is_false_start();

    virtual void set_wild_track(bool flagged);
    virtual bool is_wild_track();

    virtual void set_pickup(bool flagged);
    virtual bool is_pickup();

    virtual void set_rehearsal(bool flagged);
    virtual bool is_rehearsal();

    virtual void set_announcement(bool flagged);
    virtual bool is_announcement();

    virtual void set_sound_guide(bool flagged);
    virtual bool is_sound_guide();


/*                              Speed                               */

protected:
    virtual void _set_framerate_str();
    virtual void _set_timestamp_ixml();
    void _set_timestamp() override;
    void _set_timestamp(uint64_t numSamples) override;
    void _set_timestamp(uint32_t ssmLo, uint32_t ssmHi) override;

public:
    void set_sample_rate(uint32_t samplerate) override;
    void set_bit_depth(uint16_t bitsPerSample) override;
    
    void set_framerate(double fps) override;
    void set_framerate(int fps) override;
    void set_framerate(const char* fps) override;
    void set_dropframe(bool isDropframe) override;

    void set_timecode(int hr, int min, int sec, int frm) override;
    void set_timecode(std::array<int, 4> tc) override;
    void set_timecode(int numFrames) override;
    void set_timecode(TimecodeBase::Base& base) override;
    void set_timecode(TimecodeBase::Clock& clock) override;
    void clear_timecode() override;

/*                            Sync Points                           */

protected:
    static bool _sync_point_sorter(
            std::shared_ptr<SyncPoint> first,
            std::shared_ptr<SyncPoint> second
        );
    size_t _get_sync_point_index(std::shared_ptr<SyncPoint> point);
    void _update_sync_point_count();
    virtual std::shared_ptr<SyncPoint> _create_sync_point(uint64_t numSamples);

public:
    virtual std::shared_ptr<SyncPoint> create_sync_point(uint64_t numSamples);
    virtual std::shared_ptr<SyncPoint> create_sync_point(TimecodeBase::Clock& clock);
    virtual std::shared_ptr<SyncPoint> create_sync_point();
    virtual void destroy_sync_point(std::shared_ptr<SyncPoint> point);
    virtual void clear_sync_points();

/*                              History                             */

    virtual void set_filename(const char* filename);
    virtual const char* get_filename();

    virtual void set_parent_uid(const char* data);
    virtual const char* get_parent_uid();


/*                             Loudness                             */

    virtual void set_loudness_value(double value);
    virtual void set_loudness_range(double range);
    virtual void set_loudness_max_true_peak(double level);
    virtual void set_loudness_max_momentary(double level);
    virtual void set_loudness_max_short_term(double value);
    virtual void clear_loudness();

/*                             File set                             */

    virtual void set_total_files(unsigned int numFiles);
    virtual int get_total_files();
    
    virtual void set_family_uid();
    virtual const char* get_family_uid();
    
    virtual void set_family_name(const char* familyName);
    virtual void set_family_name();
    virtual const char* get_family_name();
    
    virtual void set_file_set_index(const char* index);
    virtual const char* get_file_set_index();

/*                            Track list                            */

protected:
    virtual void _write_track_list();

public:
    void set_channels(uint16_t channels) override;
    virtual uint16_t get_channels();

    virtual std::shared_ptr<Track> create_track();
    virtual std::shared_ptr<Track> get_track(const uint16_t index);

    virtual void destroy_track(std::shared_ptr<Track> track);
    virtual void destroy_track(const uint16_t index);

    virtual void disable_track(std::shared_ptr<Track> track);
    virtual void disable_track(const uint16_t index);
    
    virtual void enable_track(std::shared_ptr<Track> track);
    virtual void enable_track(const uint16_t index);

/*                               BEXT                               */

public:
    // Originator
    virtual void set_originator(const char* newOriginator);
    virtual void clear_originator();

    // Originator Reference
    virtual void set_originator_reference(const char* newReference);
    virtual void clear_originator_reference();

    // Description
    virtual void set_description(const char* newDescription);
    virtual void clear_description();

    // Origination Date
    // virtual void set_date(int16_t year, uint8_t month, uint8_t day);
    virtual void set_date_str(const char* date);
    virtual void clear_date();

    // Origination Time
    // virtual void set_time(uint8_t hour, uint8_t minute, uint8_t second);
    virtual void set_time_str(const char* time);
    virtual void clear_time();

    // BWF Version
    virtual void set_bwf_version(uint16_t versionNumber);

    // UMID
    virtual void set_umid(const uint8_t* newUmid);
    virtual void clear_umid();

    // Reserved
    virtual void set_reserved(const uint8_t* data);

    // Coding History
    virtual void set_coding_history(std::string history);
    virtual void set_coding_history(const char* history);
    // virtual void set_coding_history(BEXT::CodingHistoryRow row);
    // virtual void append_to_coding_history(BEXT::CodingHistoryRow row);
    virtual void clear_coding_history();

    // Import data from external BEXT Chunk
    virtual void import_bext_chunk(BEXT::BEXTChunk& chunk);

/*                             Location                             */

protected:
    bool
        _isInterior, _isExterior,
        _isTimeSunrise, _isTimeMorning, _isTimeMidday,
        _isTimeDay, _isTimeAfternoon, _isTimeEvening,
        _isTimeSunset, _isTimeNight;

public:
    virtual void set_location_name(const char* text);
    virtual const char* get_location_name();

    virtual void set_location_gps(double longitude, double latitude);
    virtual const char* get_location_gps();

    virtual void set_location_altitude(int altitude);
    virtual const char* get_location_altitude();
    
protected:
    virtual void _set_location_type();
    virtual void _set_location_time();

public:
    virtual void set_interior(bool flag);
    virtual bool is_interior();

    virtual void set_exterior(bool flag);
    virtual bool is_exterior();

    virtual void set_location_time_sunrise(bool flag);
    virtual bool get_location_time_sunrise();

    virtual void set_location_time_morning(bool flag);
    virtual bool get_location_time_morning();

    virtual void set_location_time_midday(bool flag);
    virtual bool get_location_time_midday();

    virtual void set_location_time_day(bool flag);
    virtual bool get_location_time_day();

    virtual void set_location_time_afternoon(bool flag);
    virtual bool get_location_time_afternoon();

    virtual void set_location_time_evening(bool flag);
    virtual bool get_location_time_evening();

    virtual void set_location_time_sunset(bool flag);
    virtual bool get_location_time_sunset();

    virtual void set_location_time_night(bool flag);
    virtual bool get_location_time_night();

    virtual const char* get_location_type();
    virtual const char* get_location_time();

/*                               User                               */

public:
    virtual void set_full_title(const char* text);
    virtual const char* get_full_title();
    
    virtual void set_director_name(const char* text);
    virtual const char* get_director_name();
    
    virtual void set_production_name(const char* text);
    virtual const char* get_production_name();
    
    virtual void set_production_address(const char* text);
    virtual const char* get_production_address();
    
    virtual void set_production_email(const char* text);
    virtual const char* get_production_email();
    
    virtual void set_production_phone(const char* text);
    virtual const char* get_production_phone();
    
    virtual void set_production_note(const char* text);
    virtual const char* get_production_note();
    
    virtual void set_sound_mixer_name(const char* text);
    virtual const char* get_sound_mixer_name();
    
    virtual void set_sound_mixer_address(const char* text);
    virtual const char* get_sound_mixer_address();
    
    virtual void set_sound_mixer_email(const char* text);
    virtual const char* get_sound_mixer_email();
    
    virtual void set_sound_mixer_phone(const char* text);
    virtual const char* get_sound_mixer_phone();
    
    virtual void set_sound_mixer_note(const char* text);
    virtual const char* get_sound_mixer_note();
    
    virtual void set_audio_recorder_model(const char* text);
    virtual const char* get_audio_recorder_model();
    
    virtual void set_audio_recorder_serial_number(const char* text);
    virtual const char* get_audio_recorder_serial_number();
    
    virtual void set_audio_recorder_firmware(const char* text);
    virtual const char* get_audio_recorder_firmware();

};

};

#endif
