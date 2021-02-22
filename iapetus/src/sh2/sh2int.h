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

#ifndef INT_H
#define INT_H

enum INTMASK
{
   MASK_VBLANKIN = 0x00000001,
   MASK_VBLANKOUT = 0x00000002,
   MASK_HBLANKIN = 0x00000004,
   MASK_TIMER0 = 0x00000008,
   MASK_TIMER1 = 0x00000010,
   MASK_DSPEND = 0x00000020,
   MASK_SNDREQUEST = 0x00000040,
   MASK_SYSTEMMANAGER = 0x00000080,
   MASK_PAD = 0x00000100,
   MASK_DMA2 = 0x00000200,
   MASK_DMA1 = 0x00000400,
   MASK_DMA0 = 0x00000800,
   MASK_DMAILLEGAL = 0x00001000,
   MASK_DRAWEND = 0x00002000
};

void interrupt_set_level_mask(u32 imask);
u32 interrupt_get_level_mask(void);

#endif
