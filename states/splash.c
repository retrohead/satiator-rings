#define MAX_SPLASH_FRAME 180

#include <jo/jo.h>
#include "../main.h"
#include "routine_states.h"
#include "../satiator/satiator.h"

enum routine_state_types splash_state = ROUTINE_STATE_INITIALIZE;

void logic_splash()
{
    switch(splash_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            //jo_nbg2_clear();
            //jo_clear_background(JO_COLOR_White);
            create_sprite(load_sprite_texture("TEX", "S.TGA"), 80, 20, 1, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture("TEX", "S1.TGA"), sprites[0].x, sprites[0].y + getSpriteHeight(0) + 15, 0, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture("TEX", "S2.TGA"), sprites[0].x, sprites[1].y + getSpriteHeight(1) + 5, 0, 1.0, 1.0, 0);
            splash_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            if(s_mode(s_api) == 0)
            {
                jo_nbg2_printf(11, 20,   "Satiator Detected");
                int result = 0;
                result = s_opendir(".");
                if(result != 0)
                {
                    jo_nbg2_printf(8,  22,"Satiator Is Not Working");
                    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                    {
                        s_mode(s_cdrom);
                        jo_core_exit_to_multiplayer();
                    }
                }
                else 
                {
                    jo_nbg2_printf(10, 22,  "Satiator Is Working");
                    splash_state = ROUTINE_STATE_END;
                    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                    {
                        splash_state = ROUTINE_STATE_END;
                    }
                }
            } else
            {
                jo_nbg2_printf(9, 20, "Satiator Not Detected");
                if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                {
                    s_mode(s_cdrom);
                    jo_core_exit_to_multiplayer();
                }
            }
            if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
            {
                splash_state = ROUTINE_STATE_END;
            }
            break;
        case ROUTINE_STATE_END:
            splash_state = ROUTINE_STATE_INITIALIZE;
            prog_state = PROG_STATE_GAMELIST;
            break;
    }
}