#include <jo/jo.h>
#include "satiator_functions.h"
#include "sound.h"
#include "states/options.h"

#define MAX_SFX 5
#define SFX_FRAME_DELAY 6

typedef struct 
{
    jo_sound     sfx;
    bool used;
    int playDelay;
    enum sfxType type;
} sfxDataType;
sfxDataType sfx[MAX_SFX];

void initSfx()
{
    for(int i=0;i<MAX_SFX;i++)
    {
        sfx[i].used = false;
        sfx[i].playDelay = 0;
    }
}
void loadSfx(enum sfxType type)
{
    int sfxId = -1;
    for(int i=0;i<MAX_SFX;i++)
    {
        if(!sfx[i].used)
        {
            sfx[i].used = true;
            sfxId = i;
            break;
        }
    }
    if(sfxId == -1)
        return; // could not find a free sound effect slot

    sfx[sfxId].type = type;
    switch(type)
    {
        case SFX_SELECT:
            jo_audio_load_pcm("SELECT.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_MOVE:
            jo_audio_load_pcm("MOVE.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_INTRO:
            jo_audio_load_pcm("INTRO.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_THUD:
            jo_audio_load_pcm("THUD.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_SANSHIRO:
            jo_audio_load_pcm("SANSH.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
    }
}
void processSfxDelays()
{
    for(int i=0;i<MAX_SFX;i++)
    {
        if(!sfx[i].used)
            continue;

        if(sfx[i].playDelay > 0)
            sfx[i].playDelay--;
    }
}
void playSfx(enum sfxType type, bool activateDelay)
{
    int sfxId = -1;
    for(int i=0;i<MAX_SFX;i++)
    {
        if((sfx[i].used) && (sfx[i].type == type))
        {
            sfxId = i;
            break;
        }
    }
    if(sfxId == -1)
        return; // could not find the sound effect loaded in memory
    if(sfx[sfxId].playDelay > 0)
        return;
    jo_audio_play_sound(&sfx[sfxId].sfx);
    if(activateDelay)
        sfx[sfxId].playDelay = SFX_FRAME_DELAY;
}
void freeSfx()
{
    for(int i=0;i<MAX_SFX;i++)
    {
        if(sfx[i].used)
        {
            jo_audio_free_pcm(&sfx[i].sfx);
            sfx[i].used = false;
        }
    }
}