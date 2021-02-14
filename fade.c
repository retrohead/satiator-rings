#include <jo/jo.h>
#include <stdlib.h>
#include "fade.h"
#include "pad_controllers.h"

enum fadeTypes screenFadeType;
bool screenIsFading;

float screen_transparency = 0;
int screen_active = 1;

void fadeInOut(enum fadeTypes fadeType)
{
    screenIsFading = true;
    screenFadeType = fadeType;
}

void fadeScreen()
{
    if(!screenIsFading)
        return;
    switch (screenFadeType)
    {
        case FADE_IN:
            screen_transparency += SCREEN_FADE_SPEED;
            if(screen_transparency >= 31)
            {
                screen_transparency = 31;
                screenIsFading = false;
                jo_disable_all_screens_transparency();
            }
            break;
        case FADE_OUT:
            screen_transparency -= SCREEN_FADE_SPEED;
            if(screen_transparency <= 0)
            {
                screen_transparency = 0;
                screenIsFading = false;
            }
            break;
    }
}

void draw_faded_screen()
{
    if(screenIsFading)
        jo_enable_screen_transparency(screen_active, (int)screen_transparency);
}

void debugScreenFade()
{
    jo_printf(0, 5, "screen_transparency=%d              ", (int)screen_transparency);
    jo_printf(0, 6, "screen_active=%d                    ", screen_active);

    if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
    {
        switch(pad_controllers[0].direction_id)
        {
            case LEFT:
                screen_transparency--;
                break;
            case RIGHT:
                screen_transparency++;
                break;
            case UP:
                screen_active++;
                break;
            case DOWN:
                screen_active--;
                break;
        }
    }
}
