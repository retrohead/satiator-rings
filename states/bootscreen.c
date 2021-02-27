#define MAX_SPLASH_FRAME 180

#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "routine_states.h"
#include "gamelist.h"
#include "../satiator_functions.h"

enum routine_state_types bootscreen_state = ROUTINE_STATE_INITIALIZE;

void logic_bootscreen()
{
    enum SATIATOR_ERROR_CODE ret;
    switch(bootscreen_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            //jo_nbg2_clear();
            //jo_clear_background(JO_COLOR_White);
            create_sprite(load_sprite_texture("TEX", "S.TGA"), 80, 20, 1, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture("TEX", "S1.TGA"), sprites[0].x, sprites[0].y + getSpriteHeight(0) + 15, 0, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture("TEX", "S2.TGA"), sprites[0].x, sprites[1].y + getSpriteHeight(1) + 5, 0, 1.0, 1.0, 0);
            jo_nbg2_printf(14, 20,   "Loading....");
            bootscreen_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            routine_scene++;
            if(routine_scene >= 100)
            {
                ret = satiatorTryLaunchFile(dirEntries[selectedDirEntry].name);
                if(ret != SATIATIOR_SUCCESS)
                {
                    jo_nbg2_printf(1,  22,"Error: %s", cdparse_error_string);
                }
                bootscreen_state = ROUTINE_STATE_END;
            }
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            bootscreen_state = ROUTINE_STATE_INITIALIZE;
            prog_state = PROG_STATE_GAMELIST;
            break;
    }
}