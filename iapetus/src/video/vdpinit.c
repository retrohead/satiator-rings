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

#include "../iapetus.h"

//////////////////////////////////////////////////////////////////////////////

extern volatile int commlink_service;
extern void (*commlink_func)(u8);

//////////////////////////////////////////////////////////////////////////////

volatile u16 *vdp1_ram = (volatile u16 *)VDP1_RAM;
volatile u16 *vdp2_ram = (volatile u16 *)VDP2_RAM;
volatile u16 *vdp2_cram = (volatile u16 *)VDP2_CRAM;

//////////////////////////////////////////////////////////////////////////////
// Internal variables

vdp2_settings_struct vdp2_settings;

//static u16 cmdlistnum = 1;

//////////////////////////////////////////////////////////////////////////////

void vdp_init(int res)
{
   int i;
   u16 EWRR;

   if (res & 0x1)
      bios_set_clock_speed(1); // 352
   else
      bios_set_clock_speed(0); // 320

   for (i = 0; i < sizeof(vdp2_settings); i++)
      *(((u8 *)&vdp2_settings)+i) = 0;

   switch(res & 0x3)
   {
      case 0:
         vdp2_settings.screen_width = 320;
         break;
      case 1:
         vdp2_settings.screen_width = 352;
         break;
      case 2:
         vdp2_settings.screen_width = 640;
         break;
      case 3:
         vdp2_settings.screen_width = 704;
         break;
      default: break;
   }

   switch((res >> 4) & 0x3)
   {
      case 0:
         vdp2_settings.screen_height = 224;
         break;
      case 1:
         vdp2_settings.screen_height = 240;
         break;
      case 2:
         vdp2_settings.screen_height = 256;
         break;
      default: break;
   }

   if (res & 0xC0)
      vdp2_settings.screen_height <<= 1;   

   // Clear registers
   for (i = 0; i < 0x200; i+=2)
      *((volatile u16 *)(0x25F80000+i)) = 0x0000;

   // Make sure VDP2 is in a sane state
   VDP2_REG_TVMD = (u16)res;

   // Make sure VDP1 is in a sane state
   VDP1_REG_PTMR = 0;

   // Clear VDP1 Ram
   for (i = 0; i < 0x80000; i+=2)
      *((volatile u16 *)(VDP1_RAM+i)) = 0x0000;

   // Set end code in first entry
   *((volatile u16 *)(VDP1_RAM)) = 0x8000;

   // Set Sprite priorities
   VDP2_REG_PRISA = 0x0707;
   VDP2_REG_PRISB = 0x0707;
   VDP2_REG_PRISC = 0x0707;
   VDP2_REG_PRISD = 0x0707;

   // Enable VDP1
   VDP1_REG_FBCR = 0x0000;
   VDP1_REG_TVMR = 0x0000;
   VDP1_REG_EWDR = 0x0000;
   VDP1_REG_EWLR = (0 << 9) | 0;

   if (vdp2_settings.screen_width <= 512)
      EWRR = (vdp2_settings.screen_width << 9);
   else
      EWRR = (1 << 9);

   if (vdp2_settings.screen_height <= 256)
      EWRR |= vdp2_settings.screen_height;
   else
      EWRR |= 256;

   VDP1_REG_EWRR = EWRR;

   // Setup Sprite data to be both RGB and palette
   VDP2_REG_SPCTL = 0x0020;
   VDP1_REG_PTMR = 0x0002;

   // Clear VDP2 Ram
   for (i = 0; i < 0x80000; i+=2)
      *((volatile u16 *)(VDP2_RAM+i)) = 0x0000;

   // Clear Color Ram
   for (i = 0; i < 0x1000; i+=2)
      *((volatile u16 *)(VDP2_CRAM+i)) = 0x0000;

   // Set Background Color
   VDP2_REG_BKTA = 0x7FFFE >> 1; // Single Color, use 0x05E7FFFE for color

   // Set Default Ram Control
   VDP2_REG_RAMCTL = 0x0000;

   // Set Default Priorities
   VDP2_REG_PRINA = 0x0101;
   VDP2_REG_PRINB = 0x0101;
   VDP2_REG_PRIR = 0x0101;

   // Set default VRAM access
   VDP2_REG_CYCA0L = 0xFFFF; // No Access, No Access, No Access, No Access 
   VDP2_REG_CYCA0U = 0xFFFF; // No Access, No Access, No Access, No Access 
   VDP2_REG_CYCA1L = 0xFFFF; // No Access, No Access, No Access, No Access
   VDP2_REG_CYCA1U = 0xFFFF; // No Access, No Access, No Access, No Access
   VDP2_REG_CYCB0L = 0xFFFF; // No Access, No Access, No Access, No Access
   VDP2_REG_CYCB0U = 0xFFFF; // No Access, No Access, No Access, No Access
   VDP2_REG_CYCB1L = 0xFFFF; // No Access, No Access, No Access, No Access
   VDP2_REG_CYCB1U = 0xFFFF; // No Access, No Access, No Access, No Access
}

