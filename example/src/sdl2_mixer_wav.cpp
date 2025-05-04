
#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <fluidlite.h>

using namespace std;

#define WAV_PATH "sound.wav"

Mix_Chunk* wave = NULL;

#define SAMPLE_RATE 44100
#define SAMPLE_SIZE 4 //4: Float Buffer   2: Signed Int Buffer
#define NUM_FRAMES SAMPLE_RATE
#define NUM_CHANNELS 2
#define NUM_SAMPLES (NUM_FRAMES * NUM_CHANNELS)
#define TIME_INTERVAL 1000000 //1500000:duration us

//-L../../ -lfluidlite

//gcc sdl2_mixer_wav.cpp -lSDL2 -lfluidlite -I../../include -L../../ -lstdc++ -lSDL2_mixer


int main(int argc, char* argv[])
{
    fluid_settings_t* settings = new_fluid_settings();
    fluid_synth_t* synth = new_fluid_synth(settings);
    int res = fluid_synth_sfload(synth, "sitar.sf2", 1);

    if (res <= 0)
    {
        printf("Could not soundfont.sf2\n");
        return -1;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return -1;

    if (Mix_OpenAudio(SAMPLE_RATE, MIX_DEFAULT_FORMAT, NUM_CHANNELS, 1024) == -1)
        return -1;


    double dlength = (double)(SAMPLE_RATE * NUM_CHANNELS * SAMPLE_SIZE) * TIME_INTERVAL / 1000000;
    long length = (long)dlength;

    wave = new Mix_Chunk();
    wave->allocated = 1;
    wave->alen = length;
    wave->abuf = (Uint8*)calloc(1, length);
    wave->volume = 128;

    cout << "Play Start!\n";

    while (1)
    {
        int nKey = 60 + rand() % 30;
        fluid_synth_noteon(synth, 0, nKey, 127);
        fluid_synth_write_float(synth, SAMPLE_RATE, wave->abuf, 0, NUM_CHANNELS, wave->abuf, 1, NUM_CHANNELS);   
        fluid_synth_noteoff(synth, 0, 60);

        while (1)
        {
            if (!Mix_Playing(-1)) break;
        }

        if (Mix_PlayChannel(-1, wave, 0) == -1)
            break;

    }
  
    cout << "Sound play End\n";

    free(wave->abuf);
    delete wave;

    SDL_Quit();

    delete_fluid_synth(synth);
    delete_fluid_settings(settings);

    return 0;
}

//int main()
//{
//    std::cout << "Hello World!\n";
//
//    return 0;
//}
