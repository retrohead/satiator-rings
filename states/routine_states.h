#include "options.h"

#define GAME_LIST_MAX_ITEMS 22
#define GAME_LIST_MAX_ITEM_LEN 27
#define MAX_FAVOURITES 100

extern void displayStatusText(char * text, bool vblank);
#define displayStatus(...)		do {sprintf(debug_buf, __VA_ARGS__); displayStatusText(debug_buf, false); } while(0)
#define displayStatusVblank(...)		do {sprintf(debug_buf, __VA_ARGS__); displayStatusText(debug_buf, true); } while(0)

enum routine_state_types
{
    ROUTINE_STATE_INITIALIZE,
    ROUTINE_STATE_RUN,
    ROUTINE_STATE_END,
    ROUTINE_STATE_END_CANCEL
};
extern int logosprites[3];

extern int routine_scene;
extern void displayTime();
extern void displayVersion();
extern void logic_gamelist();
extern void logic_bootscreen();
extern void logic_init();
extern void logic_splash();
extern void logic_menu();
extern void logic_exit_menu();
extern void logic_console_info();
extern void logic_satiator_info();
extern void logic_themelist();
extern bool bootLastGame();