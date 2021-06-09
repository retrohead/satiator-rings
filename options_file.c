#include <jo/jo.h>
#include <string.h>
#include <stdio.h>
#include "satiator_functions.h"
#include "options_file.h"
#include "debug.h"
#include "theme.h"
#include "ini.h"
#include "states/routine_states.h"

int options[OPTIONS_COUNT];
char loadedThemeName[32];

void initOptions()
{
    for(enum optionsType i=0;i<OPTIONS_COUNT;i++)
    {
        switch(i)
        {
            case OPTIONS_LIST_MODE:
                options[i] = 0;
                break;
            case OPTIONS_LIST_CATEGORY:
                options[i] = 0;
                break;
            case OPTIONS_AUTO_PATCH:
                options[i] = 0;
                break;
            case OPTIONS_DESC_CACHE:
                options[i] = 0;
                break;
            case OPTIONS_SOUND_VOLUME:
                options[i] = JO_DEFAULT_AUDIO_VOLUME;
                break;
            case OPTIONS_SKIP_SPLASH:
                options[i] = 0;
                break;
            case OPTIONS_PERGAME_SAVE:
                options[i] = 0;
                break;
            case OPTIONS_COUNT:
                break;
        }
    }
    strcpy(loadedThemeName, "default");
    initTheme();
}
void loadOptions(bool firstInit)
{
    initOptions();
    char * ini = "options.ini";
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(ini, st, sizeof(statbuf));
    if(fr >= 0)
    {
        // open options ini for reading
        fr = s_open(ini, FA_READ | FA_OPEN_EXISTING);
        if (fr < 0)
        {
            // change back to the current dir
            s_chdir(currentDirectory);
            return;
        }
        char * oneline = jo_malloc(ONE_LINE_MAX_LEN);
        strcpy(oneline, "");
        uint32_t bytes = 0;
        while(strncmp(oneline, "[START]", 7))
        {
            oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fr, &bytes, st->size);
        }
        oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fr, &bytes, st->size);
        while(strncmp(oneline, "[THEME]", 7))
        {
            if(!strncmp(oneline, "autopatch", 9))
                sscanf(oneline, "autopatch=%d", &options[OPTIONS_AUTO_PATCH]);
            if(!strncmp(oneline, "listmode", 8))
                sscanf(oneline, "listmode=%d", &options[OPTIONS_LIST_MODE]);
            if(!strncmp(oneline, "listcat", 7))
                sscanf(oneline, "listcat=%d", &options[OPTIONS_LIST_CATEGORY]);
            if(!strncmp(oneline, "volume", 6))
                sscanf(oneline, "volume=%d", &options[OPTIONS_SOUND_VOLUME]);
            if(!strncmp(oneline, "desccache", 9))
                sscanf(oneline, "desccache=%d", &options[OPTIONS_DESC_CACHE]);
            if(!strncmp(oneline, "skipsplash", 10))
                sscanf(oneline, "skipsplash=%d", &options[OPTIONS_SKIP_SPLASH]);
            if(!strncmp(oneline, "pergamesave", 11))
                sscanf(oneline, "pergamesave=%d", &options[OPTIONS_PERGAME_SAVE]);
            oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fr, &bytes, st->size);
            if(!strncmp(oneline, "[END]", 5))
                break;
        }
        if(!firstInit || (options[OPTIONS_SKIP_SPLASH] == 1))
        {
            while(strncmp(oneline, "[END]", 5))
            {
                if(!strncmp(oneline, "name", 4))
                    sscanf(oneline, "name=%s", loadedThemeName);
                if(!strncmp(oneline, "font", 4))
                    sscanf(oneline, "font=%d,%d,%d", &loadedTheme.colours[PAL_COL_FONT].r, &loadedTheme.colours[PAL_COL_FONT].g, &loadedTheme.colours[PAL_COL_FONT].b);
                if(!strncmp(oneline, "bg", 2))
                    sscanf(oneline, "bg=%d,%d,%d", &loadedTheme.colours[PAL_COL_BG].r, &loadedTheme.colours[PAL_COL_BG].g, &loadedTheme.colours[PAL_COL_BG].b);
                if(!strncmp(oneline, "selector", 8))
                    sscanf(oneline, "selector=%d,%d,%d", &loadedTheme.colours[PAL_COL_SELECTOR].r, &loadedTheme.colours[PAL_COL_SELECTOR].g, &loadedTheme.colours[PAL_COL_SELECTOR].b);
                if(!strncmp(oneline, "boxbg", 5))
                    sscanf(oneline, "boxbg=%d,%d,%d", &loadedTheme.colours[PAL_COL_BOX_BG].r, &loadedTheme.colours[PAL_COL_BOX_BG].g, &loadedTheme.colours[PAL_COL_BOX_BG].b);
                oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fr, &bytes, st->size);
                themeLoaded = true;
            }            
        } 
        s_close(fr);
        jo_free(oneline);
    }
    // change back to the current dir
    s_chdir(currentDirectory);

    // set the option values where needed
    jo_audio_set_volume(options[OPTIONS_SOUND_VOLUME]);
}

