#include "iXML.h"

using namespace iXML;

void iXML::get_flag_str(
        char* buff,
        bool& initialized,
        bool flag,
        const char* str
    )
{
    strcat(buff, ((flag && initialized) ? "," : ""));
    strcat(buff, (flag ? str : ""));
    initialized = (initialized || flag);
}

Base::Base(XMLDocument* xmldoc, const char* elementName)
{
    this->ixml = xmldoc;
    this->_element = this->ixml->NewElement(elementName);
}

Base::~Base()
{
}

XMLElement* Base::_set_child_element(const char* elementName)
{
    XMLElement* newElement = this->ixml->NewElement(elementName);
    this->_element->InsertEndChild(newElement);
    return newElement;
}

TakeType::TakeType(XMLDocument* xmldoc) :
Base(xmldoc, "TAKE_TYPE")
{
    set_default();
}

void TakeType::set_default()
{
    this->defaultTake = true;
    this->no_good = false;
    this->false_start = false;
    this->wild_track = false;
    this->pickup = false;
    this->rehearsal = false;
    this->announcement = false;
    this->sound_guide = false;
}

bool TakeType::is_default()
{
    this->defaultTake = !(
            this->no_good
            || this->false_start
            || this->wild_track
            || this->pickup
            || this->rehearsal
            || this->announcement
            || this->sound_guide
        );
    return this->defaultTake;
}

void TakeType::_set_element()
{
    char buff[80];
    bool firstFlagWritten(false);
    buff[0] = '\0';
    get_flag_str(buff, firstFlagWritten, is_default(), "DEFAULT");
    get_flag_str(buff, firstFlagWritten, this->no_good, "NO_GOOD");
    get_flag_str(buff, firstFlagWritten, this->false_start, "FALSE_START");
    get_flag_str(buff, firstFlagWritten, this->wild_track, "WILD_TRACK");
    get_flag_str(buff, firstFlagWritten, this->pickup, "PICKUP");
    get_flag_str(buff, firstFlagWritten, this->rehearsal, "REHEARSAL");
    get_flag_str(buff, firstFlagWritten, this->announcement, "ANNOUNCEMENT");
    get_flag_str(buff, firstFlagWritten, this->sound_guide, "SOUND_GUIDE");
    this->_element->SetText(buff);
}

Speed::Speed(XMLDocument* xmldoc) :
Base(xmldoc, "SPEED")
{
    this->note = _set_child_element("NOTE");
    this->master_speed = _set_child_element("MASTER_SPEED");
    this->current_speed = _set_child_element("CURRENT_SPEED");
    this->timecode_rate = _set_child_element("TIMECODE_RATE");
    this->timecode_flag = _set_child_element("TIMECODE_FLAG");
    this->file_sample_rate = _set_child_element("FILE_SAMPLE_RATE");
    this->audio_bit_depth = _set_child_element("AUDIO_BIT_DEPTH");
    this->digitizer_sample_rate = _set_child_element("DIGITIZER_SAMPLE_RATE");
    this->timestamp_samples_since_midnight_hi = _set_child_element("TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_HI");
    this->timestamp_samples_since_midnight_lo = _set_child_element("TIMESTAMP_SAMPLE_SINCE_MIDNIGHT_LO");
    this->timestamp_sample_rate = _set_child_element("TIMESTAMP_SAMPLE_RATE");
    this->note->SetText("");
}

History::History(XMLDocument* xmldoc) :
Base(xmldoc, "HISTORY")
{
    this->original_filename = _set_child_element("ORIGINAL_FILENAME");
    this->parent_filename = _set_child_element("PARENT_FILENAME");
    this->parent_uid = _set_child_element("PARENT_UID");
    this->_parent_uid = this->ixml->NewText("");
    this->parent_uid->InsertEndChild(this->_parent_uid);
}

LoudnessElement::LoudnessElement(XMLDocument* xmldoc) :
Base(xmldoc, "LOUDNESS")
{
    this->loudness_value = _set_child_element("LOUDNESS_VALUE");
    this->loudness_range = _set_child_element("LOUDNESS_RANGE");
    this->max_short_term_loudness = _set_child_element("MAX_SHORT_TERM_LOUDNESS");
    this->max_momentary_loudness = _set_child_element("MAX_MOMENTARY_LOUDNESS");
    this->max_true_peak_level = _set_child_element("MAX_TRUE_PEAK_LEVEL");
}

FileSet::FileSet(XMLDocument* xmldoc) :
Base(xmldoc, "FILE_SET")
{
    this->total_files = _set_child_element("TOTAL_FILES");
    this->family_uid = _set_child_element("FAMILY_UID");
    this->family_name = _set_child_element("FAMILY_NAME");
    this->file_set_index = _set_child_element("FILE_SET_INDEX");
    this->_family_uid = this->ixml->NewText("");
    this->family_uid->InsertEndChild(this->_family_uid);
}

Track::Track(XMLDocument* xmldoc) :
Base(xmldoc, "TRACK"),
_active(true),
_index(0),
_interleaveIndex(0),
_name(""),
_function("")
{
}

void Track::set_channel_index(uint16_t channelIndex)
{
    this->_index = channelIndex;
}

void Track::set_interleave_index(uint16_t interleaveIndex)
{
    this->_interleaveIndex = interleaveIndex;
}

void Track::set_name(const char* trackName)
{
    strcpy(this->_name, trackName);
}

void Track::set_function(const char* trackFunction)
{
    strcpy(this->_function, trackFunction);
}

uint16_t Track::get_channel_index()
{
    return this->_index;
}

TrackList::TrackList(XMLDocument* xmldoc) :
Base(xmldoc, "TRACK_LIST")
{
}

