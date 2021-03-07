#include "options.h"

enum routine_state_types
{
    ROUTINE_STATE_INITIALIZE,
    ROUTINE_STATE_RUN,
    ROUTINE_STATE_END
};

extern int routine_scene;

extern void logic_gamelist();
extern void logic_bootscreen();
extern void logic_splash();
extern void logic_menu();
extern void logic_exit_menu();
extern void logic_console_info();
extern void logic_satiator_info();