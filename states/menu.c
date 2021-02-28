#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"

enum routine_state_types menu_state = ROUTINE_STATE_INITIALIZE;

void logic_menu()
{
    static int selectedMenuOption = 0;
    static enum prog_state_types exit_state = PROG_STATE_GAMELIST;
    switch(menu_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            clearMenuOptions();
            createMenuOption("Game List", PROG_STATE_GAMELIST);
            createMenuOption("Console Information", PROG_STATE_CONSOLE_INFO);
            createMenuOption("Satiator Information", PROG_STATE_SATIATOR_INFO);
            createMenuOption("Exit To Multiplayer", PROG_STATE_EXIT);
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);
            displayMenuOptions(selectedMenuOption);
            menu_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_GAMELIST;
            break;
        case ROUTINE_STATE_RUN:
            jo_nbg2_printf(17, 4, "MENU");
            controlMenuOptions(&selectedMenuOption, &menu_state, &exit_state);
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            menu_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}