/** \file utils.h
*/
#ifndef UTILS_H
#define UTILS_H

#include <Audio.h>

/**
 * @brief Declaration of CUtils class.
*/
class CUtils
{
public:
    /** @brief Dump sample metadata to log.
     * @param[in] instrumentData A pointer to the sample metadata to be logged.
    */
    static void dumpSampleMetadata(const AudioSynthWavetable::sample_data* metadata);
};
#endif // UTILS_H