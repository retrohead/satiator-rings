#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "../debug.h"
#include "routine_states.h"
#include "../satiator_functions.h"
#include "../ini.h"
#include "../bios.h"

#define MAX_RECENT_ITEMS 100
int printRow = 0;

enum routine_state_types bootscreen_state = ROUTINE_STATE_INITIALIZE;

void addItemToRecentHistory()
{
    char * fullpath = jo_malloc(1024);
    // add item to recent history
    strcpy(fullpath, currentDirectory);
    if(!((dirEntryCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME)))
    {
        // write the full path, else auto load was used so use the directory name
        strcat(fullpath, "/");
        strcat(fullpath, dirEntries[selectedDirEntry].name);
    }
    addItemToIni("recent.ini", fullpath, true, false, false);
    jo_free(fullpath);
}
void logic_bootscreen()
{
    enum SATIATOR_ERROR_CODE ret;
    switch(bootscreen_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            if(prog_state != PROG_STATE_QUICKBOOT)
            {

                if(strcmp(dirEntries[selectedDirEntry].name, "origmenu"))
                {
                    create_sprite(load_gui_texture(PAL_COL_BOX_BG), 160, 120, 3, 50.0, 50.0, 0);
                    draw_sprites();
                    slSynch();
                    char bootname[256];
                    strcpy(bootname, dirEntries[selectedDirEntry].name);
                    const char * dot = strrchr(bootname, '.');
                    if(dot)
                        bootname[strlen(bootname) - strlen(dot)] = '\0'; // strip the file extension
                    bootname[38] = '\0'; // make sure the name doesnt go off screen
                    centerText(18, "%s", bootname);
                    // load the game boxart from the current directory
                    int tex = load_sprite_texture_satiator(JO_NULL, "BOX.TGA");
                    if(tex < 0)
                        tex = load_sprite_texture_satiator("/satiator-rings/gfx", "SBOX.TGA"); // no boxart found so using default
                    int boxSprite = create_sprite(tex, 160 - (getTextureWidth(tex) / 2), 120 - getTextureHeight(tex), 1, 1, 1, 0);
                    strcpy(bootname, "/satiator-rings/themes/");
                    strcat(bootname, loadedThemeName);
                    int shadowSprite = create_sprite(load_sprite_texture_satiator(bootname, "SHDW.TGA"), 0, sprites[boxSprite].y + getTextureHeight(tex) + 5 , 1, 1, 1, 0);
                    
                    sprites[shadowSprite].x = sprites[boxSprite].x + 2;
                    sprites[shadowSprite].y = sprites[boxSprite].y + getTextureHeight(tex) + 5;
                    sprites[shadowSprite].scale_x = 1;
                    sprites[shadowSprite].scale_y = 0.9;
                    if(getTextureWidth(tex) != 80)
                    {
                        sprites[shadowSprite].scale_x = 0.8;
                        sprites[shadowSprite].x -= 7;
                    }
                } else
                {
                    create_sprite(load_gui_texture(PAL_COL_WHITE), 160, 120, 3, 50.0, 50.0, 0);
                    draw_sprites();
                    slSynch();
                    main_palette.data[2] = JO_COLOR_Red;
                    centerText(20, "Loading");
                    logosprites[0] = create_sprite(load_sprite_texture("TEX", "S.TGA"), 80, 20, 2, 1.0, 1.0, 0);
                    logosprites[1] = create_sprite(load_sprite_texture("TEX", "S1.TGA"), sprites[logosprites[0]].x, sprites[logosprites[0]].y + getSpriteHeight(logosprites[0]) + 15, 1, 1.0, 1.0, 0);
                    logosprites[2] = create_sprite(load_sprite_texture("TEX", "S2.TGA"), sprites[logosprites[0]].x, sprites[logosprites[1]].y + getSpriteHeight(logosprites[1]) + 5, 1, 1.0, 1.0, 0);
                }
            }
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
                        routine_scene = 2;
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
                    if(options[OPTIONS_AUTO_PATCH] == 0)
                    {
                        printCenterText(22,"Region Patch Required");
                        printCenterText(24,"Confirm (C)");
                        printCenterText(25,"Cancel (B)");
                    }

                    if((pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (options[OPTIONS_AUTO_PATCH] == 1))
                    {
                        printCenterText(22,"");
                        printCenterText(24,"");
                        printCenterText(25,"");
                        centerTextVblank(20, "Patching Image");
                        if(!satiatorPatchDescFileImage(getRegionString()))
                        {
                            routine_scene = 2;
                            break;
                        }
                        centerTextVblank(20, "Adding To Recent History");
                        addItemToRecentHistory();
                        centerTextVblank(20, "Booting Disc");
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