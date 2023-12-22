/** \file instrument.cpp
*/
#include <ArduinoLog.h>
#undef CR
#include <SD.h>
#include <vector>
#include <ArduinoJson.h>
#include "instrument.h"
#include "utils.h"

static const int chipSelect = BUILTIN_SDCARD;

char filePosition[10];

CInstrument* CInstrument::singleton = nullptr;

CInstrument* CInstrument::getInstance()
{
    if (singleton == nullptr)
        singleton = new CInstrument();
    return singleton;
}

void CInstrument::updateFilePosition(File* file)
{
    snprintf(filePosition, 10, "%llu", file->position());
}

void CInstrument::dumpHexBytes(const uint8_t *bytes, size_t count)
{
    char row[256];
    size_t index = count - (count % 8);
    Log.verbose("Line %d bytes %x count %d index %d\n", __LINE__, bytes, count, index);
    size_t i = 0;
    for (; i < index; i = i + 8 )
    {
        snprintf(row, 256, "%lx %02x %02x %02x %02x %02x %02x %02x %02x\n",
            (long unsigned int)(bytes + i), bytes[i], bytes[i + 1], bytes[i + 2], bytes[i + 3],
            bytes[i + 4], bytes[i + 5], bytes[i + 6], bytes[i + 7]);
        Log.verbose("Line %d row %s",__LINE__, row);
    }

    if (index != count)
    {
        char * row_ptr = row;

        // Ordinary printf is safe here
        row_ptr = row_ptr + sprintf(row_ptr, "%lx ", (long unsigned int)(bytes + i));
        for (; i < count; i++)
        {
            row_ptr = row_ptr + sprintf(row_ptr, "%02x ", bytes[i]);
        }
        Log.verbose("Line %d row %s\n", __LINE__, row);
    }
}

void CInstrument::dumpHexBytes(const uint32_t *bytes, size_t count)
{
    char row[256];
    size_t index = count - (count % 8);
    Log.verbose("Line %d bytes %x count %d index %d\n", __LINE__, bytes, count, index);
    size_t i = 0;
    for (; i < index; i = i + 8 )
    {
        snprintf(row, 256, "%lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
            (long unsigned int)(bytes + i), bytes[i], bytes[i + 1], bytes[i + 2], bytes[i + 3],
            bytes[i + 4], bytes[i + 5], bytes[i + 6], bytes[i + 7]);
        Log.verbose("Line %d row %s",__LINE__, row);
    }

    if (index != count)
    {
        char * row_ptr = row;

        // Ordinary printf is safe here
        row_ptr = row_ptr + sprintf(row_ptr, "%lx ", (long unsigned int)(bytes + i));
        for (; i < count; i++)
        {
            row_ptr = row_ptr + sprintf(row_ptr, "%08lx ", bytes[i]);
        }
        Log.verbose("Line %d row %s\n", __LINE__, row);
    }
}

