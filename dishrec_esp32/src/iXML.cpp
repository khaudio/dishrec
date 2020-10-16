#include "iXML.h"

using namespace iXML;

const char* IXML::_ubitsValidChars = "0123456789abcdef";
const char* IXML::_uidValidChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
const char* IXML::_xmlEncoding = "\n<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";


void iXML::get_random_str(char* buff, uint32_t length)
{
    srand(time(NULL));
    for (uint32_t i(0); i < length; ++i) buff[i] = IXML::_uidValidChars[(rand() % 36)];
}

void iXML::get_random_str(char* buff, uint32_t length, unsigned int seed)
{
    srand(time(0) + seed);
    for (uint32_t i(0); i < length; ++i) buff[i] = IXML::_uidValidChars[(rand() % 36)];
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
    this->_default = true;
    this->_no_good = false;
    this->_false_start = false;
    this->_wild_track = false;
    this->_pickup = false;
    this->_rehearsal = false;
    this->_announcement = false;
    this->_sound_guide = false;
    _apply();
}

void TakeType::set_no_good(bool flagged)
{
    this->_no_good = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::set_false_start(bool flagged)
{
    this->_false_start = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::set_wild_track(bool flagged)
{
    this->_wild_track = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::set_pickup(bool flagged)
{
    this->_pickup = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::set_rehearsal(bool flagged)
{
    this->_rehearsal = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::set_announcement(bool flagged)
{
    this->_announcement = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::set_sound_guide(bool flagged)
{
    this->_sound_guide = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::_apply()
{
    bool initialized(false);
    std::ostringstream stream;
    if (this->_default)
    {
        stream << "DEFAULT";
        initialized = true;
    }
    if (this->_no_good)
    {
        if (initialized) stream << ",";
        else initialized = true;
        stream << "NO_GOOD";
    }
    if (this->_false_start)
    {
        if (initialized) stream << ",";
        else initialized = true;
        stream << "FALSE_START";
    }
    if (this->_wild_track)
    {
        if (initialized) stream << ",";
        else initialized = true;
        stream << "WILD_TRACK";
    }
    if (this->_pickup)
    {
        if (initialized) stream << ",";
        else initialized = true;
        stream << "PICKUP";
    }
    if (this->_rehearsal)
    {
        if (initialized) stream << ",";
        else initialized = true;
        stream << "REHEARSAL";
    }
    if (this->_announcement)
    {
        if (initialized) stream << ",";
        else initialized = true;
        stream << "ANNOUNCEMENT";
    }
    if (this->_sound_guide)
    {
        if (initialized) stream << ",";
        stream << "SOUND_GUIDE";
    }
    this->_element->SetText(stream.str().c_str());
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
}

FileSet::FileSet(XMLDocument* xmldoc) :
Base(xmldoc, "FILE_SET")
{
    this->total_files = _set_child_element("TOTAL_FILES");
    this->family_uid = _set_child_element("FAMILY_UID");
    this->family_name = _set_child_element("FAMILY_NAME");
    this->file_set_index = _set_child_element("FILE_SET_INDEX");
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
    this->bwf_max_true_peak_level = _set_child_element("BWF_MAX_TRUE_PEAK_LEVEL");
    this->bwf_max_momentary_loudness = _set_child_element("BWF_MAX_MOMENTARY_LOUDNESS");
    this->bwf_max_short_term_loudness = _set_child_element("BWF_MAX_SHORT_TERM_LOUDNESS");
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
    this->sync_point_low->SetText(*clock.timestampSSMLo);
    this->sync_point_high->SetText(*clock.timestampSSMHi);
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
Base(xmldoc, "LOCATION")
{
    this->location_name = _set_child_element("LOCATION_NAME");
    this->location_gps = _set_child_element("LOCATION_GPS");
    this->location_altitude = _set_child_element("LOCATION_ALTITUDE");
    this->location_type = _set_child_element("LOCATION_TYPE");
    this->location_time = _set_child_element("LOCATION_TYPE");
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
BEXT::BEXTChunk(),
take_type(&ixml),
speed(&ixml),
history(&ixml),
file_set(&ixml),
track_list(&ixml),
bext(&ixml),
sync_point_list(&ixml),
location(&ixml),
user(&ixml),
numTracks(0),
numSyncPoints(0)
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
    this->root->InsertEndChild(this->file_set._element);
    this->root->InsertEndChild(this->track_list._element);
    this->root->InsertEndChild(this->bext._element);
    this->root->InsertEndChild(this->sync_point_list._element);
    this->root->InsertEndChild(this->location._element);
    this->root->InsertEndChild(this->user._element);
    set_ixml_version(2, 10);
    clear();
    set_family_uid(); // Set random Family UID until overwritten
}

IXML::~IXML()
{
}

void IXML::clear()
{
    set_circled(false);
    set_ubits(0x00, 0x00, 0x00, 0x00);
    set_scene("");
    set_take(1);
    set_file_uid();
    set_family_uid();
    BEXT::BEXTChunk::clear();
}

void IXML::set_ixml_version(uint16_t major, uint16_t minor)
{
    this->_ixmlVersionMajor = major;
    this->_ixmlVersionMinor = minor;
    char ixmlVersion[8];
    sprintf(ixmlVersion, "%u.%u", major, minor);
    this->ixml_version->SetText(ixmlVersion);
}

void IXML::set_project(const char* projectName)
{
    this->project->SetText(projectName);
}

void IXML::set_tape(const char* tapeName)
{
    this->tape->SetText(tapeName);
}

void IXML::set_scene(const char* sceneName)
{
    this->scene->SetText(sceneName);
}

void IXML::set_take(int takeNum)
{
    this->takeNumber = takeNum;
    char takeNumStr[4];
    sprintf(takeNumStr, "%03u", takeNum);
    this->take->SetText(takeNumStr);
}

void IXML::set_circled(bool isCircled)
{
    this->circled->SetText(isCircled);
}

void IXML::set_file_uid()
{
    int sceneSum(0), i(0);
    const char* sceneName = this->scene->GetText();
    while (sceneName[i] != '\0') sceneSum += static_cast<uint16_t>(sceneName[i++]);
    unsigned int seed = (get_frames() * 2) + 3483423 + sceneSum + this->takeNumber;
    get_random_str(this->_fileUID, 32, seed);
    char buff[33];
    strncpy(buff, this->_fileUID, 32);
    buff[32] = '\0';
    this->file_uid->SetText(buff);
}

void IXML::set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
{
    char buff[9];
    sprintf(buff, "%02x%02x%02x%02x", first, second, third, fourth);
    buff[8] = '\0';
    this->ubits->SetText(buff);
}

void IXML::set_ubits(const char* userbits)
{
    for (uint8_t i(0); i < 8; ++i)
    {
        bool valid(false);
        for (uint8_t j(0); j < 16; ++j)
        {
            if (userbits[i] == _ubitsValidChars[j])
            {
                valid = true;
                break;
            }
        }
        if (!valid)
        {
            throw INVALID_UBITS_CHARS;
        }
    }
    this->ubits->SetText(userbits);
}

void IXML::set_note(const char* message)
{
    this->note->SetText(message);
}

void IXML::set_default_take_type()
{
    this->take_type.set_default();
}

void IXML::set_no_good(bool flagged)
{
    this->take_type.set_no_good(flagged);
}

void IXML::set_false_start(bool flagged)
{
    this->take_type.set_false_start(flagged);
}

void IXML::set_wild_track(bool flagged)
{
    this->take_type.set_wild_track(flagged);
}

void IXML::set_pickup(bool flagged)
{
    this->take_type.set_pickup(flagged);
}

void IXML::set_rehearsal(bool flagged)
{
    this->take_type.set_rehearsal(flagged);
}

void IXML::set_announcement(bool flagged)
{
    this->take_type.set_announcement(flagged);
}

void IXML::set_sound_guide(bool flagged)
{
    this->take_type.set_sound_guide(flagged);
}

void IXML::set_sample_rate(uint32_t samplerate)
{
    TimecodeBase::Clock::set_sample_rate(samplerate);
    this->speed.file_sample_rate->SetText(samplerate);
    this->speed.digitizer_sample_rate->SetText(samplerate);
    this->speed.timestamp_sample_rate->SetText(samplerate);
}

void IXML::set_bit_depth(uint16_t bitsPerSample, bool isFloat)
{
    this->speed.audio_bit_depth->SetText(bitsPerSample);
}

void IXML::_set_framerate(const char* fps)
{
    this->speed.master_speed->SetText(fps);
    this->speed.current_speed->SetText(fps);
    this->speed.timecode_rate->SetText(fps);
}

void IXML::set_framerate(double fps, bool isDropframe)
{
    TimecodeBase::Clock::set_framerate(fps, isDropframe);
    this->speed.timecode_flag->SetText(isDropframe ? "DF" : "NDF");
    if ((fps == 23.98) || (fps == 23.976))
    {
        _set_framerate("24000/1001");
    }
    else if (fps == 29.97)
    {
        _set_framerate("30000/1001");
    }
    else
    {
        throw INVALID_FRAMERATE;
    }
}

void IXML::set_framerate(int fps, bool isDropframe)
{
    TimecodeBase::Clock::set_framerate(fps, isDropframe);
    this->speed.timecode_flag->SetText(isDropframe ? "DF" : "NDF");
    switch (fps)
    {
        case (24):
            _set_framerate("24/1");
            break;
        case (25):
            _set_framerate("25/1");
            break;
        case (30):
            _set_framerate("30/1");
            break;
        default:
            throw INVALID_FRAMERATE;
    }
}

void IXML::_set_timestamp_ixml()
{
    this->speed.timestamp_samples_since_midnight_lo->SetText(*(this->timestampSSMLo));
    this->speed.timestamp_samples_since_midnight_hi->SetText(*(this->timestampSSMHi));
    BEXT::BEXTChunk::set_timestamp(*(this->timestampSSMLo), *(this->timestampSSMHi));
    this->bext.bwf_time_reference_low->SetText(*(this->timestampSSMLo));
    this->bext.bwf_time_reference_high->SetText(*(this->timestampSSMHi));
}

void IXML::_set_timestamp()
{
    TimecodeBase::Clock::_set_timestamp();
    _set_timestamp_ixml();
}

void IXML::_set_timestamp(uint64_t numSamples)
{
    TimecodeBase::Clock::_set_timestamp(numSamples);
    _set_timestamp_ixml();
}

void IXML::_set_timestamp(uint32_t ssmLo, uint32_t ssmHi)
{
    TimecodeBase::Clock::_set_timestamp(ssmLo, ssmHi);
    _set_timestamp_ixml();
}

void IXML::set_timecode(int hr, int min, int sec, int frm)
{
    TimecodeBase::Clock::set_timecode(hr, min, sec, frm);
}

void IXML::set_timecode(std::array<int, 4> tc)
{
    TimecodeBase::Clock::set_timecode(tc);
}

void IXML::set_timecode(int numFrames)
{
    TimecodeBase::Clock::set_timecode(numFrames);
}

void IXML::clear_timecode()
{
    set_timecode(0, 0, 0, 0);
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

void IXML::set_parent_uid(const char* data)
{
    this->history.parent_uid->SetText(data);
}

void IXML::set_total_files(unsigned int numFiles)
{
    this->file_set.total_files->SetText(numFiles);
}

void IXML::set_family_uid()
{
    int sceneSum(0), i(0);
    const char* sceneName = this->scene->GetText();
    while (sceneName[i] != '\0') sceneSum += static_cast<uint16_t>(sceneName[i++]);
    unsigned int seed = (get_frames() / 2) - 1085976 + sceneSum + this->takeNumber;
    get_random_str(this->_familyUID, 32, seed);
    char buff[33];
    strncpy(buff, this->_familyUID, 32);
    buff[32] = '\0';
    this->file_set.family_uid->SetText(buff);
}

void IXML::set_family_name()
{
    char buff[100];
    strcpy(buff, this->scene->GetText());
    strcat(buff, "/");
    strcat(buff, this->take->GetText());
    this->file_set.family_name->SetText(buff);
}

void IXML::set_family_name(const char* familyName)
{
    this->file_set.family_name->SetText(familyName);
}

void IXML::set_file_set_index(const char* index)
{
    this->file_set.file_set_index->SetText(index);
}

void IXML::set_channels(uint16_t channels)
{
    int tracksToCreate = channels - this->tracks.size();
    if (channels > 0) for (int i(0); i < tracksToCreate; ++i) create_track();
    
    // Set active/inactive tracks here?
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
            XMLElement* trackElement = this->_trackElements.emplace_back(
                    this->track_list._element->InsertNewChildElement("TRACK")
                );
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
    this->tracks.insert_or_assign(newTrack->_index, newTrack);
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
    BEXT::BEXTChunk::set_originator(newOriginator);
    this->bext.bwf_originator->SetText(this->originator);
}

void IXML::clear_originator()
{
    BEXT::BEXTChunk::clear_originator();
    this->bext.bwf_originator->SetText("");
}

void IXML::set_originator_reference(const char* newReference)
{
    BEXT::BEXTChunk::set_originator_reference(newReference);
    this->bext.bwf_originator_reference->SetText(this->originatorReference);
}

void IXML::clear_originator_reference()
{
    BEXT::BEXTChunk::clear_originator_reference();
    this->bext.bwf_originator_reference->SetText("");
}

void IXML::set_description(const char* newDescription)
{
    BEXT::BEXTChunk::set_description(newDescription);
    this->bext.bwf_description->SetText(this->description);
}

void IXML::clear_description()
{
    BEXT::BEXTChunk::clear_description();
    this->bext.bwf_description->SetText("");
}

void IXML::set_date(int16_t year, uint8_t month, uint8_t day)
{
    char buff[11];
    sprintf(buff, "%04u-%02u-%02u", year, month, day);
    buff[10] = '\0';
    BEXT::BEXTChunk::set_date_str(buff);
    this->bext.bwf_origination_date->SetText(buff);
}

void IXML::set_date_str(const char* date)
{
    BEXT::BEXTChunk::set_date_str(date);
    this->bext.bwf_origination_date->SetText(date);
}

void IXML::clear_date()
{
    BEXT::BEXTChunk::clear_date();
    this->bext.bwf_origination_date->SetText(this->originationDate);
}

void IXML::set_time(uint8_t hour, uint8_t minute, uint8_t second)
{
    char buff[9];
    sprintf(buff, "%2u:%02u:%02u", hour, minute, second);
    buff[8] = '\0';
    BEXT::BEXTChunk::set_time_str(buff);
    this->bext.bwf_origination_time->SetText(buff);
}

void IXML::set_time_str(const char* time)
{
    BEXT::BEXTChunk::set_time_str(time);
    this->bext.bwf_origination_time->SetText(time);
}

void IXML::clear_time()
{
    BEXT::BEXTChunk::clear_time();
    this->bext.bwf_origination_time->SetText(this->originationTime);
}

void IXML::set_bwf_version(uint16_t versionNumber)
{
    BEXT::BEXTChunk::set_bwf_version(versionNumber);
    
    // Format to one decimal point FP
    char buff[4];
    sprintf(buff, "%u.0", versionNumber);
    buff[3] = '\0';
    this->bext.bwf_version->SetText(buff);
}

void IXML::set_umid(const uint8_t* newUmid, uint8_t length)
{
    BEXT::BEXTChunk::set_umid(newUmid, length);
    this->bext.bwf_umid->SetText(this->umid);
}

void IXML::clear_umid()
{
    BEXT::BEXTChunk::clear_umid();
    this->bext.bwf_umid->SetText(std::string(64, '0').c_str());
}

void IXML::set_loudness_value(uint16_t value)
{
    BEXT::BEXTChunk::set_loudness_value(value);
    this->bext.bwf_loudness_value->SetText(this->loudnessValue);
}

void IXML::set_loudness_range(uint16_t range)
{
    BEXT::BEXTChunk::set_loudness_range(range);
    this->bext.bwf_loudness_range->SetText(this->loudnessRange);
}

void IXML::set_loudness_max_true_peak(uint16_t level)
{
    BEXT::BEXTChunk::set_loudness_max_true_peak(level);
    this->bext.bwf_max_true_peak_level->SetText(this->maxTruePeakLevel);
}

void IXML::set_loudness_max_momentary(uint16_t level)
{
    BEXT::BEXTChunk::set_loudness_max_momentary(level);
    this->bext.bwf_max_momentary_loudness->SetText(this->maxMomentaryLoudness);
}

void IXML::set_loudness_max_short_term(uint16_t value)
{
    BEXT::BEXTChunk::set_loudness_max_short_term(value);
    this->bext.bwf_max_short_term_loudness->SetText(this->maxShortTermLoudness);
}

void IXML::clear_loudness()
{
    BEXT::BEXTChunk::clear_loudness();
    this->bext.bwf_loudness_value->SetText("");
    this->bext.bwf_loudness_range->SetText("");
    this->bext.bwf_max_true_peak_level->SetText("");
    this->bext.bwf_max_momentary_loudness->SetText("");
    this->bext.bwf_max_short_term_loudness->SetText("");
}

void IXML::set_reserved()
{
    BEXT::BEXTChunk::set_reserved();
    std::ostringstream stream;
    for (int i(0); i < 180; ++i) stream << +(this->reserved[i]);
    this->bext.bwf_reserved->SetText(stream.str().c_str());
}

void IXML::set_coding_history(BEXT::CodingHistoryRow row)
{
    BEXT::BEXTChunk::set_coding_history(row);
    this->bext.bwf_coding_history->SetText(this->codingHistory.c_str());
}

void IXML::append_to_coding_history(BEXT::CodingHistoryRow row)
{
    BEXT::BEXTChunk::append_to_coding_history(row);
    this->bext.bwf_coding_history->SetText(this->codingHistory.c_str());
}

void IXML::clear_coding_history()
{
    BEXT::BEXTChunk::clear_coding_history();
    this->bext.bwf_coding_history->SetText("");
}

void IXML::import_bext_chunk(BEXT::BEXTChunk& chunk)
{
    char date[11], time[9];
    set_description(chunk.description);
    set_originator(chunk.originator);
    set_originator_reference(chunk.originatorReference);
    strncpy(date, chunk.originationDate, 10);
    date[10] = '\0';
    set_date_str(date);
    strncpy(time, chunk.originationTime, 8);
    time[8] = '\0';
    set_time_str(time);
    _set_timestamp(chunk.timeReferenceLow, chunk.timeReferenceHigh);
    set_bwf_version(chunk.bwfVersion);
    memcpy(this->reserved, chunk.reserved, 180);
    std::ostringstream reservedStream;
    for (int i(0); i < 180; ++i) reservedStream << +(this->reserved[i]);
    this->bext.bwf_reserved->SetText(reservedStream.str().c_str());
    memcpy(this->umid, chunk.umid, 64);
    std::ostringstream umidStream;
    for (int i(0); i < 64; ++i) umidStream << this->umid[i];
    this->bext.bwf_umid->SetText(umidStream.str().c_str());
    this->codingHistory = chunk.codingHistory;
    this->bext.bwf_coding_history->SetText(this->codingHistory.c_str());
    set_loudness_value(chunk.loudnessValue);
    set_loudness_range(chunk.loudnessRange);
    set_loudness_max_true_peak(chunk.maxTruePeakLevel);
    set_loudness_max_momentary(chunk.maxMomentaryLoudness);
    set_loudness_max_short_term(chunk.maxShortTermLoudness);
    this->_umidSet = chunk._umidSet;
    this->_loudnessValueSet = chunk._loudnessValueSet;
    this->_loudnessRangeSet = chunk._loudnessRangeSet;
    this->_maxTruePeakLevelSet = chunk._maxTruePeakLevelSet;
    this->_maxMomentaryLoudnessSet = chunk._maxMomentaryLoudnessSet;
    this->_maxShortTermLoudnessSet = chunk._maxShortTermLoudnessSet;
    if (!loudness_is_set()) clear_loudness();
}

uint32_t IXML::size()
{
    uint32_t chunkSize = BEXT::get_str_length<uint32_t>(
            reinterpret_cast<const char*>(_xml_c_str()), true
        ) + 40; // Add XML version/encoding + LF
    chunkSize += (chunkSize % 2);
    return chunkSize;
}

size_t IXML::total_size()
{
    return size() + 8; // Chunk + ID and Size indicators
}

const char* IXML::_xml_c_str()
{
    XMLPrinter printer;
    this->ixml.Print(&printer);
    const char* cstr = printer.CStr();
    return cstr;
}

void IXML::copy_to_buffer(uint8_t* buff)
{
    if (this->numTracks != get_channels()) throw TRACK_COUNT_MISMATCH;
    const char* cstr = _xml_c_str();
    this->_ixmlChunkSize = BEXT::get_str_length<uint32_t>(cstr, true);
    bool addByte = false;
    if (this->_ixmlChunkSize % 2)
    {
        this->_ixmlChunkSize += (this->_ixmlChunkSize % 2);
        addByte = true;
    }
    this->_ixmlExportedSize = this->_ixmlChunkSize + 48;
    memcpy(buff, this->_ixmlChunkID, 4);
    memcpy(buff + 4, &this->_ixmlChunkSize, 4);
    memcpy(buff + 8, this->_xmlEncoding, 40);
    memcpy(buff + 48, cstr, this->_ixmlChunkSize);
    if (addByte)
    {
        buff[this->_ixmlExportedSize - 1] = '\0';
    }
}
