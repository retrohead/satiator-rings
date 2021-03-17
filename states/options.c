#include <jo/jo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"

enum routine_state_types options_state = ROUTINE_STATE_INITIALIZE;

char * getListTypeName(int value)
{
    switch(value)
    {
        case GAME_VIEW_TEXT_AND_IMAGE:
            return "       Game List Type    <Text/Image>";
        case GAME_VIEW_TEXT_ONLY:
            return "       Game List Type    <Text Only> ";
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
        case OPTIONS_DESC_CACHE:
            if(value == 0)
                return "Desc File Caching <Off>";
            else
                return "Desc File Caching <On> ";
        case OPTIONS_SOUND_VOLUME:
            sprintf(optionName, "Audio Volume      <%d>", options[option]); 
            return (char *)optionName;
        case OPTIONS_COUNT:
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
            createGuiBoxes("OPTION.TGA", true);
            clearMenuOptions();
            for(int i=0;i<OPTIONS_COUNT;i++)
            {
                createMenuOption(getOptionName(i, options[i]), PROG_STATE_OPTIONS, OPTION_CHANGE_VARIABLE, 8 + i);
            }
            createMenuOption("Theme Selection", PROG_STATE_THEME, OPTION_PROGRAM_STATE, 8 + OPTIONS_COUNT);
            createMenuOption("Save", PROG_STATE_MENU, OPTION_PROGRAM_STATE, 8 + OPTIONS_COUNT + 2);
            createMenuOption("Cancel", PROG_STATE_MENU, OPTION_PROGRAM_STATE, 8 + OPTIONS_COUNT + 3);
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
                switch((enum optionsType)selectedMenuOption)
                {
                    case OPTIONS_LIST_MODE:
                        options[selectedMenuOption] += changed;
                        if(options[selectedMenuOption] >= GAME_VIEW_MAX_COUNT)
                            options[selectedMenuOption] = 0;
                        if(options[selectedMenuOption] < 0)
                            options[selectedMenuOption] = GAME_VIEW_MAX_COUNT - 1;
                        break;
                    case OPTIONS_AUTO_PATCH:
                    case OPTIONS_DESC_CACHE:
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
                    case OPTIONS_COUNT:
                        break;
                }
                strcpy(menuOptions[selectedMenuOption].txt, getOptionName(selectedMenuOption, options[selectedMenuOption]));
                displayMenuOptions(selectedMenuOption);
            }
            if((selectedMenuOption == OPTIONS_COUNT + 1) && (options_state == ROUTINE_STATE_END))
            {
                if(!saveOptions())
                {
                    options_state = ROUTINE_STATE_RUN;
                    displayStatus("Failed To Save Options!");
                } else
                {
                    displayStatus("Options Saved!");
                }
            } else if((exit_state == PROG_STATE_MENU) && (options_state == ROUTINE_STATE_END))
            {
                displayStatus("Reloading Options");
                initOptions();
                applyTheme();
            }
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            options_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}