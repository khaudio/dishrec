#include "BWFiXML.h"

using namespace BWFiXML;

const char* IXML::_ubits_valid_chars = "abcdef0123456789";

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
    set_default(true);
}

void TakeType::set_default(bool flagged)
{
    this->_default = flagged;
    if (this->_default)
    {
        this->_no_good = false;
        this->_false_start = false;
        this->_wild_track = false;
        this->_pickup = false;
        this->_rehearsal = false;
        this->_announcement = false;
        this->_sound_guide = false;
    }
    _apply();
}

void TakeType::no_good(bool flagged)
{
    this->_no_good = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::false_start(bool flagged)
{
    this->_false_start = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::wild_track(bool flagged)
{
    this->_wild_track = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::pickup(bool flagged)
{
    this->_pickup = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::rehearsal(bool flagged)
{
    this->_rehearsal = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::announcement(bool flagged)
{
    this->_announcement = flagged;
    if (flagged) this->_default = false;
    _apply();
}

void TakeType::sound_guide(bool flagged)
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
        stream << "NO_GOOD";
        initialized = true;
    }
    if (this->_false_start)
    {
        if (initialized) stream << ",";
        stream << "FALSE_START";
        initialized = true;
    }
    if (this->_wild_track)
    {
        if (initialized) stream << ",";
        stream << "WILD_TRACK";
        initialized = true;
    }
    if (this->_pickup)
    {
        if (initialized) stream << ",";
        stream << "PICKUP";
        initialized = true;
    }
    if (this->_rehearsal)
    {
        if (initialized) stream << ",";
        stream << "REHEARSAL";
        initialized = true;
    }
    if (this->_announcement)
    {
        if (initialized) stream << ",";
        stream << "ANNOUNCEMENT";
        initialized = true;
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
}


History::History(XMLDocument* xmldoc) :
Base(xmldoc, "HISTORY")
{
    this->original_filename = _set_child_element("ORIGINAL_FILENAME");
    this->current_filename = _set_child_element("CURRENT_FILENAME");
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
Base(xmldoc, "TRACK")
{
    this->channel_index = _set_child_element("CHANNEL_INDEX");
    this->interleave_index = _set_child_element("INTERLEAVE_INDEX");
    this->name = _set_child_element("NAME");
    this->function = _set_child_element("FUNCTION");
}

TrackList::TrackList(XMLDocument* xmldoc) :
Base(xmldoc, "TRACK_LIST")
{
    this->track_count = _set_child_element("TRACK_COUNT");
}

BEXT::BEXT(XMLDocument* xmldoc) :
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
Base(xmldoc, "SYNC_POINT")
{
    this->sync_point_type = _set_child_element("SYNC_POINT_TYPE");
    this->sync_point_function = _set_child_element("SYNC_POINT_FUNCTION");
    this->sync_point_comment = _set_child_element("SYNC_POINT_COMMENT");
    this->sync_point_low = _set_child_element("SYNC_POINT_LOW");
    this->sync_point_high = _set_child_element("SYNC_POINT_HIGH");
    this->sync_point_event_duration = _set_child_element("SYNC_POINT_EVENT_DURATION");
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
take_type(&ixml),
speed(&ixml),
history(&ixml),
file_set(&ixml),
track_list(&ixml),
bext(&ixml),
sync_point_list(&ixml),
location(&ixml),
user(&ixml)
{
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

    this->ixml_version->SetText("2.10");
    set_default();
}

IXML::~IXML()
{
}

void IXML::set_default()
{
    set_circled(false);
}

const char* IXML::c_str()
{
    XMLPrinter printer;
    this->ixml.Print(&printer);
    return printer.CStr();
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

void IXML::set_take(uint32_t takeNumber)
{
    this->take->SetText(takeNumber);
}

void IXML::set_circled(bool isCircled)
{
    this->circled->SetText(isCircled);
}

void IXML::set_ubits(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth)
{
    std::ostringstream stream;
    stream << std::hex << +first << +second << +third << +fourth;
    this->ubits->SetText(stream.str().c_str());
}

void IXML::set_ubits(const char* userbits)
{
    for (uint8_t i(0); i < 8; ++i)
    {
        bool valid(false);
        for (uint8_t j(0); j < 22; ++j)
        {
            if (userbits[i] == _ubits_valid_chars[j])
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

void IXML::set_sample_rate(uint32_t samplerate)
{
    this->speed->file_sample_rate->SetText(samplerate);
    this->speed->digitizer_sample_rate->SetText(samplerate);
    this->speed->timestamp_sample_rate->SetText(samplerate);
}

void IXML::set_bit_depth(uint16_t bitsPerSample, bool isFloat)
{
    this->speed->audio_bit_depth->SetText(bitsPerSample);
}


void IXML::_set_framerate(const char* fps)
{
    this->speed->master_speed->SetText(fps);
    this->speed->current_speed->SetText(fps);
    this->speed->timecode_rate->SetText(fps);
}

void IXML::set_framerate(float fps, bool isDropframe)
{
    this->speed->timecode_flag->SetText(isDropframe ? "DF" : "NDF");
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

void IXML::set_framerate(uint32_t fps, bool isDropframe)
{
    this->speed->timecode_flag->SetText(isDropframe ? "DF" : "NDF");
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

void IXML::set_filename(const char* filename)
{
    this->history->original_filename->SetText(filename);
    this->history->current_filename->SetText(filename);
}

