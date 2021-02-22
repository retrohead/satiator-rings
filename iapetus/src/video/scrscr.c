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

extern vdp2_settings_struct vdp2_settings;

//////////////////////////////////////////////////////////////////////////////

void calc_character_pattern_cycles_and_addr(int screen_num, u8 color, u8 map_offset)
{  
   switch(color)
   {
      case 0: // 16 color(1 cycle)
         vdp2_settings.char_pattern_cycles[screen_num] = 1;
         break;
      case 1: // 256 color
         vdp2_settings.char_pattern_cycles[screen_num] = 2;
         break;
      case 2: // 2048 colors
         vdp2_settings.char_pattern_cycles[screen_num] = 4;
         break;
      case 3: // 32,768 colors
         vdp2_settings.char_pattern_cycles[screen_num] = 4;
         break;
      case 4: // 16.7 Million colors
         vdp2_settings.char_pattern_cycles[screen_num] = 8;
         break;
      default:
         vdp2_settings.char_pattern_cycles[screen_num] = 99;
         break;
   }

   vdp2_settings.char_pattern_addr[screen_num] = vdp2_settings.MPOFN.part.n0mp * 0x20000; // fix me
}

//////////////////////////////////////////////////////////////////////////////

void calc_pattern_name_cycles_and_addr(int screen_num, int is_bitmap)
{
   vdp2_settings.pattern_name_cycles[screen_num] = is_bitmap ^= 1;
}

//////////////////////////////////////////////////////////////////////////////

