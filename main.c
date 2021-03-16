/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2020, Johannes Fetz (johannesfetz@gmail.com)
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the Johannes Fetz nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL Johannes Fetz BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <jo/jo.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "ini.h"
#include "states/routine_states.h"
#include "satiator_functions.h"
#include "satiator/iapetus/video/vdp.h"

enum prog_state_types prog_state = PROG_STATE_INITIALIZE;
enum prog_state_types prog_previous_state = PROG_STATE_INITIALIZE;
int routine_scene = 0;
jo_datetime dt;
void application_logic(void)
{
    jo_getdate(&dt);
    updateControllers();
    processSfxDelays();
    prog_previous_state = prog_state;
    switch(prog_state)
    {
        case PROG_STATE_INITIALIZE:
            init_background();
            init_sprites();
            prog_state = PROG_STATE_SPLASH;
            break;
        case PROG_STATE_SPLASH:
            logic_splash();
            break;
        case PROG_STATE_GAMELIST:
            logic_gamelist();
            break;
        case PROG_STATE_QUICKBOOT:
            // load the most recent game
            if(bootLastGame())
            {
                displayStatus("Booting");
                prog_state = PROG_STATE_BOOT;
            } else
            {
                displayStatus("Failed Quick Boot");
                prog_state = PROG_STATE_GAMELIST;
            }
            break;
        case PROG_STATE_BOOT:
            logic_bootscreen();
            break;
        case PROG_STATE_MENU:
            logic_menu();
            break;
        case PROG_STATE_OPTIONS:
            logic_options();
            break;
        case PROG_STATE_CONSOLE_INFO:
            logic_console_info();
            break;
        case PROG_STATE_SATIATOR_INFO:
            logic_satiator_info();
            break;
        case PROG_STATE_EXIT:
            s_chdir("/");
            s_mode(s_cdrom);
            jo_core_restart_saturn();
            //jo_core_exit_to_multiplayer();
            break;
        case PROG_STATE_EXIT_TO_MENU:
            dirEntyCount = 1;
            selectedDirEntry = 0;
            if(dirEntries[0].name != NULL)
                jo_free(dirEntries[0].name);
            dirEntries[0].name = jo_malloc(10);
            dirEntries[0].type = DIR_GAME;
            strcpy(dirEntries[0].name, "origmenu");
            prog_state = PROG_STATE_BOOT;
            break;
    }
}

void draw_objects(void)
{
    if(prog_previous_state != prog_state)
    {
        // changing state, clear all the system
        //jo_clear_screen();
        init_sprites();
        init_background();
    } else
    {
        draw_sprites();
    }
}

void			jo_main(void)
{
	jo_core_init(JO_COLOR_Black);
    initDirEntries();
    load_fonts();
    initSfx();
    initControllers();
	jo_core_add_callback(application_logic);
	jo_core_add_callback(draw_objects);
	jo_core_run();
}

/*
** END OF FILE
*/
