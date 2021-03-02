#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "routine_states.h"
#include "../satiator_functions.h"
#include "../ini.h"

enum routine_state_types bootscreen_state = ROUTINE_STATE_INITIALIZE;

void addItemToRecentHistory()
{
    char * fullpath = jo_malloc(1024);
    // add item to recent history
    strcpy(fullpath, currentDirectory);
    if((dirEntyCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
    {
        // auto loaded game, just write the directory
    } else
    {
        // write the full path
        strcat(fullpath, "/");
        strcat(fullpath, dirEntries[selectedDirEntry].name);
    }
    // TODO delete the game from the recent.ini
    // TODO add the game to the top of the recent.ini

    jo_free(fullpath);
}
void logic_bootscreen()
{
    enum SATIATOR_ERROR_CODE ret;
    switch(bootscreen_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            create_sprite(load_sprite_texture("TEX", "S.TGA"), 80, 20, 1, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture("TEX", "S1.TGA"), sprites[0].x, sprites[0].y + getSpriteHeight(0) + 15, 0, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture("TEX", "S2.TGA"), sprites[0].x, sprites[1].y + getSpriteHeight(1) + 5, 0, 1.0, 1.0, 0);
            jo_nbg2_printf(14, 20,   "Loading");
            bootscreen_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            switch(routine_scene)
            {
                case 0:
                    // wait for 1 frame
                    routine_scene++;
                    break;
                case 1:
                    // try launching the file
                    ret = satiatorTryLaunchFile(dirEntries[selectedDirEntry].name);
                    if(ret == SATIATOR_PATCH_REQUIRED)
                    {
                        routine_scene = 3;
                    } else if(ret != SATIATOR_SUCCESS)
                    {
                        jo_nbg2_printf(2,  22, "Error: %s", cdparse_error_string);
                    } else
                    {
                        bootscreen_state = ROUTINE_STATE_END;
                    }
                    break;
                case 2:
                    // error occurred
                    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
                    {
                        bootscreen_state = ROUTINE_STATE_END;
                    }
                    break;
                case 3:
                    // confirm patching
                    jo_nbg2_printf(9,  22,"Region Patch Required");
                    jo_nbg2_printf(9,  24,"     C = Confirm    ");
                    jo_nbg2_printf(9,  25,"     B = Cancel     ");

                    if(pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS)
                    {
                        if(!satiatorPatchDescFileImage())
                        {
                            routine_scene = 2;
                            break;
                        }
                        ret = satiatorEmulateDesc("emu.desc");
                        if(ret == SATIATOR_PATCH_REQUIRED)
                        {
                            routine_scene = 3;
                        }
                        if(ret != SATIATOR_SUCCESS)
                        {
                            jo_nbg2_printf(2,  22,"Error: %s", cdparse_error_string);
                        } else
                        {
                            addItemToRecentHistory();
                            bootscreen_state = ROUTINE_STATE_END;
                        }
                    }
                    if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
                    {
                        bootscreen_state = ROUTINE_STATE_END;
                    }

                    break;
            }
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            bootscreen_state = ROUTINE_STATE_INITIALIZE;
            prog_state = PROG_STATE_GAMELIST;
            break;
    }
}