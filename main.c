// Satiator Rings Menu

/*
** Jo Sega Saturn Engine
** Copyright (c) 2012-2017, Johannes Fetz (johannesfetz@gmail.com)
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
#include "filelist.h"
#include "satiator_functions.h"

static int gamebox_sprite_id;
static int gamepad_sprite_id;
static int arrow_sprite_id;

char pathbuf[512];

int image_file_filter(file_ent *entry) {
    if (entry->isdir)
        return 1;

    int len = strlen(entry->name);
    if (!strncmp(&entry->name[len-4], ".cue", 4))
        return 1;
    if (!strncmp(&entry->name[len-4], ".iso", 4))
        return 1;

    return 0;
}

void image_menu(void) {
        int nents;
        file_ent *list = file_list_create(".", &nents, image_file_filter);

        //if (nents == 1 && strcmp(pathbuf, "/") && !list[0].isdir)
        //    launch_game(list[0].name);

        file_list_sort(list, nents);
        char namebuf[32];
        char name[32];
        strcpy(namebuf, "Satiator - ");
        char *dirname = strrchr(pathbuf, '/');
        if (dirname[1])
            dirname++;
        strncat(namebuf, dirname, sizeof(namebuf));
		int entry = 0;
        //int entry = menu_picklist(list, nents, namebuf);
        int ret;
        if (entry == -1) {
            if (!strcmp(pathbuf, "/")) {
                return;
            } else {
                char *last_slash = strrchr(pathbuf, '/');
                if (last_slash == pathbuf)
                    last_slash++;
                *last_slash = '\0';
                s_chdir(pathbuf);
            }
        } else if (list[entry].isdir) {
            // Got to strip the slash :(
            list[entry].name[strlen(list[entry].name) - 1] = '\0';
            if (pathbuf[1])
                strcat(pathbuf, "/");
            strcat(pathbuf, list[entry].name);
            ret = s_chdir(pathbuf);
            if (ret != FR_OK) {
                //menu_error("chdir", "Can't change directory, corrupt SD card?");
            }
        }
        else
            strcpy(name, list[entry].name);
        file_list_free(list, nents);

        if (name) {
            //launch_game(name);
            //free(name);
            //name = NULL;
        }
}

void my_draw(void)
{
    static int angle = 0;
    static double scale = 0;
    static double scale_mod = 0.02;

    scale += scale_mod;
    if((scale_mod > 0) && (scale >= 1))
    {
        scale = 1.0;
        scale_mod = -0.02;
    } else if((scale_mod < 0) && (scale <= -1))
    {
        scale = -1.0;
        scale_mod = 0.02;
    }
    
    angle = angle + 1;
    if(angle >= 360)
    {
        angle = 0;
    }
    
	jo_sprite_draw3D(gamepad_sprite_id, 0, 0, 500);
    jo_sprite_change_sprite_scale_xy(scale,1);
	jo_sprite_draw3D(gamebox_sprite_id, 0, 0, 500);
    jo_set_printf_color_index(JO_COLOR_INDEX_Red);

    if(satiatorIsBackupDeviceAvailable(SatiatorBackup))
	    jo_printf(0, 0, "Satiator Detected");
    else
	    jo_printf(0, 0, "Satiator Not Detected");
	
	//image_menu();
}

void my_gamepad(void)
{
    jo_sprite_change_sprite_scale_xy(1,1);
    jo_sprite_draw3D(arrow_sprite_id, 0, 0, 450);
	if (!jo_is_input_available(0))
    {
        jo_set_screen_color_filter_a(JO_SPRITE_SCREEN, 255, 0, 0); // All sprite in red if the controller in not plugged in.
        return ;
    }
    else
        jo_disable_all_screen_color_filter();

    // Note: You can also use jo_is_input_key_* for direction
    switch (jo_get_input_direction_pressed(0))
    {
        case LEFT: jo_sprite_draw3D(arrow_sprite_id, -75, -3, 450); break;
        case RIGHT: jo_sprite_draw3D(arrow_sprite_id, -45, -3, 450); break;
        case UP: jo_sprite_draw3D(arrow_sprite_id, -60, -15, 450); break;
        case DOWN: jo_sprite_draw3D(arrow_sprite_id, -60, 12, 450); break;
        case UP_LEFT: jo_sprite_draw3D(arrow_sprite_id, -72, -12, 450); break;
        case UP_RIGHT: jo_sprite_draw3D(arrow_sprite_id, -48, -12, 450); break;
        case DOWN_LEFT: jo_sprite_draw3D(arrow_sprite_id, -72, 9, 450); break;
        case DOWN_RIGHT: jo_sprite_draw3D(arrow_sprite_id, -48, 9, 450); break;
        case NONE: break;
    }
    if (jo_is_input_key_pressed(0, JO_KEY_START))
        jo_sprite_draw3D(arrow_sprite_id, 0, 17, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_L))
        jo_sprite_draw3D(arrow_sprite_id, -50, -60, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_R))
        jo_sprite_draw3D(arrow_sprite_id, 50, -60, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_A))
        jo_sprite_draw3D(arrow_sprite_id, 41, 20, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_B))
        jo_sprite_draw3D(arrow_sprite_id, 63, 4, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_C))
        jo_sprite_draw3D(arrow_sprite_id, 88, -4, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_X))
        jo_sprite_draw3D(arrow_sprite_id, 33, -7, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_Y))
        jo_sprite_draw3D(arrow_sprite_id, 51, -20, 450);
    if (jo_is_input_key_pressed(0, JO_KEY_Z))
        jo_sprite_draw3D(arrow_sprite_id, 72, -28, 450);
}
void my_background()
{
    jo_img      bg;

    bg.data = NULL;
    jo_tga_loader(&bg, "BG", "BG.TGA", JO_COLOR_Transparent);
    jo_set_background_sprite(&bg, 0, 0);
    jo_free_img(&bg);
}

void jo_main(void)
{
	jo_core_init(JO_COLOR_White);
	my_background();
	gamebox_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "GH.TGA", JO_COLOR_Green);
	gamepad_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "PAD.TGA", JO_COLOR_Green);
	arrow_sprite_id = jo_sprite_add_tga(JO_ROOT_DIR, "ARW.TGA", JO_COLOR_Green);
	jo_core_add_callback(my_gamepad);
	jo_core_add_callback(my_draw);
	jo_core_run();
}

/*
** END OF FILE
*/
