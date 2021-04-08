#include <jo/jo.h>
#include "satiator_functions.h"
#include "sound.h"
#include "states/options.h"
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

bool load_pcm_satiator(const char * directory, const char * filename, const jo_sound_mode mode, jo_sound *sound)
{
    if(directory != NULL)
        s_chdir(directory);
    // try stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(filename, st, sizeof(statbuf));
    if (fr < 0)
    {
        if(directory != NULL)
            s_chdir(currentDirectory);
        return -1;
    }
    // allocate a buffer for the texture
    char * buf = (char *)jo_malloc_with_behaviour((st->size + 1) * sizeof(*buf), JO_MALLOC_TRY_REUSE_BLOCK);
    
    // try open the file
    fr = s_open(filename, FA_READ | FA_OPEN_EXISTING);
    if (fr < 0)
    {
        if(directory != NULL)
            s_chdir(currentDirectory);
        return -1;
    }

    uint32_t readBytes = 0;
    while(readBytes < st->size)
    {
        uint32_t readSize = S_MAXBUF;
        if(st->size - readBytes < readSize)
            readSize = st->size - readBytes;
        int ret = s_read(fr, buf + readBytes, readSize);
        if(ret != (int)readSize)
        {
            jo_free(buf);
            s_close(fr);
            if(directory != NULL)
                s_chdir(currentDirectory);
            return -1;
        }
        readBytes += readSize;
    }
    s_close(fr);
    if(directory != NULL)
        s_chdir(currentDirectory);

    if(buf == JO_NULL)
        return false;
    JO_ZERO(buf[st->size]);
    sound->mode = mode;
    sound->data_length = st->size;
    sound->data = buf;
    sound->volume = JO_MAX_AUDIO_VOLUME;
    return true;
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
            load_pcm_satiator("/satiator-rings/sfx", "SELECT.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_MOVE:
            load_pcm_satiator("/satiator-rings/sfx", "MOVE.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
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
        case SFX_SLIDE:
            load_pcm_satiator("/satiator-rings/sfx", "SLIDE.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_CHANGE:
            load_pcm_satiator("/satiator-rings/sfx", "CHANGE.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_BACK:
            load_pcm_satiator("/satiator-rings/sfx", "BACK.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case SFX_OPTION:
            load_pcm_satiator("/satiator-rings/sfx", "OPTION.PCM", JoSoundMono8Bit, &sfx[sfxId].sfx);
            break;
        case MAX_SFX:
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