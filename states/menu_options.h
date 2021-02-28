#include "../main.h"
#include "routine_states.h"

typedef struct 
{
    char txt[20];
    enum prog_state_types prog_state;
} menuOption;

extern menuOption menuOptions[10];

extern void clearMenuOptions();
extern void createMenuOption(char * txt, enum prog_state_types state);
extern void controlMenuOptions(int *selectedOption, enum routine_state_types *menu_state, enum prog_state_types * exit_state);
extern void displayMenuOptions(int selectedOption);