int SetupRamCycles()
{
   //int cycles[4]= { 0, 0, 0, 0 };
   int avail_cycles;
   //char zmtbl[4] = { 1, 2, 4, 4}; // 1, 1/2, 1/4, 1/4
   //char coltbl[8] = { 1, 2, 4, 4, 8, 0, 0, 0 }; //  16 color, 256 color, 2048 color, 32768 color, 16.7M color

   // Figure out how many available cycles we have
   avail_cycles = 16;

//   if (VDP2_REG_RAMCTL & 0x100)
//      availcycles += 8;
//   if (VDP2_REG_RAMCTL & 0x200)
//      availcycles += 8;

   // Let's first make sure we have enough cycles for everything
   if (vdp2_settings.BGON.part.nbg0_enab)
   {
      // color settings
//      cycles += coltbl[(vdp2settings.CHCTLA.part.n0chcn >> 4) & 0x7];

      // Is zoom being used?
//      cycles = cycles * zmtbl[vdp2settings.ZMCTL.part.n0zm];

      // Is it tiled or bitmap?

      // which bank is being used?

      // (for tile)
      // pattern name table access requires at least one cycle(2 for when 1/2
      // reduction is used, 4 for when 1/4 reduction is used)
      // character pattern data has the same requirements as the bitmap
      // pattern data

      // Is VC Scroll or Line Scroll enabled?
   }
   if (vdp2_settings.BGON.part.nbg1_enab)
   {
   }
   if (vdp2_settings.BGON.part.nbg2_enab)
   {
   }
   if (vdp2_settings.BGON.part.nbg3_enab)
   {
   }

   // Ok, we have enough, now let's make sure everything fits right
//      VDP2_REG_CYCA0L = (VDP2_REG_CYCA0L & 0xFF) | 0x44; // NBG0 Bitmap Read, NBG0 Bitmap Read // fix me
//      VDP2_REG_CYCA0U = 0xFEEF; // No Acccess, CPU Read/Write, CPU Read/Write, No Access
   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_nbg0_init(screen_settings_struct *settings)
{
   // First make sure we're not not going to run into any 
   // problems with other enabled screens
   if (vdp2_settings.BGON.part.rbg1_enab)
      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.nbg0_enab = 1;

   // Set NBG0 to sane values

   // Screen Scroll Value Registers: No scroll
   VDP2_REG_SCXIN0 = 0;
   VDP2_REG_SCXDN0 = 0;
   VDP2_REG_SCYIN0 = 0;
   VDP2_REG_SCYDN0 = 0;

   // Coordinate Increment Registers: 1.0/1.0
   VDP2_REG_ZMXIN0 = 1;
   VDP2_REG_ZMXDN0 = 0;
   VDP2_REG_ZMYIN0 = 1;
   VDP2_REG_ZMYDN0 = 0;

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_NBG0, 1);
      
   vdp2_settings.CHCTLA.all &= 0xFF00;
   vdp2_settings.CHCTLA.part.n0chcn = settings->color & 0x7;

   // If Bitmap, set bitmap settings
   if (settings->is_bitmap)
   {
      // Bitmap Enabled
      vdp2_settings.CHCTLA.all |= (settings->bitmap_size << 1) | 0x2;

      // Special/Extra settings
      vdp2_settings.BMPNA.part.n0bmpr = settings->special_priority & 0x1;
      vdp2_settings.BMPNA.part.n0bmcc = settings->special_color_calc & 0x1;
      vdp2_settings.BMPNA.part.n0bmp = settings->extra_palette_num & 0x7;
      VDP2_REG_BMPNA = vdp2_settings.BMPNA.all;
   }
   else
   {
      // Tile Enabled
      vdp2_settings.CHCTLA.part.n0chsz |= settings->char_size & 0x1;
      if (settings->pattern_name_size & 0x1)
      {
         // 1 Word
         vdp2_settings.PNCN0.part.n0pnb = 1;
         vdp2_settings.PNCN0.part.n0cnsm = settings->flip_function;
         vdp2_settings.PNCN0.part.n0spr = settings->special_priority;
         vdp2_settings.PNCN0.part.n0scc = settings->special_color_calc;
         vdp2_settings.PNCN0.part.n0splt = settings->extra_palette_num;
         vdp2_settings.PNCN0.part.n0scn = settings->extra_char_num;
      }
      else
         // 2 Words
         vdp2_settings.PNCN0.part.n0pnb = 0;

      // Pattern Name Control Register
      VDP2_REG_PNCN0 = vdp2_settings.PNCN0.all;

      vdp2_settings.PLSZ.part.n0plsz = settings->plane_size & 0x3;
      VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;
      vdp2_settings.MPABN0.part.n0mpa = settings->map[0];
      vdp2_settings.MPABN0.part.n0mpb = settings->map[1];
      vdp2_settings.MPCDN0.part.n0mpc = settings->map[2];
      vdp2_settings.MPCDN0.part.n0mpd = settings->map[3];
      VDP2_REG_MPABN0 = vdp2_settings.MPABN0.all;
      VDP2_REG_MPCDN0 = vdp2_settings.MPCDN0.all;
   }

//   CalcCharacterPatternCyclesAndAddr(SCREEN_NBG0, settings->color);
//   CalcPatternNameCyclesAndAddr(SCREEN_NBG0, settings->isbitmap);

   // Adjust VRAM access
//   if (SetupRamCycles() != IAPETUS_ERR_OK)
//      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.nbg0_trans_enab = settings->transparent_bit;
   vdp2_settings.MPOFN.part.n0mp = settings->map_offset;

   // Map offset Register
   VDP2_REG_MPOFN = vdp2_settings.MPOFN.all;

   // Character Control Register
   VDP2_REG_CHCTLA = vdp2_settings.CHCTLA.all;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;

   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_nbg0_deinit(void)
{
   vdp2_settings.BGON.part.nbg0_enab = 0;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_nbg1_init(screen_settings_struct *settings)
{
   // First make sure we're not not going to run into any 
   // problems with other enabled screens
   if (vdp2_settings.exbgenab)
      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.nbg1_enab = 1;

   // Set NBG1 to sane values

   // Screen Scroll Value Registers: No scroll
   VDP2_REG_SCXIN1 = 0;
   VDP2_REG_SCXDN1 = 0;
   VDP2_REG_SCYIN1 = 0;
   VDP2_REG_SCYDN1 = 0;

   // Coordinate Increment Registers: 1.0/1.0
   VDP2_REG_ZMXIN1 = 1;
   VDP2_REG_ZMXDN1 = 0;
   VDP2_REG_ZMYIN1 = 1;
   VDP2_REG_ZMYDN1 = 0;

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_NBG1, 1);

   vdp2_settings.CHCTLA.all &= 0x00FF;
   vdp2_settings.CHCTLA.part.n1chcn = settings->color & 0x3;

   // If Bitmap, set bitmap settings
   if (settings->is_bitmap)
   {
      // Bitmap Enabled
      vdp2_settings.CHCTLA.all |= (settings->bitmap_size << 9) | 0x200;

      // Special/Extra settings
      vdp2_settings.BMPNA.part.n1bmpr = settings->special_priority & 0x1;
      vdp2_settings.BMPNA.part.n1bmcc = settings->special_color_calc & 0x1;
      vdp2_settings.BMPNA.part.n1bmp = settings->extra_palette_num & 0x7;
      VDP2_REG_BMPNA = vdp2_settings.BMPNA.all;
   }
   else
   {
      // Tile Enabled
      vdp2_settings.CHCTLA.part.n1chsz |= settings->char_size & 0x1;

      if (settings->pattern_name_size & 0x1)
      {
         // 1 Word
         vdp2_settings.PNCN1.part.n1pnb = 1;
         vdp2_settings.PNCN1.part.n1cnsm = settings->flip_function;
         vdp2_settings.PNCN1.part.n1spr = settings->special_priority;
         vdp2_settings.PNCN1.part.n1scc = settings->special_color_calc;
         vdp2_settings.PNCN1.part.n1splt = settings->extra_palette_num;
         vdp2_settings.PNCN1.part.n1scn = settings->extra_char_num;
      }
      else
         // 2 Words
         vdp2_settings.PNCN1.part.n1pnb = 0;

      // Pattern Name Control Register
      VDP2_REG_PNCN1 = vdp2_settings.PNCN1.all;

      vdp2_settings.PLSZ.part.n1plsz = settings->plane_size & 0x3;
      VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;
      vdp2_settings.MPABN1.part.n1mpa = settings->map[0];
      vdp2_settings.MPABN1.part.n1mpb = settings->map[1];
      vdp2_settings.MPCDN1.part.n1mpc = settings->map[2];
      vdp2_settings.MPCDN1.part.n1mpd = settings->map[3];
      VDP2_REG_MPABN1 = vdp2_settings.MPABN1.all;
      VDP2_REG_MPCDN1 = vdp2_settings.MPCDN1.all;
   }

   // Adjust VRAM access
   if (SetupRamCycles() != IAPETUS_ERR_OK)
      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.nbg1_trans_enab = settings->transparent_bit;
   vdp2_settings.MPOFN.part.n1mp = settings->map_offset;

   // Map offset Register
   VDP2_REG_MPOFN = vdp2_settings.MPOFN.all;

   // Character Control Register
   VDP2_REG_CHCTLA = vdp2_settings.CHCTLA.all;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;

   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_nbg1_deinit(void)
{
   vdp2_settings.BGON.part.nbg1_enab = 0;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_nbg2_init(screen_settings_struct *settings)
{
   vdp2_settings.BGON.part.nbg2_enab = 1;

   // Set NBG2 to sane values

   // Screen Scroll Value Registers: No scroll
   VDP2_REG_SCXN2 = 0;
   VDP2_REG_SCYN2 = 0;

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_NBG2, 1);

   vdp2_settings.CHCTLB.all &= 0xFFFC;
   vdp2_settings.CHCTLB.part.n2chcn = settings->color & 0x1;

   // Setup Tile settings
   vdp2_settings.CHCTLB.part.n2chsz |= settings->char_size & 0x1;

   if (settings->pattern_name_size & 0x1)
   {
      // 1 Word
      vdp2_settings.PNCN2.part.n2pnb = 1;
      vdp2_settings.PNCN2.part.n2cnsm = settings->flip_function;
      vdp2_settings.PNCN2.part.n2spr = settings->special_priority;
      vdp2_settings.PNCN2.part.n2scc = settings->special_color_calc;
      vdp2_settings.PNCN2.part.n2splt = settings->extra_palette_num;
      vdp2_settings.PNCN2.part.n2scn = settings->extra_char_num;
   }
   else
      // 2 Words
      vdp2_settings.PNCN2.part.n2pnb = 0;

   // Pattern Name Control Register
   VDP2_REG_PNCN2 = vdp2_settings.PNCN2.all;

   vdp2_settings.PLSZ.part.n2plsz = settings->plane_size & 0x3;
   VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;
   vdp2_settings.MPABN2.part.n2mpa = settings->map[0];
   vdp2_settings.MPABN2.part.n2mpb = settings->map[1];
   vdp2_settings.MPCDN2.part.n2mpc = settings->map[2];
   vdp2_settings.MPCDN2.part.n2mpd = settings->map[3];
   VDP2_REG_MPABN2 = vdp2_settings.MPABN2.all;
   VDP2_REG_MPCDN2 = vdp2_settings.MPCDN2.all;

   // Adjust VRAM access
   if (SetupRamCycles() != IAPETUS_ERR_OK)
      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.nbg2_trans_enab = settings->transparent_bit;
   vdp2_settings.MPOFN.part.n2mp = settings->map_offset;

   // Map offset Register
   VDP2_REG_MPOFN = vdp2_settings.MPOFN.all;

   // Character Control Register
   VDP2_REG_CHCTLB = vdp2_settings.CHCTLB.all;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;

   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_nbg2_deinit(void)
{
   vdp2_settings.BGON.part.nbg2_enab = 0;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_nbg3_init(screen_settings_struct *settings)
{
   vdp2_settings.BGON.part.nbg3_enab = 1;

   // Set NBG3 to sane values

   // Screen Scroll Value Registers: No scroll
   VDP2_REG_SCXN3 = 0;
   VDP2_REG_SCYN3 = 0;

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_NBG3, 1);

   vdp2_settings.CHCTLB.all &= 0xFFCF;
   vdp2_settings.CHCTLB.part.n3chcn = settings->color & 0x1;

   // Setup Tile settings
   vdp2_settings.CHCTLB.part.n3chsz |= settings->char_size & 0x1;

   if (settings->pattern_name_size & 0x1)
   {
      // 1 Word
      vdp2_settings.PNCN3.part.n3pnb = 1;
      vdp2_settings.PNCN3.part.n3cnsm = settings->flip_function;
      vdp2_settings.PNCN3.part.n3spr = settings->special_priority;
      vdp2_settings.PNCN3.part.n3scc = settings->special_color_calc;
      vdp2_settings.PNCN3.part.n3splt = settings->extra_palette_num;
      vdp2_settings.PNCN3.part.n3scn = settings->extra_char_num;
   }
   else
      // 2 Words
      vdp2_settings.PNCN3.part.n3pnb = 0;

   // Pattern Name Control Register
   VDP2_REG_PNCN3 = vdp2_settings.PNCN3.all;

   vdp2_settings.PLSZ.part.n3plsz = settings->plane_size & 0x3;
   VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;
   vdp2_settings.MPABN3.part.n3mpa = settings->map[0];
   vdp2_settings.MPABN3.part.n3mpb = settings->map[1];
   vdp2_settings.MPCDN3.part.n3mpc = settings->map[2];
   vdp2_settings.MPCDN3.part.n3mpd = settings->map[3];
   VDP2_REG_MPABN3 = vdp2_settings.MPABN3.all;
   VDP2_REG_MPCDN3 = vdp2_settings.MPCDN3.all;

   // Adjust VRAM access
   if (SetupRamCycles() != IAPETUS_ERR_OK)
      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.nbg3_trans_enab = settings->transparent_bit;
   vdp2_settings.MPOFN.part.n3mp = settings->map_offset;

   // Map offset Register
   VDP2_REG_MPOFN = vdp2_settings.MPOFN.all;

   // Character Control Register
   VDP2_REG_CHCTLB = vdp2_settings.CHCTLB.all;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;

   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_nbg3_deinit(void)
{
   vdp2_settings.BGON.part.nbg3_enab = 0;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_exbg_init(screen_settings_struct *settings)
{
   vdp2_settings.BGON.part.nbg1_enab = 1;

   // Set EXBG to sane values

   // Screen Scroll Value Registers: No scroll
   VDP2_REG_SCXIN1 = 0;
   VDP2_REG_SCXDN1 = 0;
   VDP2_REG_SCYIN1 = 0;
   VDP2_REG_SCYDN1 = 0;

   // Coordinate Increment Registers: 1.0/1.0
   VDP2_REG_ZMXIN1 = 1;
   VDP2_REG_ZMXDN1 = 0;
   VDP2_REG_ZMYIN1 = 1;
   VDP2_REG_ZMYDN1 = 0;

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_EXBG, 1);

   vdp2_settings.CHCTLA.all &= 0x00FF;
   vdp2_settings.CHCTLA.part.n1chcn = settings->color & 0x3;

   // If Bitmap, set bitmap settings
   if (settings->is_bitmap)
   {
      // Bitmap Enabled
      vdp2_settings.CHCTLA.all |= (settings->bitmap_size << 9) | 0x200;

      // Special/Extra settings
      vdp2_settings.BMPNA.part.n1bmpr = settings->special_priority & 0x1;
      vdp2_settings.BMPNA.part.n1bmcc = settings->special_color_calc & 0x1;
      vdp2_settings.BMPNA.part.n1bmp = settings->extra_palette_num & 0x7;
      VDP2_REG_BMPNA = vdp2_settings.BMPNA.all;
   }
   else
   {
      // Tile Enabled
      vdp2_settings.CHCTLA.part.n1chsz |= settings->char_size & 0x1;

      if (settings->pattern_name_size & 0x1)
      {
         // 1 Word
         vdp2_settings.PNCN1.part.n1pnb = 1;
         vdp2_settings.PNCN1.part.n1cnsm = settings->flip_function;
         vdp2_settings.PNCN1.part.n1spr = settings->special_priority;
         vdp2_settings.PNCN1.part.n1scc = settings->special_color_calc;
         vdp2_settings.PNCN1.part.n1splt = settings->extra_palette_num;
         vdp2_settings.PNCN1.part.n1scn = settings->extra_char_num;
      }
      else
         // 2 Words
         vdp2_settings.PNCN1.part.n1pnb = 0;

      // Pattern Name Control Register
      VDP2_REG_PNCN1 = vdp2_settings.PNCN1.all;

      vdp2_settings.PLSZ.part.n1plsz = settings->plane_size & 0x3;
      VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;
      vdp2_settings.MPABN1.part.n1mpa = settings->map[0];
      vdp2_settings.MPABN1.part.n1mpb = settings->map[1];
      vdp2_settings.MPCDN1.part.n1mpc = settings->map[2];
      vdp2_settings.MPCDN1.part.n1mpd = settings->map[3];
      VDP2_REG_MPABN1 = vdp2_settings.MPABN1.all;
      VDP2_REG_MPCDN1 = vdp2_settings.MPCDN1.all;
   }

   vdp2_settings.BGON.part.nbg1_trans_enab = settings->transparent_bit;
   vdp2_settings.MPOFN.part.n1mp = settings->map_offset;

   // Map offset Register
   VDP2_REG_MPOFN = vdp2_settings.MPOFN.all;

   // Character Control Register
   VDP2_REG_CHCTLA = vdp2_settings.CHCTLA.all;
 
   VDP2_REG_EXTEN = 0x0001;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_exbg_deinit(void)
{
   vdp2_settings.BGON.part.nbg1_enab = 0;
   vdp2_settings.exbgenab = 1;
   VDP2_REG_EXTEN = 0x0000;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////
