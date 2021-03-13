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
            createMenuOption("Game List", PROG_STATE_GAMELIST, OPTION_PROGRAM_STATE, 8);
            createMenuOption("Console Information", PROG_STATE_CONSOLE_INFO, OPTION_PROGRAM_STATE, 9);
            createMenuOption("Satiator Information", PROG_STATE_SATIATOR_INFO, OPTION_PROGRAM_STATE, 10);
            createMenuOption("Options", PROG_STATE_OPTIONS, OPTION_PROGRAM_STATE, 11);
            createMenuOption("Launch Satiator Menu", PROG_STATE_EXIT_TO_MENU, OPTION_PROGRAM_STATE, 13);
            createMenuOption("Reboot Console", PROG_STATE_EXIT, OPTION_PROGRAM_STATE, 14);
            displayVersion();
            loadSelectionSprite();
            create_sprite(load_sprite_texture("TEX", "MENU.TGA"), 0, 4, 1, 1, 1, 0);
            displayMenuOptions(selectedMenuOption);
            menu_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_GAMELIST;
            break;
        case ROUTINE_STATE_RUN:
            displayTime();
            controlMenuOptions(&selectedMenuOption, &menu_state, &exit_state);
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            menu_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}