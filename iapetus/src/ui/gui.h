/*  Copyright 2006-2008,2013 Theo Berkau

    This file is part of Iapetus.

    Iapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Iapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Iapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GUI_H
#define GUI_H

typedef struct
{
   int x;
   int y;
   int width;
   int height;
   int fgcolor;
   int bgcolor;
} window_struct;

typedef struct
{
   s8 name[26];
   void (*func)();
} menu_item_struct;

typedef struct  
{
} hmenu_struct;

enum GUI_MES
{
	GM_INIT,
	GM_KEY,
	GM_BUTTON,
	GM_MOUSEMOVE,
	GM_PAINT,
	GM_DEINIT
};

typedef struct  
{
	struct hgui_struct *hgui;
	enum GUI_MES msg;
	u32 param1;
	u32 param2;
} gui_msg_buf_struct;

#define GMS_CAPTION	(1 << 0)

typedef struct  
{
	char *name;
	u32 style;
	int x;
	int y;
	int width;
	int height;
   struct hgui_struct *hgui_parent;
	hmenu_struct *menu;
	u32 param;
	font_struct font;
	int (*func)(struct hgui_struct *hgui, enum GUI_MES msg, u32 param1, u32 param2);
} hgui_struct;

#define GBM_OK			(1 << 0)
#define GBM_SAVE		(1 << 1)
#define GBM_CANCEL	(1 << 2)
#define GBM_CLOSE		(1 << 3)
#define GBM_YES		(1 << 4)
#define GBM_NO			(1 << 5)
#define GBM_ABORT		(1 << 6)
#define GBM_RETRY		(1 << 7)

#define MTYPE_LEFTALIGN         0x00000000
#define MTYPE_CENTER            0x00000001
#define MTYPE_RIGHTALIGN        0x00000002

enum IAPETUS_ERR gui_post_message(hgui_struct *hgui, enum GUI_MES msg, u32 param1, u32 param2);
int gui_send_message(hgui_struct *hgui, enum GUI_MES msg, u32 param1, u32 param2);
enum IAPETUS_ERR gui_get_message(gui_msg_buf_struct *out_msg, hgui_struct *hgui);
enum IAPETUS_ERR gui_translate_message(gui_msg_buf_struct *msg);
enum IAPETUS_ERR gui_dispatch_message(gui_msg_buf_struct *msg);
hgui_struct *gui_create_window(char *name, u32 style, int x, int y, int width, int height, 
	hgui_struct *hgui_parent, hmenu_struct *menu, u32 param);
enum IAPETUS_ERR gui_destroy_window(hgui_struct *hgui);
int gui_do_menu(menu_item_struct *menu, font_struct *font, int x, int y, const char *title, int flags, int num_lines);
void gui_clear_scr(font_struct *font);

void gui_window_init(void);
void gui_window_draw(int x, int y, int width, int height, BOOL draw_caption, u16 fgcolor, u16 bgcolor);

extern font_struct testdispfont;

#endif