BEXTElement::BEXTElement(XMLDocument* xmldoc) :
Base(xmldoc, "BEXT")
{
    this->bwf_description = _set_child_element("BWF_DESCRIPTION");
    this->bwf_originator = _set_child_element("BWF_ORIGINATOR");
    this->bwf_originator_reference = _set_child_element("BWF_ORIGINATOR_REFERENCE");
    this->bwf_origination_date = _set_child_element("BWF_ORIGINATION_DATE");
    this->bwf_origination_time = _set_child_element("BWF_ORIGINATION_TIME");
    this->bwf_time_reference_low = _set_child_element("BWF_TIME_REFERENCE_LOW");
    this->bwf_time_reference_high = _set_child_element("BWF_TIME_REFERENCE_HIGH");
    this->bwf_version = _set_child_element("BWF_VERSION");
    this->bwf_umid = _set_child_element("BWF_UMID");
    this->bwf_reserved = _set_child_element("BWF_RESERVED");
    this->bwf_coding_history = _set_child_element("BWF_CODING_HISTORY");
    this->bwf_loudness_value = _set_child_element("BWF_LOUDNESS_VALUE");
    this->bwf_loudness_range = _set_child_element("BWF_LOUDNESS_RANGE");
    this->bwf_max_short_term_loudness = _set_child_element("BWF_MAX_SHORT_TERM_LOUDNESS");
    this->bwf_max_momentary_loudness = _set_child_element("BWF_MAX_MOMENTARY_LOUDNESS");
    this->bwf_max_true_peak_level = _set_child_element("BWF_MAX_TRUE_PEAK_LEVEL");
    this->_reserved = this->ixml->NewText("");
    this->bwf_reserved->InsertEndChild(this->_reserved);
    this->_umid = this->ixml->NewText("");
    this->bwf_umid->InsertEndChild(this->_umid);
}

SyncPoint::SyncPoint(XMLDocument* xmldoc) :
Base(xmldoc, "SYNC_POINT"),
_timestampSet(false),
_timestamp(0)
{
    this->sync_point_type = _set_child_element("SYNC_POINT_TYPE");
    this->sync_point_function = _set_child_element("SYNC_POINT_FUNCTION");
    this->sync_point_comment = _set_child_element("SYNC_POINT_COMMENT");
    this->sync_point_low = _set_child_element("SYNC_POINT_LOW");
    this->sync_point_high = _set_child_element("SYNC_POINT_HIGH");
    this->sync_point_event_duration = _set_child_element("SYNC_POINT_EVENT_DURATION");
    this->sync_point_event_duration->SetText(0);
    this->sync_point_type->SetText("");
    this->sync_point_function->SetText("");
    this->sync_point_comment->SetText("");
}

void SyncPoint::set_type(const char* syncPointType)
{
    this->sync_point_type->SetText(syncPointType);
}

void SyncPoint::set_function(const char* syncPointFunction)
{
    this->sync_point_function->SetText(syncPointFunction);
}

void SyncPoint::set_comment(const char* comment)
{
    this->sync_point_comment->SetText(comment);
}

void SyncPoint::_set_timestamp()
{
    uint32_t* cast = reinterpret_cast<uint32_t*>(&this->_timestamp);
    this->sync_point_low->SetText(*cast);
    this->sync_point_high->SetText(*(cast + 1));
    this->_timestampSet = true;
}

void SyncPoint::set_timestamp(uint64_t numSamples)
{
    this->_timestamp = numSamples;
    _set_timestamp();
}

void SyncPoint::set_timestamp(const TimecodeBase::Clock& clock)
{
    this->_timestamp = clock.samplesSinceMidnight;
    this->sync_point_low->SetText(*clock.timeReferenceLow);
    this->sync_point_high->SetText(*clock.timeReferenceHigh);
    this->_timestampSet = true;
}

uint64_t SyncPoint::get_timestamp() const
{
    return this->_timestamp;
}

void SyncPoint::set_duration(uint64_t duration)
{
    this->sync_point_event_duration->SetText(duration);
}

SyncPointList::SyncPointList(XMLDocument* xmldoc) :
Base(xmldoc, "SYNC_POINT_LIST")
{
    this->sync_point_count = _set_child_element("SYNC_POINT_COUNT");
    this->_element->InsertEndChild(this->sync_point_count);
}

Location::Location(XMLDocument* xmldoc) :
Base(xmldoc, "LOCATION"),
_isInterior(false),
_isExterior(false),
_isTimeSunrise(false),
_isTimeMorning(false),
_isTimeMidday(false),
_isTimeDay(false),
_isTimeAfternoon(false),
_isTimeEvening(false),
_isTimeSunset(false),
_isTimeNight(false)
{
    this->location_name = _set_child_element("LOCATION_NAME");
    this->location_gps = _set_child_element("LOCATION_GPS");
    this->location_altitude = _set_child_element("LOCATION_ALTITUDE");
    this->location_type = _set_child_element("LOCATION_TYPE");
    this->location_time = _set_child_element("LOCATION_TIME");
}

void Location::_set_type()
{
    char buff[32];
    bool firstFlagWritten(false);
    buff[0] = '\0';
    get_flag_str(buff, firstFlagWritten, this->_isInterior, "INT");
    get_flag_str(buff, firstFlagWritten, this->_isExterior, "EXT");
    this->location_type->SetText(buff);
}

