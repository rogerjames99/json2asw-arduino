#include <Arduino.h>
#include <ArduinoLog.h>
#undef CR
#include <map>
#include <Audio.h>
#include <cstdint>
#include <math.h>
#include "json2asw.h"
#include "pugwash.h"
#include "mylog.h"

// Include irish2 samples for comparison
//#include "../../../tools/irish2_samples.h"
//#include "../../../tools/irish2_samples.cpp"

// ctroger: begin automatically generated code
// the following JSON string contains the whole project, 
// it's included in all generated files.
// JSON string:[{"type":"settings","data":{"main":{},"arduino":{"ExportMode":1,"useExportDialog":true,"ProjectName":"ctroger","Board":{"Platform":"","Board":"","Options":""}},"BiDirDataWebSocketBridge":{},"workspaces":{},"sidebar":{},"palette":{},"editor":{},"devTest":{},"IndexedDBfiles":{"testFileNames":"testFile.txt"},"NodeDefGenerator":{},"NodeDefManager":{},"NodeHelpManager":{},"OSC":{}}},{"type":"tab","id":"Main","label":"Main","nodes":[],"links":[],"export":true,"isMain":false,"mainNameType":"tabName","mainNameExt":".ino","isAudioMain":false,"generateCppDestructor":false,"extraClassDeclarations":"","settings":{"scaleFactor":1.5}},{"id":"20231109T182443_082Z_4c5a","type":"AudioSynthWaveformSine","name":"sine","comment":"","arraySize":1,"x":410,"y":570,"z":"Main","bgColor":"#E6E0F8","wires":[["20231109T190836_796Z_a905:0","20231109T190844_158Z_e40b:0"]]},{"id":"20231109T160656_937Z_5741","type":"AudioSynthWavetable","name":"wavetable","comment":"","arraySize":1,"x":410,"y":620,"z":"Main","bgColor":"#E6E0F8","wires":[["20231109T190844_158Z_e40b:1","20231109T190836_796Z_a905:1"]]},{"id":"20231109T190836_796Z_a905","type":"AudioMixer","name":"mixer","comment":"","arraySize":1,"inputs":2,"ExtraInputs":0,"RealInputs":2,"x":585,"y":575,"z":"Main","bgColor":"#E6E0F8","wires":[["20231109T160844_970Z_b479:0"]]},{"id":"20231109T190844_158Z_e40b","type":"AudioMixer","name":"mixer1","comment":"","arraySize":1,"inputs":2,"ExtraInputs":0,"RealInputs":2,"x":585,"y":610,"z":"Main","bgColor":"#E6E0F8","wires":[["20231109T160844_970Z_b479:1"]]},{"id":"20231109T160844_970Z_b479","type":"AudioOutputMQS","name":"mqs","comment":"","x":803.3333333333334,"y":595,"z":"Main","bgColor":"#E6E0F8","wires":[]}]

