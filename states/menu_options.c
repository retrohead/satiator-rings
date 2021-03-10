#include <string.h>
#include "menu_options.h"
#define MAX_MENU_OPTIONS 10

menuOption menuOptions[MAX_MENU_OPTIONS];
int usedMenuOptions = 0;

void clearMenuOptions()
{
    usedMenuOptions = 0;
    for(int i = 0; i<MAX_MENU_OPTIONS;i++)
    {
        menuOptions[i].prog_state = PROG_STATE_INITIALIZE;
    }
}

void createMenuOption(const char * txt, enum prog_state_types state, enum menu_option_types type)
{
    if(usedMenuOptions >= MAX_MENU_OPTIONS)
        return;
    for(int i = 0; i<MAX_MENU_OPTIONS;i++)
    {
        if(menuOptions[i].prog_state == PROG_STATE_INITIALIZE)
        {
            menuOptions[i].prog_state = state;
            menuOptions[i].txt = txt;
            menuOptions[i].type = type;
            usedMenuOptions++;
            return;
        }
    }
}

void displayMenuOptions(int selectedOption)
{
    for(int i = 0; i<MAX_MENU_OPTIONS;i++)
    {
        if(menuOptions[i].prog_state == PROG_STATE_INITIALIZE)
            return;
        if(i == selectedOption)
            jo_nbg2_printf(1, i+8, "> ");
        else
            jo_nbg2_printf(1, i+8, "  ");
        jo_nbg2_printf(3, i+8, menuOptions[i].txt);
    }
}

int controlMenuOptions(int *selectedOption, enum routine_state_types *menu_state, enum prog_state_types * exit_state)
{
    if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_SELECT, false);
        *menu_state = ROUTINE_STATE_END;
        return 0;
    }
    if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_MOVE, false);
        switch(pad_controllers[0].direction_id)
        {
            case LEFT:
            case UP_LEFT:
            case DOWN_LEFT:
                if(menuOptions[*selectedOption].type != OPTION_PROGRAM_STATE)
                    return -1;
                break;
            case RIGHT:
            case DOWN_RIGHT:
            case UP_RIGHT:
                if(menuOptions[*selectedOption].type != OPTION_PROGRAM_STATE)
                    return 1;
                break;
            case UP:
                *selectedOption = *selectedOption - 1;
                if(*selectedOption < 0)
                    *selectedOption += usedMenuOptions;
                break;
            case DOWN:
                *selectedOption = *selectedOption + 1;
                if(*selectedOption >= usedMenuOptions)
                    *selectedOption -= usedMenuOptions;
                break;
        }
        displayMenuOptions(*selectedOption);
    }
    if(menuOptions[*selectedOption].type == OPTION_PROGRAM_STATE)
    {
        if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
        {
            playSfx(SFX_SELECT, false);
            *menu_state = ROUTINE_STATE_END;
            if(usedMenuOptions > 0)
                *exit_state = menuOptions[*selectedOption].prog_state;
        }
    }
    return 0;
}