void Location::_set_time()
{
    char buff[64];
    bool firstFlagWritten(false);
    buff[0] = '\0';
    get_flag_str(buff, firstFlagWritten, this->_isTimeSunrise, "SUNRISE");
    get_flag_str(buff, firstFlagWritten, this->_isTimeMorning, "MORNING");
    get_flag_str(buff, firstFlagWritten, this->_isTimeMidday, "MIDDAY");
    get_flag_str(buff, firstFlagWritten, this->_isTimeDay, "DAY");
    get_flag_str(buff, firstFlagWritten, this->_isTimeAfternoon, "AFTERNOON");
    get_flag_str(buff, firstFlagWritten, this->_isTimeEvening, "EVENING");
    get_flag_str(buff, firstFlagWritten, this->_isTimeSunset, "SUNSET");
    get_flag_str(buff, firstFlagWritten, this->_isTimeNight, "NIGHT");
    this->location_time->SetText(buff);
}

User::User(XMLDocument* xmldoc) :
Base(xmldoc, "USER")
{
    this->full_title = _set_child_element("FULL_TITLE");
    this->director_name = _set_child_element("DIRECTOR_NAME");
    this->production_name = _set_child_element("PRODUCTION_NAME");
    this->production_address = _set_child_element("PRODUCTION_ADDRESS");
    this->production_email = _set_child_element("PRODUCTION_EMAIL");
    this->production_phone = _set_child_element("PRODUCTION_PHONE");
    this->production_note = _set_child_element("PRODUCTION_NOTE");
    this->sound_mixer_name = _set_child_element("SOUND_MIXER_NAME");
    this->sound_mixer_address = _set_child_element("SOUND_MIXER_ADDRESS");
    this->sound_mixer_email = _set_child_element("SOUND_MIXER_EMAIL");
    this->sound_mixer_phone = _set_child_element("SOUND_MIXER_PHONE");
    this->sound_mixer_note = _set_child_element("SOUND_MIXER_NOTE");
    this->audio_recorder_model = _set_child_element("AUDIO_RECORDER_MODEL");
    this->audio_recorder_serial_number = _set_child_element("AUDIO_RECORDER_SERIAL_NUMBER");
    this->audio_recorder_firmware = _set_child_element("AUDIO_RECORDER_FIRMWARE");
}

IXML::IXML() :
TimecodeBase::Clock(),
take_type(&ixml),
speed(&ixml),
history(&ixml),
loudness(&ixml),
file_set(&ixml),
track_list(&ixml),
bext(&ixml),
sync_point_list(&ixml),
location(&ixml),
user(&ixml),
numTracks(0),
numSyncPoints(0),
_ixmlChunkSize(0),
_padBytes(0),
_digitizerSampleRate(sampleRate)
{
    memcpy(this->_ixmlChunkID, "iXML", 4);
    this->root = this->ixml.NewElement("BWFXML");
    this->ixml.InsertFirstChild(this->root);
    this->ixml_version = this->ixml.NewElement("IXML_VERSION");
    this->project = this->ixml.NewElement("PROJECT");
    this->tape = this->ixml.NewElement("TAPE");
    this->scene = this->ixml.NewElement("SCENE");
    this->take = this->ixml.NewElement("TAKE");
    this->circled = this->ixml.NewElement("CIRCLED");
    this->file_uid = this->ixml.NewElement("FILE_UID");
    this->_file_uid = this->ixml.NewText("");
    this->file_uid->InsertEndChild(this->_file_uid);
    this->ubits = this->ixml.NewElement("UBITS");
    this->note = this->ixml.NewElement("NOTE");
    this->root->InsertEndChild(this->ixml_version);
    this->root->InsertEndChild(this->project);
    this->root->InsertEndChild(this->tape);
    this->root->InsertEndChild(this->scene);
    this->root->InsertEndChild(this->take);
    this->root->InsertEndChild(this->circled);
    this->root->InsertEndChild(this->file_uid);
    this->root->InsertEndChild(this->ubits);
    this->root->InsertEndChild(this->note);
    this->root->InsertEndChild(this->take_type._element);
    this->root->InsertEndChild(this->speed._element);
    this->root->InsertEndChild(this->history._element);
    this->root->InsertEndChild(this->loudness._element);
    this->root->InsertEndChild(this->file_set._element);
    this->root->InsertEndChild(this->track_list._element);
    this->root->InsertEndChild(this->bext._element);
    this->root->InsertEndChild(this->sync_point_list._element);
    this->root->InsertEndChild(this->location._element);
    this->root->InsertEndChild(this->user._element);
    set_ixml_version(supportedIXMLVersionMajor, supportedIXMLVersionMinor);
    clear();
    set_family_uid(); // Set random Family UID until overwritten
}

IXML::~IXML()
{
}

void IXML::clear()
{
    set_scene("");
    set_ubits(0x00, 0x00, 0x00, 0x00);
    set_take(1);
    set_circled(false);
    set_file_uid();
    set_family_uid();
}

void IXML::set_ixml_version(uint16_t major, uint16_t minor)
{
    char ixmlVersion[8];
    sprintf(ixmlVersion, "%u.%u", major, minor);
    this->ixml_version->SetText(ixmlVersion);
}

std::pair<const uint16_t, const uint16_t> IXML::get_ixml_version()
{
    return std::pair<const uint16_t, const uint16_t>(
            supportedIXMLVersionMajor,
            supportedIXMLVersionMinor
        );
}

void IXML::set_project(const char* projectNameText)
{
    this->project->SetText(projectNameText);
}

const char* IXML::get_project()
{
    return this->project->GetText();
}

void IXML::set_tape(const char* tapeNameText)
{
    this->tape->SetText(tapeNameText);
}

const char* IXML::get_tape()
{
    return this->tape->GetText();
}

void IXML::set_scene(const char* sceneNameText)
{
    this->scene->SetText(sceneNameText);
}

const char* IXML::get_scene()
{
    return this->scene->GetText();;
}