/* This is a modified variant of the mixer code
 * to make it possible to autogenerate mixers with any size
 * by using the Design Tool++ by Manicksan (Jannik Svensson)
 * 
 * Original copyright note:
 * Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#include "AudioStream.h"

#if defined(__ARM_ARCH_7EM__)
#define MIXERS_MAX_MULT_I 65536
#define MIXERS_MAX_MULT_F 65536.0f
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef mixers_h_
#define mixers_h_

#include "Arduino.h"
#include "AudioStream.h"

#if defined(__ARM_ARCH_7EM__)
#define MIXERS_MAX_MULT_I 65536
#define MIXERS_MAX_MULT_F 65536.0f
#define MIXERS_MIN_GAIN -32767.0f
#define MIXERS_MAX_GAIN 32767.0f
#define MIXERS_MULT_TYPE int32_t
#elif defined(KINETISL)
#define MIXERS_MAX_MULT_I 256
#define MIXERS_MAX_MULT_F 256.0f
#define MIXERS_MIN_GAIN -127.0f
#define MIXERS_MAX_GAIN 127.0f
#define MIXERS_MULT_TYPE int16_t
#endif

class AudioMixer2 : public AudioStream
{
public:
	AudioMixer2(void) : AudioStream(2, inputQueueArray) {
		for (int i=0; i<2; i++) multiplier[i] = MIXERS_MAX_MULT_I;
	}
	virtual void update(void);
	void gain(unsigned int channel, float gain) {
		if (channel >= 2) return;
		if (gain > MIXERS_MAX_GAIN) gain = MIXERS_MAX_GAIN;
		else if (gain < MIXERS_MIN_GAIN) gain = MIXERS_MIN_GAIN;
		multiplier[channel] = gain * MIXERS_MAX_MULT_F; // TODO: proper roundoff?
	}
	void gain(float gain) {
	    if (gain > MIXERS_MAX_GAIN) gain = MIXERS_MAX_GAIN;
		else if (gain < MIXERS_MIN_GAIN) gain = MIXERS_MIN_GAIN;
		for (int i=0; i<2; i++) multiplier[i] = gain * MIXERS_MAX_MULT_F;
	}
private:
	MIXERS_MULT_TYPE multiplier[2];
	audio_block_t *inputQueueArray[2];
};


#endif
#include <Arduino.h>

#include "utility/dspinst.h"

#if defined(__ARM_ARCH_7EM__)
#define MULTI_UNITYGAIN 65536

static void applyGain(int16_t *data, int32_t mult)
{
	uint32_t *p = (uint32_t *)data;
	const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

	do {
		uint32_t tmp32 = *p; // read 2 samples from *data
		int32_t val1 = signed_multiply_32x16b(mult, tmp32);
		int32_t val2 = signed_multiply_32x16t(mult, tmp32);
		val1 = signed_saturate_rshift(val1, 16, 0);
		val2 = signed_saturate_rshift(val2, 16, 0);
		*p++ = pack_16b_16b(val2, val1);
	} while (p < end);
}

static void applyGainThenAdd(int16_t *data, const int16_t *in, int32_t mult)
{
	uint32_t *dst = (uint32_t *)data;
	const uint32_t *src = (uint32_t *)in;
	const uint32_t *end = (uint32_t *)(data + AUDIO_BLOCK_SAMPLES);

	if (mult == MULTI_UNITYGAIN) {
		do {
			uint32_t tmp32 = *dst;
			*dst++ = signed_add_16_and_16(tmp32, *src++);
			tmp32 = *dst;
			*dst++ = signed_add_16_and_16(tmp32, *src++);
		} while (dst < end);
	} else {
		do {
			uint32_t tmp32 = *src++; // read 2 samples from *data
			int32_t val1 = signed_multiply_32x16b(mult, tmp32);
			int32_t val2 = signed_multiply_32x16t(mult, tmp32);
			val1 = signed_saturate_rshift(val1, 16, 0);
			val2 = signed_saturate_rshift(val2, 16, 0);
			tmp32 = pack_16b_16b(val2, val1);
			uint32_t tmp32b = *dst;
			*dst++ = signed_add_16_and_16(tmp32, tmp32b);
		} while (dst < end);
	}
}

#elif defined(KINETISL)
#define MULTI_UNITYGAIN 256

static void applyGain(int16_t *data, int32_t mult)
{
	const int16_t *end = data + AUDIO_BLOCK_SAMPLES;

	do {
		int32_t val = *data * mult;
		*data++ = signed_saturate_rshift(val, 16, 0);
	} while (data < end);
}

static void applyGainThenAdd(int16_t *dst, const int16_t *src, int32_t mult)
{
	const int16_t *end = dst + AUDIO_BLOCK_SAMPLES;

	if (mult == MULTI_UNITYGAIN) {
		do {
			int32_t val = *dst + *src++;
			*dst++ = signed_saturate_rshift(val, 16, 0);
		} while (dst < end);
	} else {
		do {
			int32_t val = *dst + ((*src++ * mult) >> 8); // overflow possible??
			*dst++ = signed_saturate_rshift(val, 16, 0);
		} while (dst < end);
	}
}

#endif

void AudioMixer2::update(void)
{
	audio_block_t *in, *out=NULL;
	unsigned int channel;

	for (channel=0; channel < 2; channel++) {
		if (!out) {
			out = receiveWritable(channel);
			if (out) {
				int32_t mult = multiplier[channel];
				if (mult != MULTI_UNITYGAIN) applyGain(out->data, mult);
			}
		} else {
			in = receiveReadOnly(channel);
			if (in) {
				applyGainThenAdd(out->data, in->data, multiplier[channel]);
				release(in);
			}
		}
	}
	if (out) {
		transmit(out);
		release(out);
	}
}

class Main 
{
public:
    AudioSynthWaveformSine          sine;
    AudioSynthWavetable           wavetable;
    AudioMixer2                     mixer;
    AudioMixer2                     mixer1;
    AudioOutputMQS                  mqs;
    AudioConnection                  *patchCord[6]; // total patchCordCount:6 including array typed ones.

// constructor (this is called when class-object is created)
    Main() { 
        int pci = 0; // used only for adding new patchcords


        patchCord[pci++] = new AudioConnection(sine, 0, mixer, 0);
        patchCord[pci++] = new AudioConnection(sine, 0, mixer1, 0);
        patchCord[pci++] = new AudioConnection(wavetable, 0, mixer1, 1);
        patchCord[pci++] = new AudioConnection(wavetable, 0, mixer, 1);
        patchCord[pci++] = new AudioConnection(mixer, 0, mqs, 0);
        patchCord[pci++] = new AudioConnection(mixer1, 0, mqs, 1);
        
    }
}; // end of class Main
// ctroger: end automatically generated code

Main audioObjects;

void setup()
{
#ifndef LOGVERBOSE
    while (!Serial);
#endif
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    LOGVERBOSE("Initialising audio memory\n");
    AudioMemory(400);
    LOGVERBOSE("Disabling audio interrupts\n");
    AudioNoInterrupts();
    LOGVERBOSE("Preparing to play a sine wave\n");
    audioObjects.mixer.gain(0, 1.0);
    audioObjects.mixer.gain(1, 0.0);
    audioObjects.mixer1.gain(0, 1.0);
    audioObjects.mixer1.gain(1, 0.0);
    audioObjects.sine.amplitude(1.0);
    audioObjects.sine.frequency(440.0);
    LOGVERBOSE("Enabling audio interrupts\n");
    AudioInterrupts();
    LOGVERBOSE("Playing a sine wave for 2 seconds\n");
    delay(2000);
    LOGVERBOSE("Disabling audio interrupts\n");
    AudioNoInterrupts();
    LOGVERBOSE("Attempting to load the test data\n");
    AudioSynthWavetable::instrument_data* new_instrument = CInstrument::getInstance()->load("instrument.json");
    if (nullptr == new_instrument)
    {
        LOGVERBOSE("Failed to load test data\n");
    }
    else
    {
        audioObjects.wavetable.setInstrument(*new_instrument);
    }
    audioObjects.mixer.gain(0, 0.0);
    audioObjects.mixer.gain(1, 1.0);
    audioObjects.mixer1.gain(0, 0.0);
    audioObjects.mixer1.gain(1, 1.0);
    audioObjects.wavetable.amplitude(1.0);
    LOGVERBOSE("Enabling audio interrupts\n");
    AudioInterrupts();
}

void loop()
{
    static bool first_time = true;
    if (first_time)
    {
        first_time = false;
        
/*
        LOGVERBOSE("Playing a note for ten seconds\n");
        audioObjects.wavetable.playFrequency(440.0);
        delay(10000);
        LOGVERBOSE("Stopping the note\n");
        audioObjects.wavetable.stop();
*/

    // Play the tune
        typedef const struct note_key
        {
            int channel;
            int note;

            bool operator<(const note_key k1) const
            {
                if (k1.note < note && k1.channel < channel)
                    return true;
                else
                    return false;
            }
        } note_key_t;

        std::map<note_key_t, melody_t> note_on_map;
        int notes = sizeof(pugwash)/sizeof(melody_t);
        LOGVERBOSE("Playing Trumpet Hornpipe (Captain Pugwash}\n");

        for (int i = 0; i < notes; i++)
        {
            // For pugwash ignore everything except channel 1
            if (pugwash[i].channel != 1)
                continue;
            note_key_t note_key = {pugwash[i].channel, pugwash[i].note};
            if (pugwash[i].note_on)
                // Note on
                note_on_map[note_key] = pugwash[i];
            else
            {
                // Note off
                auto melody = note_on_map.find(note_key);
                if (melody != note_on_map.end())
                {
                    // Found match
                    //LOGVERBOSE("%s %s %d About to play note %d at index %d\n", __func__, __FILE__, __LINE__, note_key.note, i);
                    audioObjects.wavetable.playNote(note_key.note);
                    delay(int(float(pugwash[i].duration) * 1.5));
                    audioObjects.wavetable.stop();
                }
            }
        }
        LOGVERBOSE("Finished\n");
    }
    else
        delay(1000);
}

