#include <jo/jo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"

enum routine_state_types options_state = ROUTINE_STATE_INITIALIZE;
int options[OPTIONS_COUNT];

void initOptions()
{
    for(int i=0;i<OPTIONS_COUNT;i++)
    {
        switch(i)
        {
            case OPTIONS_LIST_MODE:
                options[i] = 0;
                break;
            case OPTIONS_AUTO_PATCH:
                options[i] = 0;
                break;
            case OPTIONS_SOUND_VOLUME:
                options[i] = JO_DEFAULT_AUDIO_VOLUME;
                break;
            case OPTIONS_COUNT:
            default:
                break;
        }
    }
    
    char * ini = "options.ini";
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(ini, st, sizeof(statbuf));
    if(fr >= 0)
    {
        centerTextVblank(25, "Loading Options");
        // open options ini for reading
        fr = s_open(ini, FA_READ | FA_OPEN_EXISTING);
        if (fr < 0)
        {
            // change back to the current dir
            s_chdir(currentDirectory);
            return;
        }
        char * oneline = jo_malloc(1024);
        strcpy(oneline, "");
        uint32_t bytes;
        while(strncmp(oneline, "[START]", 7))
        {
            oneline = s_gets(oneline, 1024, fr, &bytes, st->size);
        }
        oneline = s_gets(oneline, 1024, fr, &bytes, st->size);
        while(strncmp(oneline, "[END]", 5))
        {
            if(!strncmp(oneline, "autopatch", 9))
                sscanf(oneline, "autopatch=%d", &options[OPTIONS_AUTO_PATCH]);
            if(!strncmp(oneline, "listmode", 8))
                sscanf(oneline, "listmode=%d", &options[OPTIONS_LIST_MODE]);
            oneline = s_gets(oneline, 1024, fr, &bytes, st->size);
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

    s_write(fw, "[END]", 5);
    s_close(fw);
    jo_free(line);
    s_chdir(currentDirectory);
    return true;
}

char * getListTypeName(int value)
{
    switch(value)
    {
        case GAME_VIEW_TEXT_AND_IMAGE:
            return "List Type <Text and Image>";
        case GAME_VIEW_TEXT_ONLY:
            return "List Type <Text Only>     ";
    }
    return "list type err";
}
static char * getOptionName(enum optionsType option, int value)
{
    static char optionName[40];
    switch(option)
    {
        case OPTIONS_LIST_MODE:
            return getListTypeName(value);
        case OPTIONS_AUTO_PATCH:
            if(value == 0)
                return "Auto Region Patch <Off>";
            else
                return "Auto Region Patch <On> ";
        case OPTIONS_SOUND_VOLUME:
            sprintf(optionName, "Audio Volume <%d>", options[option]); 
            return (char *)optionName;
        case OPTIONS_COUNT:
        default:
            break;
    }
    return "err";
}

void logic_options()
{
    static int selectedMenuOption = 0;
    static enum prog_state_types exit_state = PROG_STATE_GAMELIST;
    switch(options_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            selectedMenuOption = 0;
            clearMenuOptions();
            for(int i=0;i<OPTIONS_COUNT;i++)
            {
                createMenuOption(getOptionName(i, options[i]), PROG_STATE_OPTIONS, OPTION_CHANGE_VARIABLE, 8 + i);
            }
            createMenuOption("Save", PROG_STATE_MENU, OPTION_PROGRAM_STATE, 8 + OPTIONS_COUNT + 1);
            createMenuOption("Cancel", PROG_STATE_MENU, OPTION_PROGRAM_STATE, 8 + OPTIONS_COUNT + 2);
            create_sprite(load_sprite_texture("TEX", "OPTION.TGA"), 0, 4, 1, 1, 1, 0);
            loadSelectionSprite();
            displayMenuOptions(selectedMenuOption);
            displayVersion();
            options_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_MENU;
            break;
        case ROUTINE_STATE_RUN:
            displayTime();
            int changed = controlMenuOptions(&selectedMenuOption, &options_state, &exit_state);  
            if((changed != 0) && (selectedMenuOption < OPTIONS_COUNT))
            {
                switch(selectedMenuOption)
                {
                    case OPTIONS_LIST_MODE:
                        options[selectedMenuOption] += changed;
                        if(options[selectedMenuOption] >= GAME_VIEW_MAX_COUNT)
                            options[selectedMenuOption] = 0;
                        if(options[selectedMenuOption] < 0)
                            options[selectedMenuOption] = GAME_VIEW_MAX_COUNT - 1;
                        break;
                    case OPTIONS_AUTO_PATCH:
                        options[selectedMenuOption] += changed;
                        if(options[selectedMenuOption] < 0)
                            options[selectedMenuOption] = 1;
                        if(options[selectedMenuOption] > 1)
                            options[selectedMenuOption] = 0;
                        break;
                    case OPTIONS_SOUND_VOLUME:
                        options[selectedMenuOption] += changed;
                        if(options[selectedMenuOption] < JO_MIN_AUDIO_VOLUME)
                            options[selectedMenuOption] = JO_MIN_AUDIO_VOLUME;
                        if(options[selectedMenuOption] > JO_MAX_AUDIO_VOLUME)
                            options[selectedMenuOption] = JO_MAX_AUDIO_VOLUME;
                        jo_audio_set_volume(options[selectedMenuOption]);
                        break;
                }
                menuOptions[selectedMenuOption].txt = getOptionName(selectedMenuOption, options[selectedMenuOption]);
                displayMenuOptions(selectedMenuOption);
            }
            if((selectedMenuOption == OPTIONS_COUNT) && (options_state == ROUTINE_STATE_END))
            {
                if(!saveOptions())
                {
                    options_state = ROUTINE_STATE_RUN;
                    displayStatus("Failed To Save Options!");
                } else
                {
                    displayStatus("Options Saved!");
                }
            }
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            options_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}