void IXML::set_take(int takeNumText)
{
    char takeNumStr[4];
    sprintf(takeNumStr, "%03u", takeNumText);
    this->take->SetText(takeNumStr);
}

int IXML::get_take()
{
    return std::atoi(this->take->GetText());
}

void IXML::set_circled(bool isCircled)
{
    this->circled->SetText(isCircled);
}

bool IXML::is_circled()
{
    return this->circled->GetText();
}

void IXML::set_file_uid()
{
    int sceneSum(0), i(0);
    char buff[33];
    const char* sceneNameText = this->scene->GetText();
    while (sceneNameText[i] != '\0') sceneSum += static_cast<uint16_t>(sceneNameText[i++]);
    unsigned int seed = (get_frames() * 2) + 3483423 + sceneSum;
    get_random_str(buff, 32, ixmlUIDValidChars, seed);
    this->_file_uid->SetValue(buff);
}

const char* IXML::get_file_uid()
{
    return this->file_uid->GetText();
}

void IXML::set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
{
    char buff[9];
    sprintf(buff, "%02x%02x%02x%02x", first, second, third, fourth);
    buff[8] = '\0';
    this->ubits->SetText(buff);
}

void IXML::_assert_valid_ubits(const char* bits)
{
    for (uint8_t i(0); i < 8; ++i)
    {
        for (uint8_t j(0); j < 16; ++j)
        {
            if (bits[i] == TimecodeBase::ubitsValidChars[j])
            {
                goto skip;
            }
        }
        throw INVALID_UBITS_CHARS;
        skip: continue;
    }
}

void IXML::set_ubits(const char* bits)
{
    _assert_valid_ubits(bits);
    this->ubits->SetText(bits);
}

const uint8_t* IXML::get_ubits() const
{
    return reinterpret_cast<const uint8_t*>(this->ubits->GetText());
}

void IXML::set_note(const char* message)
{
    this->note->SetText(message);
}

const char* IXML::get_note()
{
    return this->note->GetText();
}

void IXML::set_default_take_type()
{
    this->take_type.set_default();
}

bool IXML::is_default_take_type()
{
    return this->take_type.is_default();
}

void IXML::set_no_good(bool flag)
{
    this->take_type.no_good = flag;
}

bool IXML::is_no_good()
{
    return this->take_type.no_good;
}

void IXML::set_false_start(bool flag)
{
    this->take_type.false_start = flag;
}

bool IXML::is_false_start()
{
    return this->take_type.false_start;
}

void IXML::set_wild_track(bool flag)
{
    this->take_type.wild_track = flag;
}

bool IXML::is_wild_track()
{
    return this->take_type.wild_track;
}

void IXML::set_pickup(bool flag)
{
    this->take_type.pickup = flag;
}

bool IXML::is_pickup()
{
    return this->take_type.pickup;
}

void IXML::set_rehearsal(bool flag)
{
    this->take_type.rehearsal = flag;
}
bool IXML::is_rehearsal()
{
    return this->take_type.rehearsal;
}

void IXML::set_announcement(bool flag)
{
    this->take_type.announcement = flag;
}

bool IXML::is_announcement()
{
    return this->take_type.announcement;
}

void IXML::set_sound_guide(bool flag)
{
    this->take_type.sound_guide = flag;
}

bool IXML::is_sound_guide()
{
    return this->take_type.sound_guide;
}

void IXML::set_overcrank(bool flag)
{
    this->_digitizerSampleRate = (
            flag
            ? TimecodeBase::get_overcrank_rate(this->sampleRate)
            : this->sampleRate
        );
    this->speed.digitizer_sample_rate->SetText(this->_digitizerSampleRate);
    this->speed.master_speed->SetText(
            flag ?
            TimecodeBase::get_overcrank_framerate(this->_framerate)
            : this->_framerate
        );
    this->speed.note->SetText(
            flag ?
            "0.1% Overcrank"
            : ""
        );
}

bool IXML::is_overcranked()
{
    return this->sampleRate != this->_digitizerSampleRate;
}

void IXML::set_sample_rate(uint32_t samplerate)
{
    bool isOvercrankRate(TimecodeBase::is_overcrank_rate(samplerate));
    TimecodeBase::Clock::set_sample_rate(
            isOvercrankRate ?
            TimecodeBase::get_undercrank_rate(samplerate)
            : samplerate
        );
    this->speed.file_sample_rate->SetText(this->sampleRate);
    this->speed.timestamp_sample_rate->SetText(this->sampleRate);
    set_overcrank(isOvercrankRate);
}

void IXML::set_bit_depth(uint16_t bitsPerSample)
{
    this->speed.audio_bit_depth->SetText(bitsPerSample);
}

inline void IXML::_set_framerate_str()
{
    this->speed.master_speed->SetText(this->_framerate);
    this->speed.current_speed->SetText(this->_framerate);
    this->speed.timecode_rate->SetText(this->_framerate);
}

void IXML::set_framerate(double fps)
{
    TimecodeBase::Clock::set_framerate(fps);
    _set_framerate_str();
}

void IXML::set_framerate(int fps)
{
    TimecodeBase::Clock::set_framerate(fps);
    _set_framerate_str();
}

void IXML::set_framerate(const char* fps)
{
    TimecodeBase::Clock::set_framerate(fps);
    _set_framerate_str();
}

void IXML::set_dropframe(bool isDropframe)
{
    TimecodeBase::Clock::set_dropframe(isDropframe);
    this->speed.timecode_flag->SetText(this->_dropframe ? "DF" : "NDF");
}

inline void IXML::_set_timestamp_ixml()
{
    this->speed.timestamp_samples_since_midnight_lo->SetText(*(this->timeReferenceLow));
    this->speed.timestamp_samples_since_midnight_hi->SetText(*(this->timeReferenceHigh));
    this->bext.bwf_time_reference_low->SetText(*(this->timeReferenceLow));
    this->bext.bwf_time_reference_high->SetText(*(this->timeReferenceHigh));
}

