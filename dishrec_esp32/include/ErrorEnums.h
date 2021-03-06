#ifndef ERRORENUMS_H
#define ERRORENUMS_H

/* Enums that should be replaced with std::exceptions */

enum timecode_err
{
    INVALID_FRAMERATE = 10,
    TIMEBASE_UNINITIALIZED = 11,
    INVALID_TC_STRING_LENGTH = 12,
    SAMPLERATE_UNINITIALIZED = 13,
};

enum ubits_err
{
    INVALID_UBITS_CHARS = 30,
};

enum format_err
{
    INVALID_SAMPLERATE = 50,
    INVALID_BIT_DEPTH = 51,
};

// enum file_err
// {
//     INAVLID_DIRECTORY = 70,
//     FILE_NOT_OPEN = 71,
//     FILENAME_NOT_SET = 72,
//     SEEK_ERROR = 73,
//     FILE_MODE_INCORRECT = 74,
// };

#endif
