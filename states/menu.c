#include <jo/jo.h>
#include "../main.h"
#include "../fade.h"
#include "../sprite_manager.h"
#include "../pad_controllers.h"
#include "../background.h"
#include "routine_states.h"

enum routine_state_types menu_state = ROUTINE_STATE_INITIALIZE;

void logic_menu()
{
    debugScreenFade();
    static enum prog_state_types exit_state = PROG_STATE_MENU;
    jo_printf(0, 0, "MENU");
    jo_printf(0, 1, "menu_state=%d", menu_state);
    switch(menu_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            jo_clear_background(JO_COLOR_White);
            load_background("TEX", "BG.TGA");
            load_sprite_texture("ISO/GH", "BOX.TGA");
            load_sprite_texture("ISO/GH", "BOXSMALL.TGA");
            load_sprite_texture("ISO/RS", "BOX.TGA");
            load_sprite_texture("ISO/RS", "BOXSMALL.TGA");
            create_sprite(spriteTex[0].texture_id, 60, 100, 400, 1.0, 1.0, 0);
            create_sprite(spriteTex[1].texture_id, 32 + 130, 50 + 100, 400, 1.0, 1.0, 0);
            create_sprite(spriteTex[2].texture_id, 80, 80 + 100, 300 , 1.0, 1.0, 0);
            create_sprite(spriteTex[3].texture_id, 40 + 90, 40 + 90 + 100, 400, 1.0, 1.0, 0);

            fadeInOut(FADE_IN);
            menu_state = ROUTINE_STATE_FADE_IN;
            exit_state = PROG_STATE_MENU;
            break;
        case ROUTINE_STATE_FADE_IN:
            if(!screenIsFading)
                menu_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
            {
                switch(pad_controllers[0].direction_id)
                {
                    case LEFT:
                        break;
                    case RIGHT:
                        break;
                    case UP:
                        break;
                    case DOWN:
                        break;
                }
            }
            if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
            {
                fadeInOut(FADE_OUT);
                menu_state = ROUTINE_STATE_FADE_OUT;
                exit_state = PROG_STATE_GAMELIST;
            }
            if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
            {
                fadeInOut(FADE_OUT);
                menu_state = ROUTINE_STATE_FADE_OUT;
                exit_state = PROG_STATE_SPLASH;
            }
            break;
        case ROUTINE_STATE_FADE_OUT:
            if(!screenIsFading)
                menu_state = ROUTINE_STATE_END;
            break;
        case ROUTINE_STATE_END:
            menu_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}