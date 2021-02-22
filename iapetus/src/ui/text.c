/*  Copyright 2006-2007,2013 Theo Berkau

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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "../iapetus.h"

//////////////////////////////////////////////////////////////////////////////

extern vdp2_settings_struct vdp2_settings;

//////////////////////////////////////////////////////////////////////////////

void vdp_print_text(font_struct *font, int x, int y, int color, const char *text)
{
   int i;
   int length = strlen(text);

   for (i = 0; i < length; i++)
      x += font->drawchar(font, x, y, color, text[i]);
}

//////////////////////////////////////////////////////////////////////////////

void vdp_printf(font_struct *font, int x, int y, int color, char *format, ...)
{
   char string[256]; // I hope that's enough
   va_list arg;

   va_start(arg, format);
   vsprintf(string, format, arg);
   vdp_print_text(font, x, y, color, string);
   va_end(arg);
}

//////////////////////////////////////////////////////////////////////////////

void vdp_clear_screen(font_struct *font)
{
   int i;

   if (font->screen == SCREEN_VDP1)
   {
      vdp_start_draw_list();
      vdp_end_draw_list();
   }
   else
   {
      // Figure out whether we're dealing with bitmap or tiled(fix me)
//      if (bitmap)
      {
         for (i = 0; i < (512 * 256 / 4); i++)
            ((u32 *)font->out)[i] = 0;
      }
//      else
//      {
//      }
   }
}

//////////////////////////////////////////////////////////////////////////////

