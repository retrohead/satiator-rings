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
#include "states/splash.h"
#include "states/gamelist.h"

enum prog_state_types prog_state = PROG_STATE_INITIALIZE;
enum prog_state_types prog_previous_state = PROG_STATE_INITIALIZE;

void application_logic(void)
{
    updateControllers();
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
	jo_core_init(JO_COLOR_White);
    load_nbg2_font();
    initControllers();
	jo_core_add_callback(application_logic);
	jo_core_add_callback(draw_objects);
	jo_core_run();
}

/*
** END OF FILE
*/
