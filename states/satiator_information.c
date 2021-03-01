#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "menu_options.h"
#include "../satiator_functions.h"

enum routine_state_types satiator_info_state = ROUTINE_STATE_INITIALIZE;

void logic_satiator_info()
{
    static int selectedMenuOption = 0;
    static enum prog_state_types exit_state = PROG_STATE_MENU;

    char fwVer[256];
    uint32_t ver;
    uint32_t serial;

    switch(satiator_info_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            clearMenuOptions();
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);

            s_get_fw_version(fwVer, sizeof(fwVer));
            jo_nbg2_printf(1, 8, "Firmware Ver  : %s", fwVer);
            s_get_bootloader_version(&ver);
            jo_nbg2_printf(1, 9, "Bootloader Ver: %d", ver);
            s_get_serial_number(&serial);
            jo_nbg2_printf(1, 10, "Serial Number : %d", serial);


            displayMenuOptions(selectedMenuOption);
            satiator_info_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_MENU;
            break;
        case ROUTINE_STATE_RUN:
            jo_nbg2_printf(13, 4, "SATIATOR INFO");
            if(dt.second % 2 == 0)
                jo_nbg2_printf(33, 4, "%02d %02d", dt.hour, dt.minute);
            else
                jo_nbg2_printf(33, 4, "%02d:%02d", dt.hour, dt.minute);

            controlMenuOptions(&selectedMenuOption, &satiator_info_state, &exit_state);
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            satiator_info_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}