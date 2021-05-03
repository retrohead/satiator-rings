#define MAX_SPLASH_FRAME 20

#include <jo/jo.h>
#include <string.h>
#include "../main.h"
#include "../ini.h"
#include "../debug.h"
#include "routine_states.h"
#include "../libsatiator/satiator.h"
#include "../satiator_functions.h"

enum routine_state_types splash_state = ROUTINE_STATE_INITIALIZE;

jo_sound     introsound;

void finishLogo()
{
    sprites[logosprites[0]].y = 20;
    sprites[logosprites[0]].rot_speed = 0;

    for(int i=0;i<3;i++)
    {
        sprites[logosprites[i]].x = 80;
        sprites[logosprites[i]].scale_x = 1;
        sprites[logosprites[i]].scale_y = 1;
        sprites[logosprites[i]].rot_angle = 0;
        sprites[logosprites[i]].speed_x = 0;
    }
}
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
                playSfx(SFX_THUD, false);
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
                playSfx(SFX_THUD, false);
                sprites[logosprites[2]].scale_x = 1;
                sprites[logosprites[2]].scale_y = 1;
                *frame = *frame + 1;
            }
            break;
            
        default:
            finishLogo();
            if(*frame < MAX_SPLASH_FRAME)
                *frame = *frame + 1;
            break;
    }
}
extern bool bootLastGameDir(char * dir);
extern void bootLastGameImage(char * fn);
extern bool bootLastGame();
void logic_splash()
{
    static enum prog_state_types exit_state = PROG_STATE_GAMELIST;
    switch(splash_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            create_sprite(load_gui_texture(PAL_COL_WHITE), 160, 120, 3, 50.0, 50.0, 0);
            routine_scene = 0;
            logosprites[0] = create_sprite(load_sprite_texture_satiator("/satiator-rings/gfx", "S.TGA"), 80, 20, 2, 1.0, 1.0, 0);
            logosprites[1] = create_sprite(load_sprite_texture_satiator("/satiator-rings/gfx", "S1.TGA"), sprites[logosprites[0]].x, sprites[logosprites[0]].y + getSpriteHeight(logosprites[0]) + 15, 1, 1.0, 1.0, 0);
            logosprites[2] = create_sprite(load_sprite_texture_satiator("/satiator-rings/gfx", "S2.TGA"), sprites[logosprites[0]].x, sprites[logosprites[1]].y + getSpriteHeight(logosprites[1]) + 5, 1, 1.0, 1.0, 0);

            loadSfx(SFX_INTRO);
            loadSfx(SFX_THUD);
            loadSfx(SFX_SANSHIRO);
            
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
            exit_state = PROG_STATE_GAMELIST;
            playSfx(SFX_INTRO, false);
            break;
        case ROUTINE_STATE_RUN:
            animateLogo(&routine_scene);
            if(routine_scene == 5)
            {
                playSfx(SFX_SANSHIRO, false);
            }
            if(routine_scene > 5)
            {
                switch(satiatorState)
                {
                    case SATIATOR_STATE_NOT_FOUND:
                        centerTextVblank(20, "Satiator Not Detected");
                        if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                        {
                            if(routine_scene < MAX_SPLASH_FRAME - 1)
                                routine_scene = MAX_SPLASH_FRAME - 1;
                        }
                        if(routine_scene >= MAX_SPLASH_FRAME)
                        {
                            splash_state = ROUTINE_STATE_END;
                            exit_state = PROG_STATE_GAMELIST;
                        }
                        break;
                    case SATIATOR_STATE_NOT_WORKING:
                        centerTextVblank(20, "Satiator Detected");
                        centerTextVblank(22, "Satiator Is Not Working");
                        if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                        {
                            if(routine_scene < MAX_SPLASH_FRAME - 1)
                                routine_scene = MAX_SPLASH_FRAME - 1;
                        }
                        if(routine_scene >= MAX_SPLASH_FRAME)
                        {
                            splash_state = ROUTINE_STATE_END;
                            exit_state = PROG_STATE_EXIT;
                        }
                        break;
                    case SATIATOR_STATE_WORKING:
                        centerTextVblank(20, "Satiator Detected");
                        centerTextVblank(22, "Satiator Is Working");

                        if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
                        {
                            if(routine_scene < MAX_SPLASH_FRAME - 1)
                                routine_scene = MAX_SPLASH_FRAME - 1;
                        }
                        if(routine_scene >= MAX_SPLASH_FRAME)
                        {
                            splash_state = ROUTINE_STATE_END;
                            exit_state = PROG_STATE_GAMELIST;
                        }
                        if(pad_controllers[0].btn_z == BUTTON_STATE_HELD)
                        {
                            centerTextVblank(28, "Skipping To Boot Sequence");
                            exit_state = PROG_STATE_QUICKBOOT;
                            splash_state = ROUTINE_STATE_END;
                        }
                        break;
                }
            }
            if((pad_controllers[0].btn_z == BUTTON_STATE_HELD) || (pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS))
            {
                if(routine_scene < 4)
                    routine_scene = 4;
            }
            break;
        case ROUTINE_STATE_END_CANCEL:
        case ROUTINE_STATE_END:
            routine_scene = 0;
            finishLogo();
            draw_sprites();
            slSynch();

            // prepare the shared resources for the main app
            if(satiatorState == SATIATOR_STATE_WORKING)
                loadOptions(false);
            freeSfx();
            loadSfx(SFX_SELECT);
            loadSfx(SFX_MOVE);
            loadSfx(SFX_SLIDE);
            loadSfx(SFX_CHANGE);
            loadSfx(SFX_BACK);
            loadSfx(SFX_OPTION);

            splash_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}