void CInstrument::dumpInstrumentData(instrument_data_t *instrumentData)
{
        char buf[256];
        snprintf(buf, 256, "Instrument data\n"
                            "===============\n"
                            "Number of samples %d\n"
                            "Sample note ranges pointer %p\n"
                            "Sample metadata pointer %p\n",
                            instrumentData->sample_count,
                            instrumentData->sample_note_ranges,
                            instrumentData->samples
                            );
        Log.verbose("%s", buf);
}
AudioSynthWavetable::instrument_data* CInstrument::load(const char *name)
{
    char buf[256];
    size_t capacity = 50000;

    DynamicJsonDocument doc(capacity);
    DeserializationError retcode;

    if (!SD.begin(chipSelect))
    {
        Log.verbose("Line %d SD card intialisation failed\n", __LINE__);
        return nullptr;
    }

    if (!SD.exists(name))
    {
        Log.verbose("Line %d Cannot find file %s\n", __LINE__, name);
        return nullptr;
    }
    
    Log.verbose("Line %d Opening file %s\n", __LINE__, name);
    File data = SD.open(name);
    if (!data)
    {
        Log.verbose("Line %d Unable to open data file\n", __LINE__);
        return nullptr;
    }

    if ((retcode = deserializeJson(doc, data)) != DeserializationError::Ok)
    {
        Log.verbose("Line %d deserialisation failed %s\n", __LINE__, retcode.f_str());
        return nullptr;
    }

    instrument_data = std::make_unique<instrument_data_t>();

    instrument_data->sample_count = doc["sample_count"];
    JsonArray sample_ranges = doc["sample_note_ranges"].as<JsonArray>();
    JsonArray samples = doc["samples"].as<JsonArray>();

    // Create raw sample data
    std::vector<uint32_t> raw_samples_array;
    std::vector<size_t> sample_offsets_array;
    for (int i = 0; i < instrument_data->sample_count; i++)
    {
        size_t current_sample_offset = raw_samples_array.size();
        sample_offsets_array.push_back(current_sample_offset);
        Log.verbose("Line %d current sample size %d\n", __LINE__, current_sample_offset);
        JsonArray raw_samples = samples[i]["sample"].as<JsonArray>();
        for(JsonVariant v : raw_samples)
            raw_samples_array.push_back(v.as<uint32_t>());
    }
    size_t total_sample_size = raw_samples_array.size();
    Log.verbose("Line %d raw_samples_array size %d\n", __LINE__, total_sample_size);
    raw_sample_data_array =  std::make_unique<uint32_t[]>(total_sample_size);
    for (size_t i = 0; i < total_sample_size; i++)
        raw_sample_data_array[i] = raw_samples_array[i];

    // Create sample note ranges array
    sample_note_ranges_array = std::make_unique<uint8_t[]>(instrument_data->sample_count);

    for (int i = 0; i < instrument_data->sample_count; i++)
    {
        sample_note_ranges_array[i] = (uint8_t)sample_ranges[i];
        Log.verbose("Line %d sample_range[%d] %d\n", __LINE__, i, sample_note_ranges_array[i]);
    }

    // Create sample metadata array
    samples_metadata_array = std::make_unique<my_non_const_sample_metadata[]>(instrument_data->sample_count); 

    for (int i = 0; i < instrument_data->sample_count; i++)
    {
        samples_metadata_array[i].sample = (uint16_t*)&raw_sample_data_array[sample_offsets_array[i]];
        snprintf(buf, 256, "sample %p\n", samples_metadata_array[i].sample);
        Log.verbose("%s", buf);

        samples_metadata_array[i].LOOP = samples[i]["LOOP"];
        samples_metadata_array[i].INDEX_BITS = samples[i]["INDEX_BITS"];
        samples_metadata_array[i].PER_HERTZ_PHASE_INCREMENT = samples[i]["PER_HERTZ_PHASE_INCREMENT"];
        samples_metadata_array[i].MAX_PHASE = samples[i]["MAX_PHASE"];
        samples_metadata_array[i].LOOP_PHASE_END = samples[i]["LOOP_PHASE_END"];
        samples_metadata_array[i].LOOP_PHASE_LENGTH = samples[i]["LOOP_PHASE_LENGTH"];
        samples_metadata_array[i].INITIAL_ATTENUATION_SCALAR = samples[i]["INITIAL_ATTENUATION_SCALAR"];
        samples_metadata_array[i].DELAY_COUNT = samples[i]["DELAY_COUNT"];
        samples_metadata_array[i].ATTACK_COUNT = samples[i]["ATTACK_COUNT"];
        samples_metadata_array[i].HOLD_COUNT = samples[i]["HOLD_COUNT"];
        samples_metadata_array[i].DECAY_COUNT = samples[i]["DECAY_COUNT"];
        samples_metadata_array[i].RELEASE_COUNT = samples[i]["RELEASE_COUNT"];
        samples_metadata_array[i].SUSTAIN_MULT = samples[i]["SUSTAIN_MULT"];
        samples_metadata_array[i].VIBRATO_DELAY = samples[i]["VIBRATO_DELAY"];
        samples_metadata_array[i].VIBRATO_INCREMENT = samples[i]["VIBRATO_INCREMENT"];
        samples_metadata_array[i].VIBRATO_PITCH_COEFFICIENT_INITIAL = samples[i]["VIBRATO_PITCH_COEFFICIENT_INITIAL"];
        samples_metadata_array[i].VIBRATO_PITCH_COEFFICIENT_SECOND = samples[i]["VIBRATO_PITCH_COEFFICIENT_SECOND"];
        samples_metadata_array[i].MODULATION_PITCH_COEFFICIENT_INITIAL = samples[i]["MODULATION_PITCH_COEFFICIENT_INITIAL"];
        samples_metadata_array[i].MODULATION_PITCH_COEFFICIENT_SECOND = samples[i]["MODULATION_PITCH_COEFFICIENT_SECOND"];
        samples_metadata_array[i].MODULATION_AMPLITUDE_INITIAL_GAIN = samples[i]["MODULATION_AMPLITUDE_INITIAL_GAIN"];
        samples_metadata_array[i].MODULATION_AMPLITUDE_SECOND_GAIN = samples[i]["MODULATION_AMPLITUDE_SECOND_GAIN"];
    }

    snprintf(buf, 256, "samples_metadata_array %p\n",
        (void*)samples_metadata_array.get());
    Log.verbose("%s", buf);
    CUtils::dumpSampleMetadata((const struct AudioSynthWavetable::sample_data*)samples_metadata_array.get());

    instrument_data->sample_note_ranges = sample_note_ranges_array.get();
    instrument_data->samples = samples_metadata_array.get();

    dumpInstrumentData(instrument_data.get());
    
    return (AudioSynthWavetable::instrument_data*)instrument_data.get();
}
