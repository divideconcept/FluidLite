
#include <iostream>

#include "SDL/SDL.h"
#include "fluidlite.h"

//based on code found from http://blog.csdn.net/leixiaohua1020
//Simplest Audio Play SDL2 (SDL2 play PCM) 

//gcc sdldemo.cpp -L../../ -lfluidlite -lSDL -lm -I../../include -o sdldemo -lstdc++

using namespace std;

//Buffer:
//|-----------|-------------|
//chunk-------pos---len-----|
static  Uint8* audio_chunk;
static  Uint32  audio_len;
static  Uint8* audio_pos;

/* Audio Callback
 * The audio function callback takes the following parameters:
 * stream: A pointer to the audio buffer to be filled
 * len: The length (in bytes) of the audio buffer
 *
*/
void  fill_audio(void* udata, Uint8* stream, int len) 
{
	//SDL 2.0
	SDL_memset(stream, 0, len);
	if (audio_len == 0)
		return;
	len = (len > (int)audio_len ? audio_len : len);

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}


#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 2 //4: Float Buffer   2: Signed Int Buffer
#define NUM_FRAMES SAMPLE_RATE
#define NUM_CHANNELS 1
#define NUM_SAMPLES (NUM_FRAMES * NUM_CHANNELS)
#define TIME_INTERVAL 1000000 //1500000:duration us

int main(int argc, char* argv[])
{
	fluid_settings_t* settings = new_fluid_settings();
	fluid_synth_t* synth = new_fluid_synth(settings);
	int res = fluid_synth_sfload(synth, "soundfont.sf2", 1);

	if (res <= 0)
	{
		printf("Could not soundfont.sf2\n");
		return -1;		
	}

	//Init
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}

	cout << "Play Start!\n";	


	double dlength =  (double)(SAMPLE_RATE * NUM_CHANNELS * SAMPLE_SIZE) * TIME_INTERVAL / 1000000;
	long length = (long)dlength;

	//SDL_AudioSpec
	SDL_AudioSpec wanted_spec;
	wanted_spec.freq = SAMPLE_RATE;	
	wanted_spec.channels = NUM_CHANNELS;
	wanted_spec.silence = 0;
	wanted_spec.samples = length;
	wanted_spec.callback = fill_audio;

//	if (SAMPLE_SIZE == 4)
	//	wanted_spec.format = AUDIO_F32SYS;
	//else if (SAMPLE_SIZE == 2)
		wanted_spec.format = AUDIO_S16SYS;	

	if (SDL_OpenAudio(&wanted_spec, NULL) < 0) 
	{
		printf("can't open audio.\n");
		return -1;
	}

	char* lpBuffer = (char*)calloc(1, length);

	//Play
	SDL_PauseAudio(0);
	while (1) 
	{
		int nKey = 60 + rand() % 30;

		fluid_synth_noteon(synth, 0, nKey, 127);

		//if (wanted_spec.format == AUDIO_F32SYS)
		//	fluid_synth_write_float(synth, SAMPLE_RATE, lpBuffer, 0, NUM_CHANNELS, lpBuffer, 1, NUM_CHANNELS);
		//else if (wanted_spec.format == AUDIO_S16SYS)
			fluid_synth_write_s16(synth, SAMPLE_RATE, lpBuffer, 0, NUM_CHANNELS, lpBuffer, 1, NUM_CHANNELS);

		fluid_synth_noteoff(synth, 0, 60);

		//Set audio buffer (PCM data)
		audio_chunk = (Uint8*)lpBuffer;
		//Audio buffer length
		audio_len = length;
		audio_pos = audio_chunk;

		while (audio_len > 0)//Wait until finish
			SDL_Delay(1);
	}
	free(lpBuffer);
	SDL_Quit();

	return 0;
}
