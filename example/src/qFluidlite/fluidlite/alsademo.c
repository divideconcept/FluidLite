
/*
https://www.linuxjournal.com/article/6735
This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

put in example folder with main.c

maybe delete the .so fluidlite and use static or use "export LD_LIBRARY_PATH=./" before running and place with so file
gcc alsademo.c -L../../ -lfluidlite -lasound -lm -I../../include -o alsademo

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include "fluidlite.h"
#include <alsa/asoundlib.h>

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE sizeof(float)
#define NUM_FRAMES SAMPLE_RATE
#define NUM_CHANNELS 2
#define NUM_SAMPLES (NUM_FRAMES * NUM_CHANNELS)

  fluid_synth_t* synth;
  float* flbuffer;
  int audio_channels;

int main() {
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  char *buffer;


//flstuff
  audio_channels = 2;
  fluid_settings_t* settings = new_fluid_settings();
  synth = new_fluid_synth(settings);
 // fluid_synth_sfload(synth, "../sf_/Boomwhacker.sf2", 1);
  fluid_synth_sfload(synth, "./soundfont.sf2", 1);
//  flbuffer = (float *)calloc( SAMPLE_SIZE, NUM_SAMPLES );
    float* flbuffer = calloc(SAMPLE_SIZE, NUM_SAMPLES);

  fluid_synth_noteon(synth, 0, 80, 127);
 // printf("triggered note\n");
  fluid_synth_noteon(synth, 0, 60, 127);
  printf("triggered note\n");
 // fluid_synth_noteon(synth, 0, 60, 127);
 // printf("triggered note\n");

  /* Open PCM device for playback. */
  rc = snd_pcm_open(&handle, "default",
                    SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0) {
    fprintf(stderr,
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,
                               SND_PCM_FORMAT_S32_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  val = SAMPLE_RATE;
  snd_pcm_hw_params_set_rate_near(handle, params,
                                  &val, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle,
                              params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    fprintf(stderr,
            "unable to set hw parameters: %s\n",
            snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,
                                    &dir);
  size = SAMPLE_SIZE * 4; /* 2 bytes/sample, 2 channels */
  buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,
                                    &val, &dir);
  /* 5 seconds in microseconds divided by
   * period time */
  loops = 1000 / val;


// time to loop the buffer 1 second
  while (loops > 0) {
    loops--;

    fluid_synth_write_float(synth, NUM_SAMPLES, flbuffer, 0, audio_channels, flbuffer, 0, audio_channels );
    rc = snd_pcm_writei(handle, flbuffer, NUM_FRAMES);

  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  return 0;
}


