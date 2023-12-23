/** \file instrument.h
*/
#ifndef INSTRUMENT_H
#define INSTRUMENT_H
#include <Audio.h>
#include <memory>
#include "non-const-sample-metadata.h"

/**
 * @brief Declaration of CInstrument class.
*/
class CInstrument
{
public:
    /** @brief Deleted copy constructor.*/
    CInstrument(CInstrument &other) = delete;

    /** @brief Deleted operator overload.*/
    void operator = (const CInstrument &) = delete;

    /** @brief Create a singleton if necessary.
     * @return A pointer the the singleton.*/
    static CInstrument *getInstance();


    AudioSynthWavetable::instrument_data* load(const char* name);

protected:
    /** @brief constructor.*/
    CInstrument() {};

    /** @brief Dump hex bytes.
     * @param[in] bytes Pointer to the memory to be dumped.
     * @param[in] count Number of bytes to be dumped.
    */
    static void dumpHexBytes(const uint8_t *bytes, size_t count);
    static void dumpHexBytes(const uint32_t *bytes, size_t count);

    /** @brief update the current binary file position
     * @param[in] file a pointer to the File structure to query.
    */
    static void updateFilePosition(File* file);
    
    /** @brief Dump instrument data to log.
     * @param[in] instrumentData A pointer to the instrument data to be logged.
    */
    void dumpInstrumentData(instrument_data_t *instrumentData);

    /** @brief A pointer to the singleton.*/
    static CInstrument* singleton;
    /** @brief A pointer the instrument data.*/
    std::unique_ptr<instrument_data_t> instrument_data;
    /** @brief A pointer to the  raw sample data array.*/
    std::unique_ptr<uint32_t[]> raw_sample_data_array;
    /** @brief A pointer to the sample note ranges array.*/
    std::unique_ptr<uint8_t[]> sample_note_ranges_array;
    /** @brief A pointer to the samples metadata array.*/
    std::unique_ptr<my_non_const_sample_metadata[]> samples_metadata_array;
};
#endif // INSTRUMENT_H