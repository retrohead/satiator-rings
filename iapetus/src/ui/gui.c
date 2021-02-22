/*  Copyright 2006-2009, 2013 Theo Berkau

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

#include <string.h>
#include <stdlib.h>
#include "../iapetus.h"
#include "gui.h"

#define MAX_GUI_MSG 10
gui_msg_buf_struct gui_msg_buf[MAX_GUI_MSG];
int gui_msg_cnt = 0;

//////////////////////////////////////////////////////////////////////////////

enum IAPETUS_ERR gui_post_message(hgui_struct *hgui, enum GUI_MES msg, u32 param1, u32 param2)
{
	gui_msg_buf_struct *msg_buf;
	if (gui_msg_cnt >= MAX_GUI_MSG)
	   return IAPETUS_ERR_OUTOFMEMORY;
	msg_buf = gui_msg_buf+gui_msg_cnt;
	msg_buf->hgui = (struct hgui_struct *)hgui;
	msg_buf->msg = msg;
	msg_buf->param1 = param1;
	msg_buf->param1 = param2;
	gui_msg_cnt++;
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int gui_send_message(hgui_struct *hgui, enum GUI_MES msg, u32 param1, u32 param2)
{
	return hgui->func((struct hgui_struct *)hgui, msg, param1, param2);
}

//////////////////////////////////////////////////////////////////////////////

enum IAPETUS_ERR gui_get_message(gui_msg_buf_struct *out_msg, hgui_struct *hgui)
{
   int i;
   if (gui_msg_cnt == 0)
   {
      for (;;)
      {
         u16 push_once;
         vdp_vsync(); 

         push_once = per[0].but_push_once >> 3;
         // poll peripheral
         for (i = 3; i < 15; i++)
         {
            if (push_once & 1)
               gui_post_message(hgui, GM_BUTTON, 1 << i, 0);
            push_once >>= 1;
         }
         if (gui_msg_cnt != 0)
            break;
      }
   }

   for (i = 0; i < gui_msg_cnt; i++)
   {
      if ((u32)gui_msg_buf[i].hgui == (u32)hgui)
      {
         memcpy(out_msg, gui_msg_buf+i, sizeof(gui_msg_buf_struct));
         
         if (i+1 < gui_msg_cnt)
            memcpy(gui_msg_buf+i, gui_msg_buf+i+1, (gui_msg_cnt-(i+1))*sizeof(gui_msg_buf_struct));
      }
   }

	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

enum IAPETUS_ERR gui_translate_message(gui_msg_buf_struct *msg)
{
	// stub
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

enum IAPETUS_ERR gui_dispatch_message(gui_msg_buf_struct *msg)
{
	((hgui_struct *)msg->hgui)->func(msg->hgui, msg->msg, msg->param1, msg->param2);
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int gui_message_box(char *title, char *text, u32 butmask)
{
	// stub
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void gui_clear_scr(font_struct *font)
{
   vdp_disp_off();
   vdp_clear_screen(font);
   vdp_disp_on();
}

//////////////////////////////////////////////////////////////////////////////

int gui_do_menu(menu_item_struct *menu, font_struct *font, int x, int y, const char *title, int flags, int num_lines)
{
   int cursel=0;
   int nummenu=0;
   int i;
   int menux, menuy;
	int width, height;
   int win_width, win_height;
   int len, maxlen=0;
   int old_transparent;

	vdp_get_scr_width_height(&width, &height);

   gui_window_init();

	old_transparent = font->transparent;
	font->transparent = 1;

   // Figure out how many menu items there are, and get the longest one
   for (;;)
   {     
      if (menu[nummenu].name[0] == '\0') break;
      len = strlen((char *)menu[nummenu].name);
      if (len > maxlen)
         maxlen = len;
      nummenu++;
   }

   // Make sure title isn't longer either
   len = strlen(title);
   if (len > maxlen)
      maxlen = len;

   win_width = 6 + 8 + 6 + (maxlen * font->width);
   if (num_lines == -1)
      win_height = 5+5+5+8+nummenu*8;
   else
   	  win_height = 5+5+5+8+num_lines*8;

   if (flags & MTYPE_CENTER)
   {
      x = ((width - win_width) >> 1) + x;
      y = ((height - win_height) >> 1) + y;
   }
   else if (flags & MTYPE_RIGHTALIGN)
   {
      x = width - win_width - x;
      y = height - win_height - y;
   }

   gui_window_draw(x, y, win_width, win_height, TRUE, 0, RGB16(26, 26, 25) | 0x8000);

   // Draw title
   vdp_printf(font, x+6, y+5, 15, "%s", title);

   menux = x + 6;
   menuy = y + 5 + 8 + 5;

   // Add Menu Sprite to draw list
   for (i = 0; i < nummenu; i++)
   {
      if (num_lines != -1 && i >= num_lines)
         break;

      vdp_printf(font, menux + (1 * 8) + 1, menuy + (i * 8)+1, 0x10, (char *)menu[i].name);
      if (menu[i].func)
         vdp_printf(font, menux + (1 * 8), menuy + (i * 8), 0xF, (char *)menu[i].name);
      else
         vdp_printf(font, menux + (1 * 8), menuy + (i * 8), 0x7, (char *)menu[i].name);
   }

   // Add Selected Menu Item(should always be first item) sprite to draw list
   vdp_printf(font, menux, menuy+(cursel * 8), 0xF, ">");

   for (;;)
   {
      vdp_vsync(); 

      // poll joypad(if menu item is selected, return)
      if (per[0].but_push_once & PAD_UP)
      {
         vdp_printf(font, menux, menuy + (cursel * 8), 0, ">");

         if (cursel != 0)
            cursel--;
         else
            cursel = (nummenu - 1);

         vdp_printf(font, menux, menuy + (cursel * 8), 0xF, ">");
      }
      else if (per[0].but_push_once & PAD_DOWN)
      {
         vdp_printf(font, menux, menuy + (cursel * 8), 0x0, ">");

         if (cursel != (nummenu - 1))
            cursel++;
         else
            cursel = (nummenu - 1);

         vdp_printf(font, menux, menuy + (cursel * 8), 0xF, ">");
      }

      if (per[0].but_push_once & PAD_A)
      {
         gui_clear_scr(font);

         vdp_start_draw_list();
         vdp_end_draw_list();

         if (menu[cursel].func)
            menu[cursel].func();
         break;
      }
      else if (per[0].but_push_once & PAD_B)
      {
         gui_clear_scr(font);

         vdp_start_draw_list();
         vdp_end_draw_list();
         cursel = -1;
         break;
      }
   }
   font->transparent = old_transparent;
   return cursel;
}

//////////////////////////////////////////////////////////////////////////////

hgui_struct *gui_create_window(char *name, u32 style, int x, int y, int width, int height, 
                               hgui_struct *hgui_parent, hmenu_struct *menu, u32 param)
{
   hgui_struct *hgui = (hgui_struct *)malloc(sizeof(hgui_struct));

   if (hgui == NULL)
       return NULL;
   strcpy(hgui->name, name);
   hgui->style = style;
   hgui->x = x;
   hgui->y = y;
   hgui->width = width;
   hgui->height = height;
   hgui->hgui_parent = (struct hgui_struct *)hgui_parent;
   hgui->menu = menu;

   gui_send_message(hgui, GM_INIT, param, 0);   
   return hgui;
}

//////////////////////////////////////////////////////////////////////////////

enum IAPETUS_ERR gui_destroy_window(hgui_struct *hgui)
{
   if (hgui)
   {
      gui_send_message(hgui, GM_DEINIT, 0, 0);
      free(hgui);
      return IAPETUS_ERR_OK;
   }
   return IAPETUS_ERR_INVALIDPOINTER;
}   

//////////////////////////////////////////////////////////////////////////////

int gui_def_win_func(hgui_struct *hgui, enum GUI_MES msg, u32 param1, u32 param2)
{
   switch (msg)
   {
	   case GM_INIT:
         return TRUE;
	   case GM_PAINT:						
		{
			BOOL draw_caption = (hgui->style & GMS_CAPTION) == GMS_CAPTION;
         gui_window_draw(hgui->x, hgui->y, hgui->width, hgui->height, draw_caption, 0, RGB16(26, 26, 25) | 0x8000);
			// Draw caption text
			if (draw_caption)				
			   vdp_printf(&hgui->font, hgui->x+6, hgui->y+5, 15, "%s", hgui->name);
         return TRUE;
		}
	   case GM_DEINIT:
         return TRUE;
      default: break;
   }

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

void gui_window_init()
{
	// Setup Priorities
	VDP2_REG_PRIR = 0x0002;
	VDP2_REG_PRISA = 0x0101;
	VDP2_REG_PRISB = 0x0101;
	VDP2_REG_PRISC = 0x0101;
	VDP2_REG_PRISD = 0x0101;
}

//////////////////////////////////////////////////////////////////////////////

void gui_window_draw(int x, int y, int width, int height, BOOL draw_caption, u16 fgcolor, u16 bgcolor)
{
   sprite_struct localcoord;
   sprite_struct sprite;

   localcoord.attr = 0;
   localcoord.x = 0;
   localcoord.y = 0;

   vdp_start_draw_list();
   vdp_local_coordinate(&localcoord);

   sprite.attr = SPRITE_PRECLIPENABLE | SPRITE_HIGHSPEEDSHRINKDISABLE |
                 SPRITE_CLIPDISABLE | SPRITE_ENDCODEDISABLE | SPRITE_16BPP;
   sprite.bank = bgcolor;
   sprite.x = x;
   sprite.y = y;
   sprite.x2 = x+width;
   sprite.y2 = y;
   sprite.x3 = x+width;
   sprite.y3 = y+height;
   sprite.x4 = x;
   sprite.y4 = y+height;

   vdp_draw_polygon(&sprite);

   // White lines
   sprite.x += 1;
   sprite.y += 1;
   sprite.x2 -= 1;
   sprite.y2 += 1;
   sprite.bank = RGB16(31, 31, 31) | 0X8000;
   vdp_draw_line(&sprite);

   sprite.x2 = sprite.x;
   sprite.y2 = sprite.y4-1;
   vdp_draw_line(&sprite);

   // Gray lines
   sprite.x = sprite.x2;
   sprite.y = sprite.y2;
   sprite.x2 = sprite.x3-1;
   sprite.y2 = sprite.y3-1;
   sprite.bank = RGB16(16, 16, 16) | 0X8000;
   vdp_draw_line(&sprite);

   sprite.x = sprite.x2;
   sprite.y = y+1;
   vdp_draw_line(&sprite);

   // Dark lines
   sprite.x+=1;
   sprite.y-=1;
   sprite.x2+=1;
   sprite.y2+=1;
   sprite.bank = RGB16(8, 8, 8) | 0X8000;
   vdp_draw_line(&sprite);

   sprite.x = x;
   sprite.y = sprite.y2;
   vdp_draw_line(&sprite);

   // Caption
	if (draw_caption)
	{
		sprite.x = x+3;
		sprite.y = y+3;
		sprite.x2 = x+width-3;
		sprite.y2 = sprite.y;
		sprite.x3 = sprite.x2;
		sprite.y3 = y+12;
		sprite.x4 = sprite.x;
		sprite.y4 = sprite.y3;
		sprite.bank = RGB16(1, 4, 13) | 0X8000;
		vdp_draw_polygon(&sprite);
	}

   vdp_end_draw_list();
}

//////////////////////////////////////////////////////////////////////////////
