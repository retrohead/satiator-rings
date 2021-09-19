#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"
#include "../bios.h"

enum routine_state_types console_info_state = ROUTINE_STATE_INITIALIZE;

void logic_console_info()
{
    static int selectedMenuOption = 0;
    static enum prog_state_types exit_state = PROG_STATE_MENU;
    switch(console_info_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            clearMenuOptions();
            displayMenuOptions(selectedMenuOption);
            createGuiBoxes("CONSOLE.TGA", false);
            draw_sprites();
            slSynch();
            displayVersion();
            displayStatus("Press any button to continue");
            jo_nbg2_printf(1, 10, "Cartridge     : %s", getCartridgeString());

            int total = jo_backup_get_total_block_count(JoInternalMemoryBackup );
            jo_nbg2_printf(1, 11, "System        : %d/%d Blocks", total - jo_backup_get_free_block_count(JoInternalMemoryBackup ), total);
            console_info_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_MENU;
            break;
        case ROUTINE_STATE_RUN:
            displayTime();            
            jo_nbg2_printf(1, 8,  "Console Region: %s", getRegionString());
            jo_nbg2_printf(1, 9,  "Console Date  : %d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.second);
            controlMenuOptions(&selectedMenuOption, &console_info_state, &exit_state);
            break;
        case ROUTINE_STATE_END_CANCEL:
        case ROUTINE_STATE_END:
            routine_scene = 0;
            console_info_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}