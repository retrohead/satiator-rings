#include <jo/jo.h>
#include "../main.h"
#include "../fade.h"
#include "../sprite_manager.h"
#include "../pad_controllers.h"
#include "../background.h"
#include "routine_states.h"

enum routine_state_types game_list_state = ROUTINE_STATE_INITIALIZE;

jo_pos3Df                   pos;
jo_rot3Df                   rot;
jo_palette                  image_pal;

void                        my_input(void)
{
    static int              movement_speed = 0;
    static float            angle_increment = 0.0f;

	if (jo_is_pad1_key_pressed(JO_KEY_UP))
	{
		if ((movement_speed += 10) > 60)
			movement_speed = 60;
	}
	else if (jo_is_pad1_key_pressed(JO_KEY_DOWN))
	{
		if ((movement_speed -= 10) < -60)
			movement_speed = -60;
	}
	else if (movement_speed > 5)
		movement_speed -= 5;
	else if (movement_speed < -5)
		movement_speed += 5;
	else
		movement_speed = 0;

	if (jo_is_pad1_key_pressed(JO_KEY_RIGHT))
		angle_increment += 1.5;
	else if (jo_is_pad1_key_pressed(JO_KEY_LEFT))
        angle_increment -= 1.5;

    if (jo_is_pad1_key_pressed(JO_KEY_L))
        pos.z += 1.0;
	else if (jo_is_pad1_key_pressed(JO_KEY_R))
        pos.z -= 1.0;

    rot.ry = -JO_DEG_TO_RAD(angle_increment);
    rot.rz += JO_DEG_TO_RAD(angle_increment) / 2.0;
    angle_increment = angle_increment * 4.0 / 5.0;

    pos.x -= movement_speed * jo_sin_radf(rot.rz) / 10.0;
	pos.y -= movement_speed * jo_cos_radf(rot.rz) / 10.0;
}

void                            draw_3d_planes(void)
{
    // FLOOR
    jo_3d_push_matrix();
	{
		jo_3d_rotate_matrix_rad(rot.rx, rot.ry, rot.rz);
		jo_3d_translate_matrixf(pos.x, pos.y, pos.z);
        jo_background_3d_plane_a_draw(true);
	}
	jo_3d_pop_matrix();
    //SKY
	jo_3d_push_matrix();
	{
	    jo_3d_translate_matrix(rot.rz * 100, 110 - 112 * jo_cos_radf(rot.rx), 100);
        jo_3d_rotate_matrix_rad_z(-rot.ry);
        jo_background_3d_plane_b_draw(false);
	}
	jo_3d_pop_matrix();
}

jo_palette          *my_tga_palette_handling(void)
{
    // We create a new palette for each image. It's not optimal but OK for a demo.
    jo_create_palette(&image_pal);
    return (&image_pal);
}

void                init_3d_planes(void)
{
    jo_img_8bits    img;

    jo_core_tv_off();

    jo_enable_background_3d_plane(JO_COLOR_Black);

    // FLOOR
    img.data = JO_NULL;
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FLOOR.TGA", 0);
    jo_background_3d_plane_a_img(&img, image_pal.id, true, true);
    jo_free_img(&img);

    //SKY
    img.data = JO_NULL;
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "SKY.TGA", 0);
    jo_background_3d_plane_b_img(&img, image_pal.id, true, false);
    jo_free_img(&img);

    jo_core_tv_on();
}

void logic_gamelist()
{
    debugScreenFade();
    static int callbackId = 0;
    static enum prog_state_types exit_state = PROG_STATE_GAMELIST;
    jo_set_printf_color_index(JO_COLOR_INDEX_Red);
    jo_printf(0, 0, "GAMELIST");
    jo_printf(0, 1, "game_list_state=%d", game_list_state);
    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            jo_set_tga_palette_handling(my_tga_palette_handling);
            init_3d_planes();

            pos.x = 800.0;
            pos.y = 800.0;
            pos.z = -35.0;

            rot.rx = JO_DEG_TO_RAD(90.0);
            rot.ry = JO_DEG_TO_RAD(0.0);
            rot.rz = JO_DEG_TO_RAD(45.0);

	        callbackId = jo_core_add_callback(draw_3d_planes);

            load_sprite_texture("ISO/RS", "BOXSMALL.TGA");
            create_sprite(spriteTex[0].texture_id, 60, 100, 0, 1.0, 1.0, 0);

            fadeInOut(FADE_IN);
            game_list_state = ROUTINE_STATE_FADE_IN;
            exit_state = PROG_STATE_GAMELIST;
            break;
        case ROUTINE_STATE_FADE_IN:
            if(!screenIsFading)
                game_list_state = ROUTINE_STATE_RUN;
            break;
        case ROUTINE_STATE_RUN:
            if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
            {
                fadeInOut(FADE_OUT);
                game_list_state = ROUTINE_STATE_FADE_OUT;
                exit_state = PROG_STATE_MENU;
            }

            if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
            {
                switch(pad_controllers[0].direction_id)
                {
                    case LEFT:
                        break;
                    case RIGHT:
                        break;
                    case UP:
                        break;
                    case DOWN:
                        break;
                }
            }

            if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
            {
                // launch selected game
            }
            break;
        case ROUTINE_STATE_FADE_OUT:
            if(!screenIsFading)
                game_list_state = ROUTINE_STATE_END;
            break;
        case ROUTINE_STATE_END:
	        jo_core_remove_callback(callbackId);
            game_list_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}