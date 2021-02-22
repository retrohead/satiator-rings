/*  Copyright 2005-2007,2013 Theo Berkau

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

#include "iapetus.h"

//////////////////////////////////////////////////////////////////////

void init_iapetus(int res)
{
   int i;

   interrupt_set_level_mask(0xF);

   for (i = 0; i < 0x80; i++)
      bios_set_sh2_interrupt(i, 0);

   for (i = 0x40; i < 0x60; i++)
      bios_set_scu_interrupt(i, 0);

   // Make sure all interrupts have been called
   bios_change_scu_interrupt_mask(0, 0);
   bios_change_scu_interrupt_mask(0xFFFFFFFF, 0xFFFFFFFF);

   vdp_init(res);
   per_init();

   commlink_stop_service();
   cl_set_service_func(cl_check2);

#ifdef DEBUG
   debug_init();
#endif

   // If DSP is running, stop it
   if (dsp_is_exec())
       dsp_stop();

   if (interrupt_get_level_mask() > 0x7)
      interrupt_set_level_mask(0x7);
}

//////////////////////////////////////////////////////////////////////
