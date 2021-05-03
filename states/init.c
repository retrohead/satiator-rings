#include <jo/jo.h>
#include <string.h>
#include "../main.h"
#include "../ini.h"
#include "../debug.h"
#include "routine_states.h"
#include "../libsatiator/satiator.h"
#include "../satiator_functions.h"

enum routine_state_types init_state = ROUTINE_STATE_INITIALIZE;

char name[512];
bool bootLastGameDir(char * dir)
{
    if(strcmp(currentDirectory, "/"))
        s_chdir("/");
    int ret = s_chdir(dir);
    if (ret != FR_OK) {
        displayStatus("Could not open %s", dir);
        return false;
    } else
    {
        strcpy(currentDirectory, dir);
        loadFileList(".", satiatorExecutableFilter);
        if((dirEntryCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
        {
            return true;
        }
    }
    return false;
}
void bootLastGameImage(char * fn)
{
    if(strcmp(currentDirectory, "/"))
        s_chdir("/");
    char * lastSlash = strrchr(fn, '/');
    if(!lastSlash)
    {
        strcpy(currentDirectory, fn);
    } else
    {
        // need to change directory
        const char * fileName = lastSlash + 1;
        strcpy(name, fn);
        name[lastSlash - name] = '\0';
        s_chdir(name);
        strcpy(currentDirectory, name);
        if(dirEntries[selectedDirEntry].name != NULL)
            jo_free(dirEntries[selectedDirEntry].name);
        dirEntries[selectedDirEntry].name = jo_malloc(strlen(fileName));
        strcpy(dirEntries[selectedDirEntry].name, fileName);
    }
}
bool bootLastGame()
{
    static bool failed = false; // we only ever want to run this once on boot
    if(failed)
        return false;
    dirEntryCount = 0;

    if(loadIniListFirstLine("recent.ini", name))
    {
        if (name[strlen(name) - 4] == '.')
        {
            bootLastGameImage(name);
            return true;
        }
        else
        {
            return bootLastGameDir(name);
        }
    }
    failed = true;
    return false;
}
void logic_init()
{
    static enum prog_state_types exit_state = PROG_STATE_SPLASH;
    switch(init_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            initOptions();
            initSatiator();
            load_font_satiator("/satiator-rings/gfx", "FONT.TGA");
            routine_scene = 0;
            init_state = ROUTINE_STATE_RUN;                       
            exit_state = PROG_STATE_SPLASH;
            break;
        case ROUTINE_STATE_RUN:
            switch(satiatorState)
            {
                case SATIATOR_STATE_NOT_FOUND:
                    centerTextVblank(20, "Satiator Not Detected");
                    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                    {
                        init_state = ROUTINE_STATE_END;
                        exit_state = PROG_STATE_EXIT;
                    }
                    break;
                case SATIATOR_STATE_NOT_WORKING:
                    centerTextVblank(20, "Satiator Detected");
                    centerTextVblank(22, "Satiator Is Not Working");
                    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                    {
                        init_state = ROUTINE_STATE_END;
                        exit_state = PROG_STATE_EXIT;
                    }
                    break;
                case SATIATOR_STATE_WORKING:
                    if(pad_controllers[0].btn_z == BUTTON_STATE_HELD)
                    {
                        centerTextVblank(28, "Skipping To Boot Sequence");
                        exit_state = PROG_STATE_QUICKBOOT;
                        init_state = ROUTINE_STATE_END;
                    } else
                    {
                        init_state = ROUTINE_STATE_END;
                        exit_state = PROG_STATE_SPLASH;
                    }
                    break;
            }
            break;
        case ROUTINE_STATE_END_CANCEL:
        case ROUTINE_STATE_END:
            if(satiatorState == SATIATOR_STATE_WORKING)
                loadOptions(true);
            if(options[OPTIONS_SKIP_SPLASH] == 1)
            {
                freeSfx();
                loadSfx(SFX_SELECT);
                loadSfx(SFX_MOVE);
                loadSfx(SFX_SLIDE);
                loadSfx(SFX_CHANGE);
                loadSfx(SFX_BACK);
                loadSfx(SFX_OPTION);
                exit_state = PROG_STATE_GAMELIST;
            }

            init_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}