inline void IXML::_set_timestamp()
{
    TimecodeBase::Clock::_set_timestamp();
    _set_timestamp_ixml();
}

inline void IXML::_set_timestamp(uint64_t numSamples)
{
    TimecodeBase::Clock::_set_timestamp(numSamples);
    _set_timestamp_ixml();
}

inline void IXML::_set_timestamp(uint32_t ssmLo, uint32_t ssmHi)
{
    TimecodeBase::Clock::_set_timestamp(ssmLo, ssmHi);
    _set_timestamp_ixml();
}

void IXML::set_timecode(int hr, int min, int sec, int frm)
{
    TimecodeBase::Clock::set_timecode(hr, min, sec, frm);
    _set_timestamp();
}

void IXML::set_timecode(std::array<int, 4> tc)
{
    TimecodeBase::Clock::set_timecode(tc);
    _set_timestamp();
}

void IXML::set_timecode(int numFrames)
{
    TimecodeBase::Clock::set_timecode(numFrames);
    _set_timestamp();
}

void IXML::set_timecode(TimecodeBase::Base& base)
{
    TimecodeBase::Base::set_timecode(base);
    _set_timestamp();
}

void IXML::set_timecode(TimecodeBase::Clock& clock)
{
    TimecodeBase::Clock::set_timecode(clock);
    _set_timestamp();
}

void IXML::clear_timecode()
{
    set_timecode(0);
    this->bext.bwf_time_reference_low->SetText("");
    this->bext.bwf_time_reference_high->SetText("");
}

inline bool IXML::_sync_point_sorter(
        std::shared_ptr<SyncPoint> first,
        std::shared_ptr<SyncPoint> second
    )
{
    return first->get_timestamp() < second->get_timestamp();
}

inline void IXML::_update_sync_point_count()
{
    this->numSyncPoints = this->syncPoints.size();
    this->sync_point_list.sync_point_count->SetText(this->numSyncPoints);
}

size_t IXML::_get_sync_point_index(std::shared_ptr<SyncPoint> point)
{
    std::vector<std::shared_ptr<SyncPoint>>::iterator iter;
    iter = std::find(this->syncPoints.begin(), this->syncPoints.end(), point);
    if (iter == this->syncPoints.end()) throw SYNC_POINT_NOT_FOUND;
    return iter - this->syncPoints.begin();
}

std::shared_ptr<SyncPoint> IXML::_create_sync_point(uint64_t numSamples)
{
    this->syncPoints.emplace_back(std::make_shared<SyncPoint>(&this->ixml));
    std::shared_ptr<SyncPoint> point = this->syncPoints.back();
    point->set_timestamp(numSamples);
    this->numSyncPoints = this->syncPoints.size();
    if (this->numSyncPoints == 1)
    {
        this->sync_point_list._element->InsertEndChild(point->_element);
    }
    else
    {
        std::sort(
                this->syncPoints.begin(),
                this->syncPoints.end(),
                IXML::_sync_point_sorter
            );
        int index = _get_sync_point_index(point);
        if (index)
        {
            this->sync_point_list._element->InsertAfterChild(
                    this->syncPoints[index - 1]->_element, point->_element
                );
        }
        else
        {
            this->sync_point_list._element->InsertAfterChild(
                    this->sync_point_list.sync_point_count,
                    point->_element
                );
        }
    }
    this->sync_point_list.sync_point_count->SetText(this->numSyncPoints);
    return point;
}

std::shared_ptr<SyncPoint> IXML::create_sync_point(uint64_t numSamples)
{
    return _create_sync_point(numSamples);
}

std::shared_ptr<SyncPoint> IXML::create_sync_point(TimecodeBase::Clock& clock)
{
    return _create_sync_point(clock.samplesSinceMidnight);
}

std::shared_ptr<SyncPoint> IXML::create_sync_point()
{
    // Create sync point at current timestamp
    return _create_sync_point(this->samplesSinceMidnight);
}

void IXML::destroy_sync_point(std::shared_ptr<SyncPoint> point)
{
    std::vector<std::shared_ptr<SyncPoint>>::iterator iter;
    iter = std::find(this->syncPoints.begin(), this->syncPoints.end(), point);
    if (iter == this->syncPoints.end()) return;
    point->_element->DeleteChildren();
    this->sync_point_list._element->DeleteChild(point->_element);
    this->syncPoints.erase(iter);
    this->numSyncPoints = this->syncPoints.size();
    this->sync_point_list.sync_point_count->SetText(this->numSyncPoints);
}

void IXML::clear_sync_points()
{
    this->syncPoints.clear();
    _update_sync_point_count();
}

void IXML::set_filename(const char* filename)
{
    this->history.original_filename->SetText(filename);
}

const char* IXML::get_filename()
{
    return this->history.original_filename->GetText();
}

void IXML::set_parent_uid(const char* data)
{
    this->history._parent_uid->SetValue(data);
}

const char* IXML::get_parent_uid()
{
    return this->history.parent_uid->GetText();
}

void IXML::set_total_files(unsigned int numFiles)
{
    this->file_set.total_files->SetText(numFiles);
}

int IXML::get_total_files()
{
    return std::atoi(this->file_set.total_files->GetText());
}

void IXML::set_family_uid()
{
    int sceneSum(0), i(0);
    char buff[33];
    const char* sceneName = this->scene->GetText();
    while (sceneName[i] != '\0') sceneSum += static_cast<uint16_t>(sceneName[i++]);
    unsigned int seed = (get_frames() / 2) - 1085976 + sceneSum;
    get_random_str(buff, 32, ixmlUIDValidChars, seed);
    this->file_set._family_uid->SetValue(buff);
}

