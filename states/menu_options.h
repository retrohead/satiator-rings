#include "../main.h"
#include "routine_states.h"

enum menu_option_types
{
    OPTION_PROGRAM_STATE,
    OPTION_CHANGE_VARIABLE
};

typedef struct 
{
    char txt[40];
    enum prog_state_types prog_state;
    enum menu_option_types type;
    int row;
    int sfx;
} menuOption;

extern menuOption menuOptions[10];
extern int selectorSprite;

extern void clearMenuOptions();
extern void createMenuOption(const char * txt, enum prog_state_types state, enum menu_option_types type, int row, int sfx);
extern int controlMenuOptions(int *selectedOption, enum routine_state_types *menu_state, enum prog_state_types * exit_state);
extern void displayMenuOptions(int selectedOption);