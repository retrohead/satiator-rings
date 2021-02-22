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

volatile u32 timercounter;
u32 timerfreq;

void timer_rtc_increment(void) __attribute__ ((interrupt_handler));
void timer_frt_increment(void) __attribute__ ((interrupt_handler));
void timer_wdt_increment(void) __attribute__ ((interrupt_handler));

extern vdp2_settings_struct vdp2_settings;

//////////////////////////////////////////////////////////////////////////////

void timer_rtc_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void timer_frt_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void timer_wdt_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

void timer_hblank_increment(void)
{
   timercounter++;
}

//////////////////////////////////////////////////////////////////////////////

int timer_setup(int type, u32 *freq)
{
   u32 clock=0;
   u32 old_level_mask = interrupt_get_level_mask();

   if (freq == NULL)
      return IAPETUS_ERR_INVALIDPOINTER;

   freq[0] = 0;

   interrupt_set_level_mask(0xF);

   if (bios_get_clock_speed() == 0)
      clock = 26846587;
   else
      clock = 28636360;

   switch (type)
   {
      case TIMER_RTC:
         // Disable RTC
         SH2_REG_RTCSR_W(0);
         // Setup Interrupt
         bios_set_sh2_interrupt(0x7F, timer_rtc_increment);
         // Setup vector
         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
         SH2_REG_IPRA = (0xF << 4);
         freq[0] = clock / 4;
         // Enable RTC
         SH2_REG_RTCNT_W(0);
         SH2_REG_RTCOR_W(0xFF);
         SH2_REG_RTCSR_W(0x40 | (0x7 << 3)); // I may change the increment speed
         break;
//      case TIMER_FRT:
         // Disable FRT

         // Setup so that FRC is cleared on compare match A
//         SH2_REG_FTCSR = 1; 
         // Setup Interrupt
//         bios_set_sh2_interrupt(0x7F, TimerFRTIncrement);
         // Setup vector
//         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
//         SH2_REG_IPRA = (0xF << 4);
//         freq[0] = clock / 4;
         // Enable FRT
//         SH2_REG_RTCNT = 0;
         // Setup the internal clock;
//         SH2_REG_TCR = 0; 
//         SH2_REG_FRC = 0;
//         SH2_REG_TIER = ???;
//         break;
      case TIMER_WDT:
         // Disable WDT interval timer
         SH2_REG_WTCSR_W(SH2_REG_WTCSR_R & 0x18);
         // Setup Interrupt
         bios_set_sh2_interrupt(0x7F, timer_wdt_increment);
         // Setup vector
         SH2_REG_VCRWDT = 0x7F7F;
         // Setup level
         SH2_REG_IPRA = (0xF << 4);
         freq[0] = clock / 2 / 256; // double check this
         // Enable WDT interval timer
         SH2_REG_WTCNT_W(0);
         SH2_REG_WTCSR_W((SH2_REG_WTCSR_R & 0xDF) | 0x20 | 7);
         break;
      case TIMER_HBLANK:
         // Setup Interrupt
         bios_set_scu_interrupt(0x42, timer_hblank_increment);
         bios_change_scu_interrupt_mask(~MASK_HBLANKIN, 0);
         freq[0] = vdp2_settings.screen_height * ((VDP2_REG_TVSTAT & 1) == 0 ? 60 : 50);
        
         if (old_level_mask > 0xC)
            old_level_mask = 0xC;

         break;
      default:
         return IAPETUS_ERR_INVALIDARG;
   }

   timercounter = 0;
   if (old_level_mask == 0xF)
      old_level_mask = 0xE;

   interrupt_set_level_mask(old_level_mask);

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

u32 timer_counter()
{
   return timercounter;
}

//////////////////////////////////////////////////////////////////////////////

void timer_stop()
{
   bios_set_sh2_interrupt(0x7F, 0);  
}

//////////////////////////////////////////////////////////////////////////////

void timer_delay(u32 freq, int ms)
{
   u32 start_time = timer_counter();
   u32 max_time = start_time + ((ms+1) * freq / 1000);

   // Wait until time has elapsed
   while (timer_counter() < max_time) {}
}

//////////////////////////////////////////////////////////////////////////////
