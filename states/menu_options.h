#include "../main.h"
#include "routine_states.h"

enum menu_option_types
{
    OPTION_PROGRAM_STATE,
    OPTION_CHANGE_VARIABLE
};

typedef struct 
{
    const char * txt;
    enum prog_state_types prog_state;
    enum menu_option_types type;
} menuOption;

extern menuOption menuOptions[10];

extern void clearMenuOptions();
extern void createMenuOption(const char * txt, enum prog_state_types state, enum menu_option_types type);
extern int controlMenuOptions(int *selectedOption, enum routine_state_types *menu_state, enum prog_state_types * exit_state);
extern void displayMenuOptions(int selectedOption);