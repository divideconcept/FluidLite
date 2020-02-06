# FluidLite

[![License: LGPL-2.1](https://img.shields.io/badge/License-LGPL--2.1-brightgreen.svg)](https://opensource.org/licenses/LGPL-2.1)
[![Travis-CI Status](https://travis-ci.com/katyo/fluidlite.svg?branch=master)](https://travis-ci.com/katyo/fluidlite)

FluidLite (c) 2016 Robin Lobel

FluidLite is a very light version of FluidSynth
designed to be hardware, platform and external dependency independant.
It only uses standard C libraries.

It also adds support for SF3 files (SF2 files compressed with ogg vorbis)
and an additional setting to remove the constraint of channel 9 (drums):
fluid_settings_setstr(settings, "synth.drums-channel.active", "no");
you can still select bank 128 on any channel to use drum kits.

FluidLite keeps very minimal functionnalities (settings and synth),
therefore MIDI file reading, realtime MIDI events and audio output must be
implemented externally.

## Config

By default SF3 support is disabled. To enable it use `-DENABLE_SF3=YES` with cmake.

Alternatively it can be configured to use [stb_vorbis](https://github.com/nothings/stb) to decompress SF3 instead of Xiph's [libogg](https://github.com/xiph/ogg)/[libvorbis](https://github.com/xiph/vorbis). You can pass `-DSTB_VORBIS=YES` to cmake to do it.

## Usage

```c
#include <stdlib.h>
#include <stdio.h>

#include "fluidlite.h"

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE sizeof(float)
#define NUM_FRAMES SAMPLE_RATE
#define NUM_CHANNELS 2
#define NUM_SAMPLES (NUM_FRAMES * NUM_CHANNELS)

int main() {
    fluid_settings_t* settings = new_fluid_settings();
    fluid_synth_t* synth = new_fluid_synth(settings);
    fluid_synth_sfload(synth, "soundfont.sf2", 1);

    float* buffer = calloc(SAMPLE_SIZE, NUM_SAMPLES);

    FILE* file = fopen("float32output.pcm", "wb");

    fluid_synth_noteon(synth, 0, 60, 127);
    fluid_synth_write_float(synth, NUM_FRAMES, buffer, 0, NUM_CHANNELS, buffer, 1, NUM_CHANNELS);
    fwrite(buffer, SAMPLE_SIZE, NUM_SAMPLES, file);

    fluid_synth_noteoff(synth, 0, 60);
    fluid_synth_write_float(synth, NUM_FRAMES, buffer, 0, NUM_CHANNELS, buffer, 1, NUM_CHANNELS);
    fwrite(buffer, SAMPLE_SIZE, NUM_SAMPLES, file);

    fclose(file);

    free(buffer);

    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}
```