const char* IXML::get_family_uid()
{
    return this->file_set.family_uid->GetText();
}

void IXML::set_family_name()
{
    char buff[512];
    strcpy(buff, this->scene->GetText());
    strcat(buff, "/");
    strcat(buff, this->take->GetText());
    this->file_set.family_name->SetText(buff);
}

void IXML::set_family_name(const char* familyName)
{
    this->file_set.family_name->SetText(familyName);
}

const char* IXML::get_family_name()
{
    return this->file_set.family_name->GetText();
}

void IXML::set_file_set_index(const char* index)
{
    this->file_set.file_set_index->SetText(index);
}

const char* IXML::get_file_set_index()
{
    return this->file_set.file_set_index->GetText();
}

void IXML::set_channels(uint16_t channels)
{
    int tracksToCreate = channels - this->tracks.size();
    if (tracksToCreate > 0) for (int i(0); i < tracksToCreate; ++i) create_track();
    else if (tracksToCreate < 0)
    {
        int index(this->tracks.size() - 1);
        for (int i(0); i > tracksToCreate; --i)
        {
            this->tracks.erase(index--);
        }
    }
    _write_track_list();
}

uint16_t IXML::get_channels()
{
    uint16_t numActive(0);
    for (std::pair<const uint16_t, std::shared_ptr<Track>> p: this->tracks)
    {
        numActive += p.second->_active;
    }
    return numActive;
}

void IXML::_write_track_list()
{
    // Delete track data (name, function, etc) elements
    for (size_t i(0); i < this->_trackElements.size(); ++i)
    {
        this->_trackElements[i]->DeleteChildren();
    }

    // Delete track elements and count from track list
    this->track_list._element->DeleteChildren();

    // Clear deleted track pointers
    this->_trackElements.clear();
    
    // Recreate track count element
    XMLElement* track_count = this->track_list._set_child_element("TRACK_COUNT");

    // Re-insert track list
    this->root->InsertAfterChild(this->file_set._element, this->track_list._element);

    // Reset track count index
    this->numTracks = 0;

    if (!this->tracks.size())
    {
        track_count->SetText(this->numTracks);
        return;
    }

    // Iterate over Track objects
    for (std::pair<const uint16_t, std::shared_ptr<Track>>& p: this->tracks)
    {
        std::shared_ptr<Track> track = p.second;
        if (track->_active)
        {
            track->set_interleave_index(++this->numTracks);
            this->_trackElements.emplace_back(
                    this->track_list._element->InsertNewChildElement("TRACK")
                );
            XMLElement* trackElement = this->_trackElements.back();
            XMLElement* channelIndex = trackElement->InsertNewChildElement("CHANNEL_INDEX");
            XMLElement* interleaveIndex = trackElement->InsertNewChildElement("INTERLEAVE_INDEX");
            XMLElement* name = trackElement->InsertNewChildElement("NAME");
            XMLElement* function = trackElement->InsertNewChildElement("FUNCTION");
            channelIndex->SetText(track->_index);
            interleaveIndex->SetText(track->_interleaveIndex);
            name->SetText(track->_name);
            function->SetText(track->_function);
        }
    }

    // Set track count element
    track_count->SetText(this->numTracks);
}

std::shared_ptr<Track> IXML::get_track(const uint16_t index)
{
    std::map<const uint16_t, std::shared_ptr<Track>>::iterator iter;
    iter = this->tracks.find(index);
    if (iter == this->tracks.end()) throw TRACK_NOT_FOUND;
    return iter->second;
}

std::shared_ptr<Track> IXML::create_track()
{
    std::shared_ptr<Track> newTrack = std::make_shared<Track>(&this->ixml);
    newTrack->set_channel_index(++this->numTracks);
    newTrack->set_name("");
    newTrack->set_function("");
    std::pair<const uint16_t, std::shared_ptr<Track>> pair(newTrack->_index, newTrack);
    this->tracks.insert(pair);
    _write_track_list();
    return newTrack;
}

void IXML::destroy_track(std::shared_ptr<Track> track)
{
    this->tracks.erase(track->_index);
    _write_track_list();
}

void IXML::destroy_track(const uint16_t index)
{
    this->tracks.erase(index);
    _write_track_list();
}

void IXML::enable_track(std::shared_ptr<Track> track)
{
    if (track->_active) return;
    track->_active = true;
    _write_track_list();
}

void IXML::enable_track(const uint16_t index)
{
    std::shared_ptr<Track> track = get_track(index);
    if (track->_active) return;
    track->_active = true;
    _write_track_list();
}

void IXML::disable_track(std::shared_ptr<Track> track)
{
    if (!track->_active) return;
    track->_active = false;
    _write_track_list();
}

void IXML::disable_track(const uint16_t index)
{
    std::shared_ptr<Track> track = get_track(index);
    if (!track->_active) return;
    track->_active = false;
    _write_track_list();
}

void IXML::set_originator(const char* newOriginator)
{
    this->bext.bwf_originator->SetText(newOriginator);
}

void IXML::clear_originator()
{
    this->bext.bwf_originator->SetText("");
}

void IXML::set_originator_reference(const char* newReference)
{
    this->bext.bwf_originator_reference->SetText(newReference);
}

void IXML::clear_originator_reference()
{
    this->bext.bwf_originator_reference->SetText("");
}

void IXML::set_description(const char* newDescription)
{
    this->bext.bwf_description->SetText(newDescription);
}

void IXML::clear_description()
{
    this->bext.bwf_description->SetText("");
}

