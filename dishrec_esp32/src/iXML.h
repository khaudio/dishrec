#ifndef IXML_H
#define IXML_H

#ifdef ESP32
#include <tinyxml2.h>
#else
#include "../lib/tinyxml2/tinyxml2.h"
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include "TimecodeBase.h"
#include "BEXTChunk.h"
#include "ErrorEnums.h"

using namespace tinyxml2;

namespace iXML
{

#ifndef MAX_TRACK_STR_LENGTH
#define MAX_TRACK_STR_LENGTH        100
#endif

enum ixml_err
{
    TRACK_COUNT_MISMATCH = 120,
    TRACK_NOT_FOUND = 121,
    INVALID_UID_LENGTH = 122,
    SYNC_POINT_TIMESTAMP_NOT_SET = 123,
    SYNC_POINT_NOT_FOUND = 124,
    ITEM_NOT_FOUND = 125,
};

void get_random_str(char* buff, uint32_t length);
void get_random_str(char* buff, uint32_t length, unsigned int seed);

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

class IXML : public TimecodeBase::Clock, public BEXT::BEXTChunk
{
protected:
    char _ixmlChunkID[4];
    uint32_t _ixmlChunkSize, _ixmlExportedSize;
    uint16_t _ixmlVersionMajor, _ixmlVersionMinor;
    static const char *_ubitsValidChars, *_xmlEncoding;
    char _fileUID[32], _familyUID[32], _parentUID[32];
    std::map<const uint16_t, std::shared_ptr<Track>> tracks;
    std::vector<std::shared_ptr<SyncPoint>> syncPoints;
    std::vector<XMLElement*> _trackElements;

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
    uint16_t numTracks, numSyncPoints;
    int takeNumber;
    static const char *_uidValidChars;

    IXML();
    ~IXML();
    void clear() override;

/*                        Top Level Metadata                        */
    virtual void set_ixml_version(uint16_t major, uint16_t minor);
    virtual void set_project(const char* projectName);
    virtual void set_tape(const char* tapeName);
    virtual void set_scene(const char* sceneName);
    virtual void set_take(int takeNum);
    virtual void set_circled(bool isCircled);
    virtual void set_file_uid();
    virtual void set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);
    virtual void set_ubits(const char* userbits);
    virtual void set_note(const char* message);

/*                             Take type                            */
    virtual void set_default_take_type();
    virtual void set_no_good(bool flagged);
    virtual void set_false_start(bool flagged);
    virtual void set_wild_track(bool flagged);
    virtual void set_pickup(bool flagged);
    virtual void set_rehearsal(bool flagged);
    virtual void set_announcement(bool flagged);
    virtual void set_sound_guide(bool flagged);

/*                              Speed                               */
protected:
    virtual void _set_framerate(const char* fps);
    virtual void _set_timestamp_ixml();
    void _set_timestamp() override;
    void _set_timestamp(uint64_t numSamples) override;
    void _set_timestamp(uint32_t ssmLo, uint32_t ssmHi) override;

public:
    void set_sample_rate(uint32_t samplerate) override;
    virtual void set_bit_depth(uint16_t bitsPerSample);
    void set_framerate(double fps, bool isDropframe) override;
    void set_framerate(int fps, bool isDropframe) override;
    void set_timecode(int hr, int min, int sec, int frm) override;
    void set_timecode(std::array<int, 4> tc) override;
    void set_timecode(int numFrames) override;
    void clear_timecode() override;

/*                            Sync Points                           */
protected:
    static inline bool _sync_point_sorter(
            std::shared_ptr<SyncPoint> first,
            std::shared_ptr<SyncPoint> second
        );
    inline size_t _get_sync_point_index(std::shared_ptr<SyncPoint> point);
    inline void _update_sync_point_count();
    virtual std::shared_ptr<SyncPoint> _create_sync_point(uint64_t numSamples);

public:
    virtual std::shared_ptr<SyncPoint> create_sync_point(uint64_t numSamples);
    virtual std::shared_ptr<SyncPoint> create_sync_point(TimecodeBase::Clock& clock);
    virtual std::shared_ptr<SyncPoint> create_sync_point();
    virtual void destroy_sync_point(std::shared_ptr<SyncPoint> point);
    virtual void clear_sync_points();

/*                              History                             */
    virtual void set_filename(const char* filename);
    virtual void set_parent_uid(const char* data);

/*                             File set                             */
    virtual void set_total_files(unsigned int numFiles);
    virtual void set_family_uid();
    virtual void set_family_name(const char* familyName);
    virtual void set_family_name();
    virtual void set_file_set_index(const char* index);

/*                            Track list                            */
protected:
    virtual void _write_track_list();

public:
    virtual void set_channels(uint16_t channels);
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
    void set_originator(const char* newOriginator) override;
    void clear_originator() override;
    void set_originator_reference(const char* newReference) override;
    void clear_originator_reference() override;
    void set_description(const char* newDescription) override;
    void clear_description() override;
    void set_date(int16_t year, uint8_t month, uint8_t day) override;
    void set_date_str(const char* date) override;
    void clear_date() override;
    void set_time(uint8_t hour, uint8_t minute, uint8_t second) override;
    void set_time_str(const char* time) override;
    void clear_time() override;
    void set_bwf_version(uint16_t versionNumber) override;
    void set_umid(const uint8_t* newUmid) override;
    void clear_umid() override;
    void set_loudness_value(uint16_t value) override;
    void set_loudness_range(uint16_t range) override;
    void set_loudness_max_true_peak(uint16_t level) override;
    void set_loudness_max_momentary(uint16_t level) override;
    void set_loudness_max_short_term(uint16_t value) override;
    void clear_loudness() override;
    void set_reserved() override;
    void set_coding_history(BEXT::CodingHistoryRow row) override;
    void append_to_coding_history(BEXT::CodingHistoryRow row) override;
    void clear_coding_history() override;
    virtual void import_bext_chunk(BEXT::BEXTChunk& chunk);

    uint32_t size() override;
    size_t total_size() override;
    const char* _xml_c_str();
    void copy_to_buffer(uint8_t* buff) override;
};

};

#endif
