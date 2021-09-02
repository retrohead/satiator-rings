#include <string.h>
#include "menu_options.h"
#include "../direntry.h"


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

void createMenuOption(const char * txt, enum prog_state_types state, enum menu_option_types type, int row, int sfx)
{
    if(usedMenuOptions >= MAX_MENU_OPTIONS)
        return;
    for(int i = 0; i<MAX_MENU_OPTIONS;i++)
    {
        if(menuOptions[i].prog_state == PROG_STATE_INITIALIZE)
        {
            menuOptions[i].prog_state = state;
            strcpy(menuOptions[i].txt, txt);
            menuOptions[i].type = type;
            menuOptions[i].row = row;
            menuOptions[i].sfx = sfx;
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
            updateSelectionSprite(menuOptions[i].row, false, false);
    }
}

int controlMenuOptions(int *selectedOption, enum routine_state_types *menu_state, enum prog_state_types * exit_state)
{
    static int delay = 0;
    if((pad_controllers[controllerMonitor].direction_status == BUTTON_STATE_NEWPRESS) || ((pad_controllers[controllerMonitor].direction_status == BUTTON_STATE_HELD) && (*selectedOption == OPTIONS_SOUND_VOLUME)))
    {
        switch(pad_controllers[controllerMonitor].direction_id)
        {
            case LEFT:
            case UP_LEFT:
            case DOWN_LEFT:
                if(menuOptions[*selectedOption].type != OPTION_PROGRAM_STATE)
                {
                    if((pad_controllers[controllerMonitor].direction_status == BUTTON_STATE_HELD) && (delay < 10))
                    {
                        if(*selectedOption != OPTIONS_SOUND_VOLUME)
                            break;
                        delay++;
                        break;
                    }
                    if(pad_controllers[controllerMonitor].direction_status == BUTTON_STATE_NEWPRESS)
                        delay = 0;
                    playSfx(SFX_OPTION, false);
                    return -1;
                }
                break;
            case RIGHT:
            case DOWN_RIGHT:
            case UP_RIGHT:
                if(menuOptions[*selectedOption].type != OPTION_PROGRAM_STATE)
                {
                    if((pad_controllers[controllerMonitor].direction_status == BUTTON_STATE_HELD) && (delay < 10))
                    {
                        if(*selectedOption != OPTIONS_SOUND_VOLUME)
                            break;
                        delay++;
                        break;
                    }
                    if(pad_controllers[controllerMonitor].direction_status == BUTTON_STATE_NEWPRESS)
                        delay = 0;
                    playSfx(SFX_OPTION, false);
                    return 1;
                }
                break;
            case UP:
                if((pad_controllers[controllerMonitor].direction_status != BUTTON_STATE_HELD) && (usedMenuOptions > 0))
                {
                    playSfx(SFX_MOVE, false);
                    *selectedOption = *selectedOption - 1;
                    if(*selectedOption < 0)
                        *selectedOption += usedMenuOptions;
                }
                break;
            case DOWN:
                if((pad_controllers[controllerMonitor].direction_status != BUTTON_STATE_HELD) && (usedMenuOptions > 0))
                {
                    playSfx(SFX_MOVE, false);
                    *selectedOption = *selectedOption + 1;
                    if(*selectedOption >= usedMenuOptions)
                        *selectedOption -= usedMenuOptions;
                }
                break;
        }
        displayMenuOptions(*selectedOption);
    }
    if(pad_controllers[controllerMonitor].btn_b == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_CHANGE, false);
        *menu_state = ROUTINE_STATE_END_CANCEL;
        return 0;
    }
    if(menuOptions[*selectedOption].type == OPTION_PROGRAM_STATE)
    {
        if((pad_controllers[controllerMonitor].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[controllerMonitor].btn_c == BUTTON_STATE_NEWPRESS))
        {
            playSfx(menuOptions[*selectedOption].sfx, false);
            *menu_state = ROUTINE_STATE_END;
            if(usedMenuOptions > 0)
                *exit_state = menuOptions[*selectedOption].prog_state;
        }
    }
    return 0;
}