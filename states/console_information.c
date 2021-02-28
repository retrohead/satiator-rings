#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"
#include "../bios.h"

enum routine_state_types console_info_state = ROUTINE_STATE_INITIALIZE;

void logic_console_info()
{
    static int selectedMenuOption = 0;
    static enum prog_state_types exit_state = PROG_STATE_MENU;
    switch(console_info_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            clearMenuOptions();
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);
            displayMenuOptions(selectedMenuOption);
            jo_nbg2_printf(1, 8, "Console Region: %s", getRegionString());
            console_info_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_MENU;
            break;
        case ROUTINE_STATE_RUN:
            jo_nbg2_printf(13, 4, "CONSOLE INFO");
            controlMenuOptions(&selectedMenuOption, &console_info_state, &exit_state);
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            console_info_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}