bool saveOptions()
{
    char * ini = "options.ini";
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(ini, st, sizeof(statbuf));
    if(fr >= 0)
    {
        // file already exists, delete
        s_unlink(ini);
    }

    // open new favs ini for writing
    int fw = s_open(ini, FA_WRITE | FA_CREATE_NEW);
    if (fw < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        return false;
    }
    s_write(fw, "[START]\r\n", 9);
    char * line = jo_malloc(256);

    sprintf(line, "autopatch=%d\r\n", options[OPTIONS_AUTO_PATCH]);
    s_write(fw, line, strlen(line));
    
    sprintf(line, "listmode=%d\r\n", options[OPTIONS_LIST_MODE]);
    s_write(fw, line, strlen(line));
    
    sprintf(line, "listcat=%d\r\n", options[OPTIONS_LIST_CATEGORY]);
    s_write(fw, line, strlen(line));
    
    sprintf(line, "volume=%d\r\n", options[OPTIONS_SOUND_VOLUME]);
    s_write(fw, line, strlen(line));
    
    sprintf(line, "desccache=%d\r\n", options[OPTIONS_DESC_CACHE]);
    s_write(fw, line, strlen(line));
    
    sprintf(line, "skipsplash=%d\r\n", options[OPTIONS_SKIP_SPLASH]);
    s_write(fw, line, strlen(line));

    sprintf(line, "pergamesave=%d\r\n", options[OPTIONS_PERGAME_SAVE]);
    s_write(fw, line, strlen(line));

    s_write(fw, "[THEME]\r\n", 9);
    sprintf(line, "name=%s\r\n", loadedThemeName);
    s_write(fw, line, strlen(line));
    sprintf(line, "font=%d,%d,%d\r\n", loadedTheme.colours[PAL_COL_FONT].r, loadedTheme.colours[PAL_COL_FONT].g, loadedTheme.colours[PAL_COL_FONT].b);
    s_write(fw, line, strlen(line));
    sprintf(line, "bg=%d,%d,%d\r\n", loadedTheme.colours[PAL_COL_BG].r, loadedTheme.colours[PAL_COL_BG].g, loadedTheme.colours[PAL_COL_BG].b);
    s_write(fw, line, strlen(line));
    sprintf(line, "selector=%d,%d,%d\r\n", loadedTheme.colours[PAL_COL_SELECTOR].r, loadedTheme.colours[PAL_COL_SELECTOR].g, loadedTheme.colours[PAL_COL_SELECTOR].b);
    s_write(fw, line, strlen(line));
    sprintf(line, "boxbg=%d,%d,%d\r\n", loadedTheme.colours[PAL_COL_BOX_BG].r, loadedTheme.colours[PAL_COL_BOX_BG].g, loadedTheme.colours[PAL_COL_BOX_BG].b);
    s_write(fw, line, strlen(line));

    s_write(fw, "[END]", 5);
    s_close(fw);
    jo_free(line);
    s_chdir(currentDirectory);
    return true;
}
