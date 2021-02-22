/*  Copyright 2007 Theo Berkau

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

// Copy a VGA palette to Color Ram

#include "../iapetus.h"

#define RGB555(r,g,b) ((r) | ((g) << 5) | ((b) << 10))

u16 defaultpalette[256] =
{
   RGB555(0,0,0),    RGB555(0,0,21),   RGB555(0,21,0),   RGB555(0,21,21),
   RGB555(21,0,0),   RGB555(21,0,21),  RGB555(21,10,0),  RGB555(21,21,21),
   RGB555(10,10,10), RGB555(10,10,31), RGB555(10,31,10), RGB555(10,31,31),
   RGB555(31,10,10), RGB555(31,10,31), RGB555(31,31,10), RGB555(31,31,31),
   RGB555(0,0,0),    RGB555(2,2,2),    RGB555(4,4,4),    RGB555(5,5,5), 
   RGB555(7,7,7),    RGB555(8,8,8),    RGB555(10,10,10), RGB555(12,12,12),
   RGB555(14,14,14), RGB555(16,16,16), RGB555(18,18,18), RGB555(20,20,20),
   RGB555(22,22,22), RGB555(25,25,25), RGB555(28,28,28), RGB555(31,31,31),
   RGB555(0,0,31),   RGB555(8,0,31),   RGB555(15,0,31),  RGB555(23,0,31),
   RGB555(31,0,31),  RGB555(31,0,23),  RGB555(31,0,15),  RGB555(31,0,8),
   RGB555(31,0,0),   RGB555(31,8,0),   RGB555(31,15,0),  RGB555(31,23,0),
   RGB555(31,31,0),  RGB555(23,31,0),  RGB555(15,31,0),  RGB555(8,31,0),
   RGB555(0,31,0),   RGB555(0,31,8),   RGB555(0,31,15),  RGB555(0,31,23),
   RGB555(0,31,31),  RGB555(0,23,31),  RGB555(0,15,31),  RGB555(0,8,31),
   RGB555(15,15,31), RGB555(19,15,31), RGB555(23,15,31), RGB555(27,15,31),
   RGB555(31,15,31), RGB555(31,15,27), RGB555(31,15,23), RGB555(31,15,19),
   RGB555(31,15,15), RGB555(31,19,15), RGB555(31,23,15), RGB555(31,27,15),
   RGB555(31,31,15), RGB555(27,31,15), RGB555(23,31,15), RGB555(19,31,15),
   RGB555(15,31,15), RGB555(15,31,19), RGB555(15,31,23), RGB555(15,31,27),
   RGB555(15,31,31), RGB555(15,27,31), RGB555(15,23,31), RGB555(15,19,31),
   RGB555(22,22,31), RGB555(24,22,31), RGB555(27,22,31), RGB555(29,22,31),
   RGB555(31,22,31), RGB555(31,22,29), RGB555(31,22,27), RGB555(31,22,24),
   RGB555(31,22,22), RGB555(31,24,22), RGB555(31,27,22), RGB555(31,29,22),
   RGB555(31,31,22), RGB555(29,31,22), RGB555(27,31,22), RGB555(24,31,22),
   RGB555(22,31,22), RGB555(22,31,24), RGB555(22,31,27), RGB555(22,31,29),
   RGB555(22,31,31), RGB555(22,29,31), RGB555(22,27,31), RGB555(22,24,31),
   RGB555(0,0,14),   RGB555(3,0,14),   RGB555(7,0,14),   RGB555(10,0,14),
   RGB555(14,0,14),  RGB555(14,0,10),  RGB555(14,0,7),   RGB555(14,0,3),
   RGB555(14,0,0),   RGB555(14,3,0),   RGB555(14,7,0),   RGB555(14,10,0),
   RGB555(14,14,0),  RGB555(10,14,0),  RGB555(7,14,0),   RGB555(3,14,0),
   RGB555(0,14,0),   RGB555(0,14,3),   RGB555(0,14,7),   RGB555(0,14,10),
   RGB555(0,14,14),  RGB555(0,10,14),  RGB555(0,7,14),   RGB555(0,3,14),
   RGB555(7,7,14),   RGB555(8,7,14),   RGB555(10,7,14),  RGB555(12,7,14),
   RGB555(14,7,14),  RGB555(14,7,12),  RGB555(14,7,10),  RGB555(14,7,8),
   RGB555(14,7,7),   RGB555(14,8,7),   RGB555(14,10,7),  RGB555(14,12,7),
   RGB555(14,14,7),  RGB555(12,14,7),  RGB555(10,14,7),  RGB555(8,14,7),
   RGB555(7,14,7),   RGB555(7,14,8),   RGB555(7,14,10),  RGB555(7,14,12),
   RGB555(7,14,14),  RGB555(7,12,14),  RGB555(7,10,14),  RGB555(7,8,14),
   RGB555(10,10,14), RGB555(11,10,14), RGB555(12,10,14), RGB555(13,10,14),
   RGB555(14,10,14), RGB555(14,10,13), RGB555(14,10,12), RGB555(14,10,11),
   RGB555(14,10,10), RGB555(14,11,10), RGB555(14,12,10), RGB555(14,13,10),
   RGB555(14,14,10), RGB555(13,14,10), RGB555(12,14,10), RGB555(11,14,10),
   RGB555(10,14,10), RGB555(10,14,11), RGB555(10,14,12), RGB555(10,14,13),
   RGB555(10,14,14), RGB555(10,13,14), RGB555(10,12,14), RGB555(10,11,14),
   RGB555(0,0,8),    RGB555(2,0,8),    RGB555(4,0,8),    RGB555(6,0,8),
   RGB555(8,0,8),    RGB555(8,0,6),    RGB555(8,0,4),    RGB555(8,0,2),
   RGB555(8,0,0),    RGB555(8,2,0),    RGB555(8,4,0),    RGB555(8,6,0),
   RGB555(8,8,0),    RGB555(6,8,0),    RGB555(4,8,0),    RGB555(2,8,0),
   RGB555(0,8,0),    RGB555(0,8,2),    RGB555(0,8,4),    RGB555(0,8,6),
   RGB555(0,8,8),    RGB555(0,6,8),    RGB555(0,4,8),    RGB555(0,2,8),
   RGB555(4,4,8),    RGB555(5,4,8),    RGB555(6,4,8),    RGB555(7,4,8),
   RGB555(8,4,8),    RGB555(8,4,7),    RGB555(8,4,6),    RGB555(8,4,5),
   RGB555(8,4,4),    RGB555(8,5,4),    RGB555(8,6,4),    RGB555(8,7,4),
   RGB555(8,8,4),    RGB555(7,8,4),    RGB555(6,8,4),    RGB555(5,8,4),
   RGB555(4,8,4),    RGB555(4,8,5),    RGB555(4,8,6),    RGB555(4,8,7),
   RGB555(4,8,8),    RGB555(4,7,8),    RGB555(4,6,8),    RGB555(4,5,8),
   RGB555(5,5,8),    RGB555(6,5,8),    RGB555(6,5,8),    RGB555(7,5,8),
   RGB555(8,5,8),    RGB555(8,5,7),    RGB555(8,5,6),    RGB555(8,5,6),
   RGB555(8,5,5),    RGB555(8,6,5),    RGB555(8,6,5),    RGB555(8,7,5),
   RGB555(8,8,5),    RGB555(7,8,5),    RGB555(6,8,5),    RGB555(6,8,5),
   RGB555(5,8,5),    RGB555(5,8,6),    RGB555(5,8,6),    RGB555(5,8,7),
   RGB555(5,8,8),    RGB555(5,7,8),    RGB555(5,6,8),    RGB555(5,6,8),
   RGB555(0,0,0),    RGB555(0,0,0),    RGB555(0,0,0),    RGB555(0,0,0),
   RGB555(0,0,0),    RGB555(0,0,0),    RGB555(0,0,0),    RGB555(0,0,0)
};

//////////////////////////////////////////////////////////////////////////////

int vdp_set_palette(int type, void *palette, int size)
{
   int i;

   switch (type)
   {
      case CRM5_1024:
      case CRM5_2048:
         // Set color ram mode
         VDP2_REG_RAMCTL = (VDP2_REG_RAMCTL & 0xCFFF) | (type << 12);
         // Copy over palette to color ram
         for (i = 0; i < (size * 2); i+=2)
            *((volatile u16 *)(VDP2_CRAM+i)) = *((u16 *)(palette+i));
         break;
      case CRM8_1024:
         // Set color ram mode
         VDP2_REG_RAMCTL = (VDP2_REG_RAMCTL & 0xCFFF) | 0x3000;
         // Copy over palette to color ram
         for (i = 0; i < (size * 4); i+=4)
            *((volatile u32 *)(VDP2_CRAM+i)) = *((u32 *)(palette+i));            
         break;
      default:
         return IAPETUS_ERR_INVALIDARG;
   }

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_set_default_palette(void)
{
   return vdp_set_palette(CRM5_2048, defaultpalette, 256);
}

//////////////////////////////////////////////////////////////////////////////

