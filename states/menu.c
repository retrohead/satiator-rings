#include <jo/jo.h>
#include "routine_states.h"

enum routine_state_types menu_state = ROUTINE_STATE_INITIALIZE;

void logic_menu()
{
    jo_printf(0, 2, "MENU");
    jo_printf(0, 4, "menu_state=%d", menu_state);
    switch(menu_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            break;
        case ROUTINE_STATE_RUN:
            break;
        case ROUTINE_STATE_END:
            break;
    }
}