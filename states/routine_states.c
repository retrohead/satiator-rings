#include "../main.h"
#include "routine_states.h"
#include <string.h>

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

void clearMessage()
{
    displayStatus(" ");
}