#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../main.h"
#include "../ini.h"
#include "routine_states.h"
#include "../satiator_functions.h"

enum routine_state_types theme_list_state = ROUTINE_STATE_INITIALIZE;


int themeFileFilter(dirEntry *entry) {
    if (entry->type == DIR_DIRECTORY)
        return 1;

    int len = strlen(entry->name);
    if (!strncmp(&entry->name[len-4], ".ini", 4))
        return 1;
    return 0;
}
void displayThemeList(bool triggersHeld)
{
    clearMessage();
    resetTextScroll();

    setMaxListLength(38);

    for(int i=listOffset;i < listOffset + GAME_LIST_MAX_ITEMS;i++)
    {
        if(i >= dirEntryCount)
        {
            displayDirListItem("", (i - listOffset) + 5, false, dirEntries[i].type, triggersHeld);
            continue;
        }
        displayDirListItem(dirEntries[i].name, (i - listOffset) + 5, i==selectedDirEntry, dirEntries[i].type, triggersHeld);
    }
    updateSelectionSprite((selectedDirEntry- listOffset) + 5, false);
}
void logic_standard(enum prog_state_types * exit_state)
{
    jo_nbg2_printf(1, 3, "Loaded Theme: %s                              ", loadedThemeName);

    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
    {
        if(dirEntryCount > 0)
        {
            playSfx(SFX_SLIDE, false);
            if(dirEntries[selectedDirEntry].type == DIR_DIRECTORY)
            {
                if(!loadThemeFile(dirEntries[selectedDirEntry].name))
                    displayStatus("Could not load %s", dirEntries[selectedDirEntry].name);

                // change back to the theme directory
                s_chdir("/satiator-rings/themes");
                theme_list_state = ROUTINE_STATE_END;
                *exit_state = PROG_STATE_THEME_RELOAD;
            }
        }
    }
    if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_CHANGE, false);
        theme_list_state = ROUTINE_STATE_END;
        *exit_state = PROG_STATE_OPTIONS;
    }
}
void logic_themelist()
{
    static enum prog_state_types exit_state = PROG_STATE_SPLASH;
    static bool triggersHeld = false;
    int maxlistItems = GAME_LIST_MAX_ITEMS;
    switch(theme_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            createGuiBoxes("THEME.TGA", true);
            routine_scene = 0;
            triggersHeld = false;
            s_chdir("/satiator-rings/themes");
            loadFileList(".", themeFileFilter);

            setMaxListLength(GAME_LIST_MAX_ITEM_LEN);
            if(options[OPTIONS_LIST_MODE] == GAME_VIEW_TEXT_ONLY)
                setMaxListLength(38);
            displayThemeList(triggersHeld);
            displayStatus("Select A Theme To Load");
            theme_list_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_SPLASH;
            break;
        case ROUTINE_STATE_RUN:
            displayTime();
            logic_standard(&exit_state);

            // all list controls
            if(dirEntryCount > 0)
            {
                // scroll the selected item text every frame
                displayDirListItem(dirEntries[selectedDirEntry].name, (selectedDirEntry - listOffset) + 5, true, dirEntries[selectedDirEntry].type, triggersHeld);
            }
            
            if(pad_controllers[0].direction_status == BUTTON_STATE_HELD)
            {
                switch(pad_controllers[0].direction_id)
                {    
                    case UP:
                        if((listScrolldelay < LIST_SCROLL_DELAY) && !triggersHeld)
                        {
                            listScrolldelay++;
                            break;
                        }
                        listScrolldelay = 0;
                        moveDirEntrySelectionUp(maxlistItems, SFX_MOVE, false, true);
                        displayThemeList(triggersHeld);
                        break;
                    case DOWN:
                        if((listScrolldelay < LIST_SCROLL_DELAY) && !triggersHeld)
                        {
                            listScrolldelay++;
                            break;
                        }
                        listScrolldelay = 0;
                        moveDirEntrySelectionDown(maxlistItems, SFX_MOVE, false, true);
                        displayThemeList(triggersHeld);
                        break;
                }
            }
            if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
            {
                switch(pad_controllers[0].direction_id)
                {
                    case LEFT:
                        break;
                    case RIGHT:
                        break;
                    case UP:
                        listScrolldelay = 0;
                        moveDirEntrySelectionUp(maxlistItems, SFX_MOVE, false, true);
                        break;
                    case DOWN:
                        sprites[selectionSprite].scale_y -= 0.1f;
                        listScrolldelay = 0;
                        moveDirEntrySelectionDown(maxlistItems, SFX_MOVE, false, true);
                        break;
                }
                displayThemeList(triggersHeld);
            }
            break; 
        case ROUTINE_STATE_END:
            s_chdir(currentDirectory);
            routine_scene = 0;
            theme_list_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}