void IXML::set_date_str(const char* date)
{
    this->bext.bwf_origination_date->SetText(date);
}

void IXML::clear_date()
{
    this->bext.bwf_origination_date->SetText("");
}

void IXML::set_time_str(const char* time)
{
    this->bext.bwf_origination_time->SetText(time);
}

void IXML::clear_time()
{
    this->bext.bwf_origination_time->SetText("");
}

void IXML::set_bwf_version(uint16_t versionNumber)
{
    // Format to one decimal point FP
    char buff[4];
    sprintf(buff, "%u.0", versionNumber);
    buff[3] = '\0';
    this->bext.bwf_version->SetText(buff);
}

void IXML::set_umid(const uint8_t* data)
{
    this->bext._umid->SetValue(reinterpret_cast<const char*>(data));
}

void IXML::clear_umid()
{
    this->bext._umid->SetValue(std::string(64, '0').c_str());
}

void IXML::set_loudness_value(double value)
{
    this->loudness.loudness_value->SetText(value);
    this->bext.bwf_loudness_value->SetText(value);
}

void IXML::set_loudness_range(double value)
{
    this->loudness.loudness_range->SetText(value);
    this->bext.bwf_loudness_range->SetText(value);
}

void IXML::set_loudness_max_short_term(double value)
{
    this->loudness.max_short_term_loudness->SetText(value);
    this->bext.bwf_max_short_term_loudness->SetText(value);
}

void IXML::set_loudness_max_momentary(double value)
{
    this->loudness.max_momentary_loudness->SetText(value);
    this->bext.bwf_max_momentary_loudness->SetText(value);
}

void IXML::set_loudness_max_true_peak(double value)
{
    this->loudness.max_true_peak_level->SetText(value);
    this->bext.bwf_max_true_peak_level->SetText(value);
}

void IXML::clear_loudness()
{
    this->bext.bwf_loudness_value->SetText("");
    this->bext.bwf_loudness_range->SetText("");
    this->bext.bwf_max_short_term_loudness->SetText("");
    this->bext.bwf_max_momentary_loudness->SetText("");
    this->bext.bwf_max_true_peak_level->SetText("");
}

void IXML::set_reserved(const uint8_t* data)
{
    this->bext._reserved->SetValue(reinterpret_cast<const char*>(data));
}

void IXML::set_coding_history(std::string history)
{
    this->bext.bwf_coding_history->SetText(history.c_str());
}

void IXML::set_coding_history(const char* history)
{
    this->bext.bwf_coding_history->SetText(history);
}

void IXML::clear_coding_history()
{
    this->bext.bwf_coding_history->SetText("");
}

void IXML::import_bext_chunk(BEXT::BEXTChunk& chunk)
{
    set_description(chunk.description);
    set_originator(chunk.originator);
    set_originator_reference(chunk.originatorReference);
    set_date_str(chunk.originationDate);
    set_time_str(chunk.originationTime);
    _set_timestamp(*chunk.timeReferenceLow, *chunk.timeReferenceHigh);
    set_bwf_version(chunk.bwfVersion);
    set_reserved(chunk.reserved);
    set_umid(chunk.umid);
    set_coding_history(chunk.codingHistory.c_str());
    set_loudness_value(chunk.loudnessValue);
    set_loudness_range(chunk.loudnessRange);
    set_loudness_max_short_term(chunk.maxShortTermLoudness);
    set_loudness_max_momentary(chunk.maxMomentaryLoudness);
    set_loudness_max_true_peak(chunk.maxTruePeakLevel);
}

void IXML::set_location_name(const char* text)
{
    this->location.location_name->SetText(text);
}

const char* IXML::get_location_name()
{
    return this->location.location_name->GetText();
}

void IXML::set_location_gps(double longitude, double latitude)
{
    char buff[33];
    sprintf(buff, "%.8g, %.8g", longitude, latitude);
    this->location.location_gps->SetText(buff);
}

const char* IXML::get_location_gps()
{
    return this->location.location_gps->GetText();
}

void IXML::set_location_altitude(int altitude)
{
    this->location.location_altitude->SetText(altitude);
}

const char* IXML::get_location_altitude()
{
    // Returns altitude as text, not int
    return this->location.location_altitude->GetText();
}

void IXML::set_interior(bool flag)
{
    this->location._isInterior = flag;
}

bool IXML::is_interior()
{
    return this->location._isInterior;
}

void IXML::set_exterior(bool flag)
{
    this->location._isExterior = flag;
}

bool IXML::is_exterior()
{
    return this->location._isExterior;
}

void IXML::set_location_time_sunrise(bool flag)
{
    this->location._isTimeSunrise = flag;
}

bool IXML::get_location_time_sunrise()
{
    return this->location._isTimeSunrise;
}

void IXML::set_location_time_morning(bool flag)
{
    this->location._isTimeMorning = flag;
}

bool IXML::get_location_time_morning()
{
    return this->location._isTimeMorning;
}

void IXML::set_location_time_midday(bool flag)
{
    this->location._isTimeMidday = flag;
}

bool IXML::get_location_time_midday()
{
    return this->location._isTimeMidday;
}

void IXML::set_location_time_day(bool flag)
{
    this->location._isTimeDay = flag;
}

bool IXML::get_location_time_day()
{
    return this->location._isTimeDay;
}

void IXML::set_location_time_afternoon(bool flag)
{
    this->location._isTimeAfternoon = flag;
}

bool IXML::get_location_time_afternoon()
{
    return this->location._isTimeAfternoon;
}

void IXML::set_location_time_evening(bool flag)
{
    this->location._isTimeEvening = flag;
}

bool IXML::get_location_time_evening()
{
    return this->location._isTimeEvening;
}

