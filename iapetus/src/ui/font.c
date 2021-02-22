/*  Copyright 2007,2013 Theo Berkau

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

#include "../iapetus.h"

extern vdp2_settings_struct vdp2_settings;

//////////////////////////////////////////////////////////////////////////////

int draw_char_vdp1(font_struct *font, int x, int y, int color, int charnum)
{
   sprite_struct sprite;
   u32 src = (u32)font->out + (charnum * font->charsize);

   sprite.attr = SPRITE_PRECLIPENABLE | SPRITE_HIGHSPEEDSHRINKDISABLE |
                 SPRITE_CLIPDISABLE | SPRITE_ENDCODEDISABLE | SPRITE_8BPP256COLOR;
   sprite.addr = src;
//   sprite.bank = RGB16(31, 0, 0) | 0X8000;
   sprite.bank = 0; // fix me
   sprite.width = font->width;
   sprite.height = font->height;
   sprite.x = x;
   sprite.y = y;
   vdp_draw_normal_sprite(&sprite);

   return font->width;
}

//////////////////////////////////////////////////////////////////////////////

int draw_char_bitmap_1bpp_to_8bpp(font_struct *font, int x, int y, int color, int charnum)
{
   int i, i2;
   u8 *src = font->data + (charnum * font->charsize);
   u8 *dest = font->out + (y * font->lineinc) + x;

   for (i = 0; i < (font->height * font->width / 8); i++)
   {
      u8 pixel = src[i];

      for (i2 = 0; i2 < 8; i2++)
      {
         if ((pixel >> 7) & 0x1)
            dest[i2] = (u8)color;
         else if (!font->transparent)
            dest[i2] = 0;
         pixel <<= 1;
      }
      dest += font->lineinc;
   }

   return font->width;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_set_font(int screen, font_struct *font, int transparent)
{
   int i;

   // This is just a basic implementation so I can make sure the basic
   // framework is working
   font->charsize = font->width * font->height * font->bpp / 8;

   if (screen == SCREEN_VDP1)
   {
      font->lineinc = font->width;

      for (i = 0; i < 128; i++)
         draw_char_bitmap_1bpp_to_8bpp(font, 0, i * font->height, 1, i);
      font->drawchar = draw_char_vdp1;
   }
   else
   {
      font->lineinc = 512; // fix me(should be detected)
      font->drawchar = draw_char_bitmap_1bpp_to_8bpp;
      font->out = (u8 *)0x25E00000;
      switch (screen)
      {
         case SCREEN_NBG0:
            font->out += ((u32)vdp2_settings.MPOFN.part.n0mp << 17);
            break;
         case SCREEN_NBG1:
            font->out += ((u32)vdp2_settings.MPOFN.part.n1mp << 17);
            break;
         case SCREEN_NBG2:
            font->out += ((u32)vdp2_settings.MPOFN.part.n2mp << 17);
            break;
         case SCREEN_NBG3:
            font->out += ((u32)vdp2_settings.MPOFN.part.n3mp << 17);
            break;
         default: break;
      }      
   }

   font->screen = screen;
   font->transparent = transparent;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_set_default_font(int screen, font_struct *font)
{
   font->data = font_8x16;
   font->width = 8;
   font->height = 16;
   font->bpp = 1;

   if (screen == SCREEN_VDP1)
      font->out = (u8 *)0x25C20000;
   else
      font->out = (u8 *)0x25E00000;

   return vdp_set_font(screen, font, 0);
}

//////////////////////////////////////////////////////////////////////////////

