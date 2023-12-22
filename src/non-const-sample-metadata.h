/* Audio Library for Teensy 3.X
 * Copyright (c) 2017, TeensyAudio PSU Team
 *
 * Development of this audio library was sponsored by PJRC.COM, LLC.
 * Please support PJRC's efforts to develop open source 
 * software by purchasing Teensy or other PJRC products.
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
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once
#include <stdint.h>
#include <memory>


#define CENTS_SHIFT(C) (pow(2.0, C/1200.0))
#define NOTE(N) (440.0 * pow(2.0, (N - 69) / 12.0))
#define DECIBEL_SHIFT(dB) (pow(10.0, dB/20.0))

struct my_non_const_sample_metadata {
	// SAMPLE VALUES
	uint16_t* sample; // Pointer to raw sample data
	 bool LOOP;
	 int INDEX_BITS;
	 float PER_HERTZ_PHASE_INCREMENT;
	 uint32_t MAX_PHASE;
	 uint32_t LOOP_PHASE_END;
	 uint32_t LOOP_PHASE_LENGTH;
    
	 uint16_t INITIAL_ATTENUATION_SCALAR;
	
	// VOLUME ENVELOPE VALUES
	 uint32_t DELAY_COUNT;
	 uint32_t ATTACK_COUNT;
	 uint32_t HOLD_COUNT;
	 uint32_t DECAY_COUNT;
	 uint32_t RELEASE_COUNT;
	 int32_t SUSTAIN_MULT;

	// VIRBRATO VALUES
	 uint32_t VIBRATO_DELAY;
	 uint32_t VIBRATO_INCREMENT;
	 float VIBRATO_PITCH_COEFFICIENT_INITIAL;
	 float VIBRATO_PITCH_COEFFICIENT_SECOND;

	// MODULATION VALUES
	 uint32_t MODULATION_DELAY;
	 uint32_t MODULATION_INCREMENT;
	 float MODULATION_PITCH_COEFFICIENT_INITIAL;
	 float MODULATION_PITCH_COEFFICIENT_SECOND;
	 int32_t MODULATION_AMPLITUDE_INITIAL_GAIN;
	 int32_t MODULATION_AMPLITUDE_SECOND_GAIN;
};

struct instrument_data_t {

	uint8_t sample_count; // Number of entries in the sample metadata array
	uint8_t* sample_note_ranges;
	my_non_const_sample_metadata* samples;
};