void IXML::set_location_time_sunset(bool flag)
{
    this->location._isTimeSunset = flag;
}

bool IXML::get_location_time_sunset()
{
    return this->location._isTimeSunset;
}

void IXML::set_location_time_night(bool flag)
{
    this->location._isTimeNight = flag;
}

bool IXML::get_location_time_night()
{
    return this->location._isTimeNight;
}

const char* IXML::get_location_type()
{
    return this->location.location_type->GetText();
}

const char* IXML::get_location_time()
{
    return this->location.location_time->GetText();
}

void IXML::set_full_title(const char* text)
{
    this->user.full_title->SetText(text);
}

const char* IXML::get_full_title()
{
    return this->user.full_title->GetText();
}

void IXML::set_director_name(const char* text)
{
    this->user.director_name->SetText(text);
}

const char* IXML::get_director_name()
{
    return this->user.director_name->GetText();
}

void IXML::set_production_name(const char* text)
{
    this->user.production_name->SetText(text);
}

const char* IXML::get_production_name()
{
    return this->user.production_name->GetText();
}

void IXML::set_production_address(const char* text)
{
    this->user.production_address->SetText(text);
}

const char* IXML::get_production_address()
{
    return this->user.production_address->GetText();
}

void IXML::set_production_email(const char* text)
{
    this->user.production_email->SetText(text);
}

const char* IXML::get_production_email()
{
    return this->user.production_email->GetText();
}

void IXML::set_production_phone(const char* text)
{
    this->user.production_phone->SetText(text);
}

const char* IXML::get_production_phone()
{
    return this->user.production_phone->GetText();
}

void IXML::set_production_note(const char* text)
{
    this->user.production_note->SetText(text);
}

const char* IXML::get_production_note()
{
    return this->user.production_note->GetText();
}

void IXML::set_sound_mixer_name(const char* text)
{
    this->user.sound_mixer_name->SetText(text);
}

const char* IXML::get_sound_mixer_name()
{
    return this->user.sound_mixer_name->GetText();
}

void IXML::set_sound_mixer_address(const char* text)
{
    this->user.sound_mixer_address->SetText(text);
}

const char* IXML::get_sound_mixer_address()
{
    return this->user.sound_mixer_address->GetText();
}

void IXML::set_sound_mixer_email(const char* text)
{
    this->user.sound_mixer_email->SetText(text);
}

const char* IXML::get_sound_mixer_email()
{
    return this->user.sound_mixer_email->GetText();
}

void IXML::set_sound_mixer_phone(const char* text)
{
    this->user.sound_mixer_phone->SetText(text);
}

const char* IXML::get_sound_mixer_phone()
{
    return this->user.sound_mixer_phone->GetText();
}

void IXML::set_sound_mixer_note(const char* text)
{
    this->user.sound_mixer_note->SetText(text);
}

const char* IXML::get_sound_mixer_note()
{
    return this->user.sound_mixer_note->GetText();
}

void IXML::set_audio_recorder_model(const char* text)
{
    this->user.audio_recorder_model->SetText(text);
}

const char* IXML::get_audio_recorder_model()
{
    return this->user.audio_recorder_model->GetText();
}

void IXML::set_audio_recorder_serial_number(const char* text)
{
    this->user.audio_recorder_serial_number->SetText(text);
}

const char* IXML::get_audio_recorder_serial_number()
{
    return this->user.audio_recorder_serial_number->GetText();
}

void IXML::set_audio_recorder_firmware(const char* text)
{
    this->user.audio_recorder_firmware->SetText(text);
}

const char* IXML::get_audio_recorder_firmware()
{
    return this->user.audio_recorder_firmware->GetText();
}

void IXML::set_pad_size(uint32_t numBytes)
{
    #ifdef _DEBUG
    assert(!(numBytes % 2));
    #endif
    this->_padBytes = numBytes;
}

size_t IXML::size()
{
    // Add NULL term + XML version/encoding + LF
    this->_ixmlChunkSize = std::strlen(
            reinterpret_cast<const char*>(_xml_c_str())
        ) + 41;
    this->_ixmlChunkSize += (this->_ixmlChunkSize % 2);
    this->_ixmlChunkSize += this->_padBytes;
    return this->_ixmlChunkSize;
}

const char* IXML::_xml_c_str()
{
    XMLPrinter printer;
    this->take_type._set_element();
    this->location._set_type();
    this->location._set_time();
    this->ixml.Print(&printer);
    const char* cstr = printer.CStr();
    return cstr;
}

size_t IXML::get(uint8_t* buff)
{
    #ifdef _DEBUG
    if (this->numTracks != get_channels()) throw TRACK_COUNT_MISMATCH;
    #endif
    
    const char* cstr = _xml_c_str();
    
    size_t xmlLength = std::strlen(cstr) + 1;
    this->_ixmlChunkSize = xmlLength + 40;

    size_t index(8);
    memcpy(buff, this->_ixmlChunkID, 4);

    /* Skip chunk size until the end
    to account for bytes potentially added
    to meet even byte boundary */

    memcpy(buff + index, xmlEncodingStr, 40);
    index += 40;

    memcpy(buff + index, cstr, xmlLength);
    index += xmlLength;

    // Optionally pad with blank data
    this->_ixmlChunkSize += this->_padBytes;
    if (this->_padBytes)
    {
        memset(buff + index, '\0', this->_padBytes);
        index += this->_padBytes;
    }

    // Pad with NULL if size is odd number of bytes
    if (index % 2)
    {
        ++this->_ixmlChunkSize;
        buff[index++ - 1] = '\0';
    }

    // Write corrected chunk size
    memcpy(buff + 4, &this->_ixmlChunkSize, 4);

    return index;
}
