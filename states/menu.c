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
            createGuiBoxes("MENU.TGA", true);
            clearMenuOptions();
            createMenuOption("Game List", PROG_STATE_GAMELIST, OPTION_PROGRAM_STATE, 8, SFX_CHANGE);
            createMenuOption("Console Information", PROG_STATE_CONSOLE_INFO, OPTION_PROGRAM_STATE, 9, SFX_SELECT);
            createMenuOption("Satiator Information", PROG_STATE_SATIATOR_INFO, OPTION_PROGRAM_STATE, 10, SFX_SELECT);
            createMenuOption("Options", PROG_STATE_OPTIONS, OPTION_PROGRAM_STATE, 11, SFX_CHANGE);
            createMenuOption("Launch Satiator Menu", PROG_STATE_EXIT_TO_MENU, OPTION_PROGRAM_STATE, 13, SFX_SELECT);
            createMenuOption("Reboot Console", PROG_STATE_EXIT, OPTION_PROGRAM_STATE, 14, SFX_SELECT);
            displayVersion();
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