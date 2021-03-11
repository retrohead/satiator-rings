#include <string.h>
#include "menu_options.h"
#define MAX_MENU_OPTIONS 10


int selectorSprite;

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

void createMenuOption(const char * txt, enum prog_state_types state, enum menu_option_types type, int row)
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
            menuOptions[i].row = row;
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
        centerText(menuOptions[i].row, menuOptions[i].txt);
        if(i == selectedOption)
            updateSelectionSprite(menuOptions[i].row, false);
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
        switch(pad_controllers[0].direction_id)
        {
            case LEFT:
            case UP_LEFT:
            case DOWN_LEFT:
                if(menuOptions[*selectedOption].type != OPTION_PROGRAM_STATE)
                {
                    playSfx(SFX_MOVE, false);
                    return -1;
                }
                break;
            case RIGHT:
            case DOWN_RIGHT:
            case UP_RIGHT:
                if(menuOptions[*selectedOption].type != OPTION_PROGRAM_STATE)
                {
                    playSfx(SFX_MOVE, false);
                    return 1;
                }
                break;
            case UP:
                playSfx(SFX_MOVE, false);
                *selectedOption = *selectedOption - 1;
                if(*selectedOption < 0)
                    *selectedOption += usedMenuOptions;
                break;
            case DOWN:
                playSfx(SFX_MOVE, false);spriteTex
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