// Basic GUI window program

#include <iapetus.h>

//////////////////////////////////////////////////////////////////////////////

int main()
{
	screen_settings_struct settings;
	font_struct font;
	gui_msg_buf_struct msg;
	hgui_struct *win;

	// This should always be called at the begining of your program.
	init_iapetus(RES_320x224);

	// Setup a screen for us draw on
	settings.is_bitmap = TRUE;
	settings.bitmap_size = BG_BITMAP512x256;
	settings.transparent_bit = 0;
	settings.color = BG_256COLOR;
	settings.special_priority = 0;
	settings.special_color_calc = 0;
	settings.extra_palette_num = 0;
	settings.map_offset = 0;
	settings.rotation_mode = 0;
	settings.parameter_addr = 0x25E60000;
	vdp_rbg0_init(&settings);

	// Use the default palette
	vdp_set_default_palette();

	// Setup the default 8x16 1BPP font
	vdp_set_default_font(SCREEN_RBG0, &font);

	win = gui_create_window("Example", GMS_CAPTION, 0, 0, 320, 224, NULL, NULL, 0);

	// Display everything
	vdp_disp_on();

	// Time to do main loop
	while(gui_get_message(&msg, win) > 0)
	{
		gui_translate_message(&msg);
		gui_dispatch_message(&msg);
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////
