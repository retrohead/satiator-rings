#define MAX_SPLASH_FRAME 180

#include <jo/jo.h>
#include "../main.h"
#include "../fade.h"
#include "../sprite_manager.h"
#include "../pad_controllers.h"
#include "../background.h"
#include "routine_states.h"

enum routine_state_types splash_state = ROUTINE_STATE_INITIALIZE;

void animateLogo(int *frame)
{
    sprites[0].rot_angle += sprites[0].rot_speed;
    if(sprites[0].rot_angle >= 360)
        sprites[0].rot_angle -= 360;
    switch(*frame)
    {
        case 2:
            // zooming in
            sprites[0].scale_x += sprites[0].speed_y;
            sprites[0].scale_y += sprites[0].speed_y;

            if(sprites[0].scale_x >= 1)
            {
                sprites[0].scale_x = 1.0;
                sprites[0].scale_y = 1.0;

                sprites[0].speed_x = 1;
                sprites[0].speed_y = 1;
                *frame = *frame + 1;
            }
            break;
        case 3:
            // moving to position
            sprites[0].x -= sprites[0].speed_x;
            sprites[0].y -= sprites[0].speed_y;
            if(sprites[0].x <= 80)
                sprites[0].x = 80;
            if(sprites[0].y <= 40)
                sprites[0].y = 40;

            if((sprites[0].x <= 80) && (sprites[0].y <= 40))
            {
                sprites[0].velocity_x = 0;
                *frame = *frame + 1;
            }
            break;
        case 4:
            // slowing down
            sprites[0].velocity_x += 0.02;
            sprites[0].rot_speed -= sprites[0].velocity_x;
            if(sprites[0].rot_speed < 1)
            {
                sprites[0].rot_speed = 1;
                if((sprites[0].rot_angle >= 0) && (sprites[0].rot_angle <= 1))
                {
                    sprites[0].rot_angle = 0;
                    sprites[0].rot_speed = 0;

                    sprites[1].x -= 320;
                    sprites[1].scale_x = 20;
                    sprites[1].scale_y = 20;
                    sprites[1].speed_x = 0.4;
                    *frame = *frame + 1;
                }
            }
            break;
        case 5:
            // first line of text
            sprites[1].scale_x -= sprites[1].speed_x;
            sprites[1].scale_y -= sprites[1].speed_x;
            if(sprites[1].scale_x <= 1)
            {
                sprites[1].scale_x = 1;
                sprites[1].scale_y = 1;

                sprites[2].x -= 320;
                sprites[2].scale_x = 20;
                sprites[2].scale_y = 20;
                sprites[2].speed_x = 0.4;

                *frame = *frame + 1;
            }
            break;
        case 6:
            // second line of text
            sprites[2].scale_x -= sprites[2].speed_x;
            sprites[2].scale_y -= sprites[2].speed_x;
            if(sprites[2].scale_x <= 1)
            {
                sprites[2].scale_x = 1;
                sprites[2].scale_y = 1;
                *frame = *frame + 1;
            }
            break;
            
        default:
            sprites[0].y = 40;
            sprites[0].rot_speed = 0;
            sprites[3].y = -100;

            for(int i=0;i<3;i++)
            {
                sprites[i].x = 80;
                sprites[i].scale_x = 1;
                sprites[i].scale_y = 1;
                sprites[i].rot_angle = 0;
                sprites[1].speed_x = 0.02;
            }
            if(*frame < MAX_SPLASH_FRAME)
                *frame = *frame + 1;
            break;
    }
}

void animateBear(int *frame)
{
    static bool rot_clockwise = true;
    
    float angle;
    switch(*frame)
    {
        case 0:
            // walking
            if(rot_clockwise)
                sprites[3].rot_angle += sprites[3].rot_speed = 2;
            else
                sprites[3].rot_angle -= sprites[3].rot_speed = 2;

            if(sprites[3].rot_angle >= 10)
                rot_clockwise = false;
            if(sprites[3].rot_angle <= -10)
                rot_clockwise = true;
            sprites[3].x += sprites[3].speed_x;
            if(sprites[3].x >= 160 - 76)
            {
                sprites[3].x = 160 - 76;
                sprites[3].speed_x = 0;
                sprites[3].velocity_y = 0.2;
                sprites[3].speed_y = 0;
                *frame = *frame + 1;
            }
            angle = sprites[3].rot_angle;
            if(angle < 0)
                angle = - angle;
            sprites[0].x = sprites[3].x + 45;
            sprites[0].y = sprites[3].y + 38 + (angle / 2);
            break; 
        case 1:
            // jumping
            sprites[3].speed_y += sprites[3].velocity_y;
            sprites[3].y -= sprites[3].speed_y;
            if(sprites[3].y < -96)
                *frame = *frame + 1;
            break; 
    }
}

void animate(int *frame)
{
    animateBear(frame);
    if(*frame > 1)
        animateLogo(frame);
}

void logic_splash()
{
    static int frame = 0;
    switch(splash_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            jo_clear_background(JO_COLOR_White);
            load_background(JO_NULL, "BG.TGA");
            
            create_sprite(load_sprite_texture(JO_NULL, "S.TGA"), 80, 40, 1, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture(JO_NULL, "S1.TGA"), sprites[0].x, sprites[0].y + getSpriteHeight(0) + 15, 0, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture(JO_NULL, "S2.TGA"), sprites[0].x, sprites[1].y + getSpriteHeight(1) + 5, 0, 1.0, 1.0, 0);
            create_sprite(load_sprite_texture(JO_NULL, "SBEAR.TGA"), -96, 120 - 67, 2, 1.0, 1.0, 0);
            
            // reset the animation
            frame = 0;
            sprites[3].rot_speed = 0.02;
            sprites[3].speed_x = 1.5;

            // move the sprites off screen until ready
            sprites[0].x += 320;
            sprites[1].x += 320;
            sprites[2].x += 320;

            // set s center screen
            sprites[0].scale_x = 0.4;
            sprites[0].scale_y = 0.4;

            
            sprites[0].rot_speed = 7;
            sprites[0].speed_y = 0.02;

            fadeInOut(FADE_IN);
            splash_state = ROUTINE_STATE_FADE_IN;
            break;
        case ROUTINE_STATE_FADE_IN:
            if(!screenIsFading)
                splash_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            animate(&frame);
            if((pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
            {
                if(frame < MAX_SPLASH_FRAME - 1)
                    frame = MAX_SPLASH_FRAME - 1;
            }
            if(frame >= MAX_SPLASH_FRAME)
            {
                fadeInOut(FADE_OUT);
                splash_state = ROUTINE_STATE_FADE_OUT;
            }
            break;
        case ROUTINE_STATE_FADE_OUT:
            if(!screenIsFading)
                splash_state = ROUTINE_STATE_END;
            break;
        case ROUTINE_STATE_END:
            splash_state = ROUTINE_STATE_INITIALIZE;
            prog_state = PROG_STATE_MENU;
            break;
    }
}