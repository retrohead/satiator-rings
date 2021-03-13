#include "../main.h"
#include "routine_states.h"
#include <string.h>

int selectionSprite;
int logosprites[3];

void displayTime()
{
    if(dt.second % 2 == 0)
        jo_nbg2_printf(34, 28, "%02d %02d", dt.hour, dt.minute);
    else
        jo_nbg2_printf(34, 28, "%02d:%02d", dt.hour, dt.minute);
}
void displayVersion()
{
    centerText(3, "Satiator Rings Version %s", VERSION);
}


void displayStatusText(char * text, bool vblank)
{   
    if(strlen(text) > 33)
        text[33] = '\0';
    char * status = jo_malloc(35);
    strcpy(status, text);
    while(strlen(status) < 33)
        strcat(status, " ");
    jo_nbg2_printf(1, 28, "%s", status);
    jo_free(status);

    if(vblank)
    {
        draw_sprites();
        slSynch();
    }
}
void loadSelectionSprite()
{
    // load the sprite off screen
    selectionSprite = create_sprite(load_sprite_texture("TEX", "SELECT.TGA"), 320, 250, 0, 2, 1, 0);
}
void updateSelectionSprite(int row, bool shortStyle)
{
    sprites[selectionSprite].y = (row * 8);
    sprites[selectionSprite].rot_angle = 0;
    if(shortStyle)
    {
        sprites[selectionSprite].scale_x = 0.67;
        sprites[selectionSprite].scale_y = 1.1;
        sprites[selectionSprite].x = -46;
    } else
    {
        sprites[selectionSprite].scale_x = 0.95;
        sprites[selectionSprite].scale_y = 1.05;
        sprites[selectionSprite].x = -1;
    }
}