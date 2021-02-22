/*  Copyright 2006-2007 Theo Berkau

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

void sound_init(void)
{
   int i;

   // Turn off Sound CPU
   smpc_issue_command(0x07);

   // Make sure SCSP is set to 512k mode
   *(volatile u8 *)(0x25B00400) = 0x02;

   // Clear Sound Ram
   for (i = 0; i < 0x80000; i+=4)
      *(volatile u32 *)(0x25A00000 + i) = 0x00000000;

   // Clear MIDI input
   i = SCSP_REG_MIBUF;
   i = SCSP_REG_MIBUF;
   i = SCSP_REG_MIBUF;
   i = SCSP_REG_MIBUF;

   // Clear DSP micro program
   for (i = 0; i < 0x400; i+=4)
      *(volatile u16 *)(0x25B00800 + i) = 0;

   // Clear DSP memory address
   for (i = 0; i < 0x80; i+=4)
      *(volatile u16 *)(0x25B00780 + i) = 0;

   // Clear DSP coefficient table
   for (i = 0; i < 0x80; i+=4)
      *(volatile u16 *)(0x25B00700 + i) = 0;

   // Clear DSP temporary work area
   for (i = 0; i < 0x200; i+=4)
      *(volatile u16 *)(0x25B00C00 + i) = 0;

   // Clear all slots
   for (i = 0; i < 0x400; i+=4)
      *(volatile u16 *)(0x25B00000 + i) = 0;

   // Key off all slots
   sound_key_off_all();

   // Set TL to max for all slots
   for (i = 0; i < 32; i++)
      *(volatile u8 *)(0x25B00000 + (i * 32) + 0x0D) = 0xFF;

   // Set Master Volume to max
   SCSP_REG_MVOL = 0xF;
}

//////////////////////////////////////////////////////////////////////////////

void sound_load_driver(u8 *data, u32 size)
{
   int i;

   // Turn off Sound CPU
   smpc_issue_command(0x07);

   // Make sure SCSP is set to 512k mode
   *(volatile u8 *)(0x25B00400) = 0x02;

   // Clear Sound Ram
   for (i = 0; i < 0x80000; i+=4)
      *(volatile u32 *)(0x25A00000 + i) = 0x00000000;

   // Copy driver over
   for (i = 0; i < size; i++)
      *(volatile u8 *)(0x25A00000 + i) = data[i];

   // Turn on Sound CPU again
   smpc_issue_command(0x06);
}

//////////////////////////////////////////////////////////////////////////////

void sound_external_audio_enable(u8 vol_l, u8 vol_r)
{
   volatile u16 *slot_ptr;

   vol_l &= 0x7;
   vol_r &= 0x7;

   // Setup SCSP Slot 16 and Slot 17 for playing
   slot_ptr = (volatile u16 *)(0x25B00000 + (0x20 * 16));
   slot_ptr[0] = 0x1000;
   slot_ptr[1] = 0x0000; 
   slot_ptr[2] = 0x0000; 
   slot_ptr[3] = 0x0000; 
   slot_ptr[4] = 0x0000; 
   slot_ptr[5] = 0x0000; 
   slot_ptr[6] = 0x00FF; 
   slot_ptr[7] = 0x0000; 
   slot_ptr[8] = 0x0000; 
   slot_ptr[9] = 0x0000; 
   slot_ptr[10] = 0x0000; 
   slot_ptr[11] = 0x001F | (vol_l << 5);
   slot_ptr[12] = 0x0000; 
   slot_ptr[13] = 0x0000; 
   slot_ptr[14] = 0x0000; 
   slot_ptr[15] = 0x0000; 

   slot_ptr = (volatile u16 *)(0x25B00000 + (0x20 * 17));
   slot_ptr[0] = 0x1000;
   slot_ptr[1] = 0x0000; 
   slot_ptr[2] = 0x0000; 
   slot_ptr[3] = 0x0000; 
   slot_ptr[4] = 0x0000; 
   slot_ptr[5] = 0x0000; 
   slot_ptr[6] = 0x00FF; 
   slot_ptr[7] = 0x0000; 
   slot_ptr[8] = 0x0000; 
   slot_ptr[9] = 0x0000; 
   slot_ptr[10] = 0x0000; 
   slot_ptr[11] = 0x000F | (vol_r << 5);
   slot_ptr[12] = 0x0000; 
   slot_ptr[13] = 0x0000; 
   slot_ptr[14] = 0x0000; 
   slot_ptr[15] = 0x0000;

   *((volatile u16 *)(0x25B00400)) = 0x020F;
}

//////////////////////////////////////////////////////////////////////////////

