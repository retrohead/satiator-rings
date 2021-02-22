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

#ifndef FONT_H
#define FONT_H
struct font_s
{
   u8 width, height, bpp;
   u8 *data;
   u32 charsize;
   int lineinc;
   u8 *out;
   int (*drawchar)(struct font_s *font, int x, int y, int color, int charnum);
   int screen;
   int transparent;
};

typedef struct font_s font_struct;

extern u8 font_8x8[];
extern u32 font_8x8_size;
extern u8 font_8x16[];
extern u32 font_8x16_size;

int vdp_set_font(int screen, font_struct *font, int transparent);
int vdp_set_default_font(int screen, font_struct *font);
#endif