//////////////////////////////////////////////////////////////////////////////

void vdp_set_priority(int screen, u8 priority)
{
   switch(screen)
   {
      case SCREEN_NBG0: // NBG0/RBG1
      case SCREEN_RBG1:
      {
         vdp2_settings.PRINA.part.nbg0_priority = priority;
         VDP2_REG_PRINA = vdp2_settings.PRINA.all;
         break;
      }
      case SCREEN_NBG1: // NBG1/EXBG
      case SCREEN_EXBG:
      {         
         vdp2_settings.PRINA.part.nbg1_priority = priority;
         VDP2_REG_PRINA = vdp2_settings.PRINA.all;
         break;
      }
      case SCREEN_NBG2: // NBG2
      {         
         vdp2_settings.PRINB.part.nbg2_priority = priority;
         VDP2_REG_PRINB = vdp2_settings.PRINB.all;
         break;
      }
      case SCREEN_NBG3: // NBG3
      {         
         vdp2_settings.PRINB.part.nbg3_priority = priority;
         VDP2_REG_PRINB = vdp2_settings.PRINB.all;
         break;
      }
      case SCREEN_RBG0: // RBG0
      {         
         vdp2_settings.PRIR.all = priority;
         VDP2_REG_PRIR = vdp2_settings.PRIR.all;
         break;
      }
      default: break;
   }
}

//////////////////////////////////////////////////////////////////////////////

void vdp_wait_hblankin(void)
{
	while(!(VDP2_REG_TVSTAT & 4)) {}
}

//////////////////////////////////////////////////////////////////////////////

void vdp_wait_hblankout(void)
{
	while(VDP2_REG_TVSTAT & 4) {}
}

//////////////////////////////////////////////////////////////////////////////

void vdp_wait_vblankin(void)
{
	while(!(VDP2_REG_TVSTAT & 8)) {}
}

//////////////////////////////////////////////////////////////////////////////

void vdp_wait_vblankout(void)
{
	while(VDP2_REG_TVSTAT & 8) {}
}

//////////////////////////////////////////////////////////////////////////////

void vdp_vsync(void)
{
   // Wait for Vblank-in
   while(!(VDP2_REG_TVSTAT & 8)) {
      if (commlink_service)
         commlink_func(0x01);
   }

   // Wait for Vblank-out
	vdp_wait_vblankout();
}

//////////////////////////////////////////////////////////////////////////////

void vdp_disp_on(void)
{
   VDP2_REG_TVMD |= 0x8000;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_disp_off(void)
{
   VDP2_REG_TVMD &= ~0x8000;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_set_color_offset(u8 num, s16 r, s16 g, s16 b)
{
   if (num == 0)
   {
      // Offset A
      VDP2_REG_COAR = r & 0x1FF;
      VDP2_REG_COAG = g & 0x1FF;
      VDP2_REG_COAB = b & 0x1FF;
   }          
   else
   {
      // Offset B
      VDP2_REG_COBR = r & 0x1FF;
      VDP2_REG_COBG = g & 0x1FF;
      VDP2_REG_COBB = b & 0x1FF;
   }
}

//////////////////////////////////////////////////////////////////////////////

void vdp_enable_color_offset(u16 screen, int select)
{
   // Adjust select first
   screen = 1 << screen;

   if (select == 0)
      vdp2_settings.CLOFSL &= ~screen;
   else
      vdp2_settings.CLOFSL |= screen;

   // Adjust enable
   vdp2_settings.CLOFEN |= screen;

   // Write the new values to registers
   VDP2_REG_CLOFSL = vdp2_settings.CLOFSL;
   VDP2_REG_CLOFEN = vdp2_settings.CLOFEN;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_disable_color_offset(u16 screen)
{
   vdp2_settings.CLOFEN &= ~screen;
   VDP2_REG_CLOFEN = vdp2_settings.CLOFEN;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_get_scr_width_height(int *width, int *height)
{	
	*width=vdp2_settings.screen_width;
	*height=vdp2_settings.screen_height;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_enable_line_window(int screennum, int windownum, u16 mode, u32 linetbladdr)
{
}

//////////////////////////////////////////////////////////////////////////////

void vdp_disable_line_window(void)
{
}

//////////////////////////////////////////////////////////////////////////////
