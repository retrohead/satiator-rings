#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"

enum routine_state_types satiator_info_state = ROUTINE_STATE_INITIALIZE;

void logic_satiator_info()
{
    static int selectedMenuOption = 0;
    static enum prog_state_types exit_state = PROG_STATE_MENU;
    switch(satiator_info_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            clearMenuOptions();
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);
            displayMenuOptions(selectedMenuOption);
            satiator_info_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_MENU;
            break;
        case ROUTINE_STATE_RUN:
            jo_nbg2_printf(12, 4, "SATIATOR INFO");
            controlMenuOptions(&selectedMenuOption, &satiator_info_state, &exit_state);
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            satiator_info_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}