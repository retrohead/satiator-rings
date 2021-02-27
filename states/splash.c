#define MAX_SPLASH_FRAME 100

#include <jo/jo.h>
#include "../main.h"
#include "routine_states.h"
#include "../satiator/satiator.h"

enum routine_state_types splash_state = ROUTINE_STATE_INITIALIZE;

int logosprites[3];

void animateLogo(int *frame)
{
    sprites[logosprites[0]].rot_angle += sprites[logosprites[0]].rot_speed;
    if(sprites[logosprites[0]].rot_angle >= 360)
        sprites[logosprites[0]].rot_angle -= 360;
    switch(*frame)
    {
        case 0:
            // zooming in
            sprites[logosprites[0]].speed_x += 0.02;
            sprites[logosprites[0]].speed_y += 0.02;
            sprites[logosprites[0]].scale_x += sprites[logosprites[0]].speed_y;
            sprites[logosprites[0]].scale_y += sprites[logosprites[0]].speed_y;

            if(sprites[logosprites[0]].scale_x >= 1)
            {
                sprites[logosprites[0]].scale_x = 1.0;
                sprites[logosprites[0]].scale_y = 1.0;

                sprites[logosprites[0]].speed_x = 2;
                sprites[logosprites[0]].speed_y = 2;
                *frame = *frame + 1;
            }
            break;
        case 1:
            // moving to position
            sprites[logosprites[0]].x -= sprites[logosprites[0]].speed_x;
            sprites[logosprites[0]].y -= sprites[logosprites[0]].speed_y;
            if(sprites[logosprites[0]].x <= 80)
                sprites[logosprites[0]].x = 80;
            if(sprites[logosprites[0]].y <= 20)
                sprites[logosprites[0]].y = 20;

            sprites[logosprites[0]].speed_x -= sprites[logosprites[0]].velocity_x / 10;
            sprites[logosprites[0]].speed_y -= sprites[logosprites[0]].velocity_y / 10;
            if(sprites[logosprites[0]].speed_x <= 1)
                sprites[logosprites[0]].speed_x = 1;
            if(sprites[logosprites[0]].speed_y <= 1)
                sprites[logosprites[0]].speed_y = 1;

            if((sprites[logosprites[0]].x <= 80) && (sprites[logosprites[0]].y <= 20))
            {
                sprites[logosprites[0]].x = 80;
                sprites[logosprites[0]].y = 20;
            }
            // slowing down
            sprites[logosprites[0]].velocity_x += 0.045;
            sprites[logosprites[0]].rot_speed -= sprites[logosprites[0]].velocity_x;
            if(sprites[logosprites[0]].rot_speed < 5)
            {
                sprites[logosprites[0]].rot_speed = 5;
                if((sprites[logosprites[0]].rot_angle >= 350) || (sprites[logosprites[0]].rot_angle <= 10))
                {
                    sprites[logosprites[0]].rot_angle = 0;
                    sprites[logosprites[0]].rot_speed = 0;

                    sprites[logosprites[1]].x -= 320;
                    sprites[logosprites[1]].scale_x = 20;
                    sprites[logosprites[1]].scale_y = 20;
                    sprites[logosprites[1]].speed_x = 0.4;
                    *frame = *frame + 1;
                }
            }
            break;
        case 2:
            // first line of text
            sprites[logosprites[1]].scale_x -= sprites[logosprites[1]].speed_x;
            sprites[logosprites[1]].scale_y -= sprites[logosprites[1]].speed_x;
            if(sprites[logosprites[1]].scale_x <= 1)
            {
                sprites[logosprites[1]].scale_x = 1;
                sprites[logosprites[1]].scale_y = 1;

                sprites[logosprites[2]].x -= 320;
                sprites[logosprites[2]].scale_x = 20;
                sprites[logosprites[2]].scale_y = 20;
                sprites[logosprites[2]].speed_x = 0.4;

                *frame = *frame + 1;
            }
            break;
        case 3:
            // second line of text
            sprites[logosprites[2]].scale_x -= sprites[logosprites[2]].speed_x;
            sprites[logosprites[2]].scale_y -= sprites[logosprites[2]].speed_x;
            if(sprites[logosprites[2]].scale_x <= 1)
            {
                sprites[logosprites[2]].scale_x = 1;
                sprites[logosprites[2]].scale_y = 1;
                *frame = *frame + 1;
            }
            break;
            
        default:
            sprites[logosprites[0]].y = 20;
            sprites[logosprites[0]].rot_speed = 0;

            for(int i=0;i<2;i++)
            {
                sprites[logosprites[i]].x = 80;
                sprites[logosprites[i]].scale_x = 1;
                sprites[logosprites[i]].scale_y = 1;
                sprites[logosprites[i]].rot_angle = 0;
                sprites[logosprites[1]].speed_x = 0.02;
            }
            if(*frame < MAX_SPLASH_FRAME)
                *frame = *frame + 1;
            break;
    }
}
void logic_splash()
{
    switch(splash_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            //jo_nbg2_clear();
            //jo_clear_background(JO_COLOR_White);
            logosprites[0] = create_sprite(load_sprite_texture("TEX", "S.TGA"), 80, 20, 1, 1.0, 1.0, 0);
            logosprites[1] = create_sprite(load_sprite_texture("TEX", "S1.TGA"), sprites[logosprites[0]].x, sprites[logosprites[0]].y + getSpriteHeight(logosprites[0]) + 15, 0, 1.0, 1.0, 0);
            logosprites[2] = create_sprite(load_sprite_texture("TEX", "S2.TGA"), sprites[logosprites[0]].x, sprites[logosprites[1]].y + getSpriteHeight(logosprites[1]) + 5, 0, 1.0, 1.0, 0);

            
            // move the sprites off screen until ready
            sprites[logosprites[1]].x += 320;
            sprites[logosprites[2]].x += 320;

            // set s center screen
            sprites[logosprites[0]].x = 160 - (getSpriteWidth(logosprites[0]) / 2);
            sprites[logosprites[0]].y = 120 - (getSpriteHeight(logosprites[0]) / 2);
            sprites[logosprites[0]].scale_x = 0.0;
            sprites[logosprites[0]].scale_y = 0.0;

            
            sprites[logosprites[0]].rot_speed = 30;
            sprites[logosprites[0]].speed_y = 0.02;

            splash_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            animateLogo(&routine_scene);
            if(routine_scene > 4)
            {
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

                        if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                        {
                            if(routine_scene < MAX_SPLASH_FRAME - 1)
                                routine_scene = MAX_SPLASH_FRAME - 1;
                        }
                    }
                    if(routine_scene >= MAX_SPLASH_FRAME)
                    {
                        splash_state = ROUTINE_STATE_END;
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
            }
            if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
            {
                if(routine_scene < 3)
                    routine_scene = 3;
            }
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            splash_state = ROUTINE_STATE_INITIALIZE;
            prog_state = PROG_STATE_GAMELIST;
            break;
    }
}