/*  Copyright 2005-2007 Theo Berkau

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

//////////////////////////////////////////////////////////////////////////////

void interrupt_set_level_mask(u32 imask)
{
   u32 sr;

   asm("stc sr,%0": "=r"(sr));
   sr &= 0xFFFFFF0F;
   imask <<= 4;
   sr |= imask;
   asm("ldc %0,sr": : "r" (sr));
}

//////////////////////////////////////////////////////////////////////////////

u32 interrupt_get_level_mask(void)
{
   u32 sr;

   asm("stc sr,%0": "=r"(sr));
   sr = (sr & 0xF0) >> 4;

   return sr;
}

//////////////////////////////////////////////////////////////////////////////

