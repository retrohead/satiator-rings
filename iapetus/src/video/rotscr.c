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

void vdp_set_rotation_table(screen_settings_struct *settings, int num, rottbl_struct *tbl)
{
   volatile u16 *out_tbl=(volatile u16 *)(0x20000000 | settings->parameter_addr | (num * 0x80));
   volatile u16 *in_tbl=(volatile u16 *)tbl;
   int i;
   
   for (i = 0; i < sizeof(rottbl_struct) / 2; i++)
      out_tbl[i] = in_tbl[i];
}

//////////////////////////////////////////////////////////////////////////////

void init_rotation_table(u32 addr)
{
   volatile rottbl_struct *tbl=(volatile rottbl_struct *)(0x20000000 | addr);
   tbl->Xst = 0;
   tbl->Yst = 0;
   tbl->Zst = 0;
   tbl->deltaXst = 0;
   tbl->deltaYst = (1 << 16);
   tbl->deltaX = (1 << 16);
   tbl->deltaY = 0;
   tbl->A = (1 << 16);
   tbl->B = 0;
   tbl->C = 0;
   tbl->D = 0;
   tbl->E = (1 << 16);
   tbl->F = 0;
   tbl->Px = vdp2_settings.screen_width / 2;
   tbl->Py = vdp2_settings.screen_height / 2;
   tbl->Pz = 0; // doesn't seem to matter
   tbl->Cx = vdp2_settings.screen_width / 2;
   tbl->Cy = vdp2_settings.screen_height / 2;
   tbl->Cz = 0;
   tbl->Mx = 0;
   tbl->My = 0;
   tbl->kx = (1 << 16);
   tbl->ky = (1 << 16);
}

//////////////////////////////////////////////////////////////////////////////

static u16 CalcRDBSbit(int bank, int type)
{
   type &= 0x3;

   switch (bank)
   {
      case 0: // A0/A1
         return type;
      case 1:
      {
         if (VDP2_REG_RAMCTL & 0x100)
            return (type << 2);
         else
            return type;
      }
      case 2: // B0/B1
            return (type << 4);
      case 3:
      {
         if (VDP2_REG_RAMCTL & 0x200)
            return (type << 6);
         else
            return (type << 4);
      }
      default:break;
   }

   return 0;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_rbg0_init(screen_settings_struct *settings)
{
   vdp2_settings.BGON.part.rbg0_enab = 1;

   // Normalize rotation settings
   switch (settings->rotation_mode)
   {
      case 0: // Rotation Parameter A
         // Set only rotation parameter A
         // Set Screen-Over to a default value
         vdp2_settings.PLSZ.part.raovr = 0;
         vdp2_settings.MPOFR.part.ramp = settings->map_offset;
         init_rotation_table(settings->parameter_addr);

         if (!settings->is_bitmap)
         {
            vdp2_settings.PLSZ.part.raplsz = settings->plane_size & 0x3;

            // The following could probably be optimized
            vdp2_settings.MPABRA.part.rampa = settings->map[0];
            vdp2_settings.MPABRA.part.rampb = settings->map[1];
            vdp2_settings.MPCDRA.part.rampc = settings->map[2];
            vdp2_settings.MPCDRA.part.rampd = settings->map[3];
            vdp2_settings.MPEFRA.part.rampe = settings->map[4];
            vdp2_settings.MPEFRA.part.rampf = settings->map[5];
            vdp2_settings.MPGHRA.part.rampg = settings->map[6];
            vdp2_settings.MPGHRA.part.ramph = settings->map[7];
            vdp2_settings.MPIJRA.part.rampi = settings->map[8];
            vdp2_settings.MPIJRA.part.rampj = settings->map[9];
            vdp2_settings.MPKLRA.part.rampk = settings->map[10];
            vdp2_settings.MPKLRA.part.rampl = settings->map[11];
            vdp2_settings.MPMNRA.part.rampm = settings->map[12];
            vdp2_settings.MPMNRA.part.rampn = settings->map[13];
            vdp2_settings.MPOPRA.part.rampo = settings->map[14];
            vdp2_settings.MPOPRA.part.rampp = settings->map[15];
            VDP2_REG_MPABRA = vdp2_settings.MPABRA.all;
            VDP2_REG_MPCDRA = vdp2_settings.MPCDRA.all;
            VDP2_REG_MPEFRA = vdp2_settings.MPEFRA.all;
            VDP2_REG_MPGHRA = vdp2_settings.MPGHRA.all;
            VDP2_REG_MPIJRA = vdp2_settings.MPIJRA.all;
            VDP2_REG_MPKLRA = vdp2_settings.MPKLRA.all;
            VDP2_REG_MPMNRA = vdp2_settings.MPMNRA.all;
            VDP2_REG_MPOPRA = vdp2_settings.MPOPRA.all;
         }
         break;
      case 1: // Rotation B
      case 2: // Switched via coefficient table
      case 3: // Switched via rotation parameter window
         // Set both Rotation Parameter A & B
         // Set Screen-Over to a default value
         vdp2_settings.PLSZ.part.raovr = vdp2_settings.PLSZ.part.rbovr = 0;
         vdp2_settings.MPOFR.part.ramp = vdp2_settings.MPOFR.part.rbmp = settings->map_offset;
         init_rotation_table(settings->parameter_addr);
         init_rotation_table(settings->parameter_addr+0x80);

         if (!settings->is_bitmap)
         {
            vdp2_settings.PLSZ.part.raplsz = vdp2_settings.PLSZ.part.rbplsz = settings->plane_size & 0x3;

            // The following could probably be optimized
            vdp2_settings.MPABRA.part.rampa = settings->map[0];
            vdp2_settings.MPABRA.part.rampb = settings->map[1];
            vdp2_settings.MPCDRA.part.rampc = settings->map[2];
            vdp2_settings.MPCDRA.part.rampd = settings->map[3];
            vdp2_settings.MPEFRA.part.rampe = settings->map[4];
            vdp2_settings.MPEFRA.part.rampf = settings->map[5];
            vdp2_settings.MPGHRA.part.rampg = settings->map[6];
            vdp2_settings.MPGHRA.part.ramph = settings->map[7];
            vdp2_settings.MPIJRA.part.rampi = settings->map[8];
            vdp2_settings.MPIJRA.part.rampj = settings->map[9];
            vdp2_settings.MPKLRA.part.rampk = settings->map[10];
            vdp2_settings.MPKLRA.part.rampl = settings->map[11];
            vdp2_settings.MPMNRA.part.rampm = settings->map[12];
            vdp2_settings.MPMNRA.part.rampn = settings->map[13];
            vdp2_settings.MPOPRA.part.rampo = settings->map[14];
            vdp2_settings.MPOPRA.part.rampp = settings->map[15];
            VDP2_REG_MPABRA = vdp2_settings.MPABRA.all;
            VDP2_REG_MPCDRA = vdp2_settings.MPCDRA.all;
            VDP2_REG_MPEFRA = vdp2_settings.MPEFRA.all;
            VDP2_REG_MPGHRA = vdp2_settings.MPGHRA.all;
            VDP2_REG_MPIJRA = vdp2_settings.MPIJRA.all;
            VDP2_REG_MPKLRA = vdp2_settings.MPKLRA.all;
            VDP2_REG_MPMNRA = vdp2_settings.MPMNRA.all;
            VDP2_REG_MPOPRA = vdp2_settings.MPOPRA.all;

            vdp2_settings.MPABRB.part.rbmpa = settings->map[0];
            vdp2_settings.MPABRB.part.rbmpb = settings->map[1];
            vdp2_settings.MPCDRB.part.rbmpc = settings->map[2];
            vdp2_settings.MPCDRB.part.rbmpd = settings->map[3];
            vdp2_settings.MPEFRB.part.rbmpe = settings->map[4];
            vdp2_settings.MPEFRB.part.rbmpf = settings->map[5];
            vdp2_settings.MPGHRB.part.rbmpg = settings->map[6];
            vdp2_settings.MPGHRB.part.rbmph = settings->map[7];
            vdp2_settings.MPIJRB.part.rbmpi = settings->map[8];
            vdp2_settings.MPIJRB.part.rbmpj = settings->map[9];
            vdp2_settings.MPKLRB.part.rbmpk = settings->map[10];
            vdp2_settings.MPKLRB.part.rbmpl = settings->map[11];
            vdp2_settings.MPMNRB.part.rbmpm = settings->map[12];
            vdp2_settings.MPMNRB.part.rbmpn = settings->map[13];
            vdp2_settings.MPOPRB.part.rbmpo = settings->map[14];
            vdp2_settings.MPOPRB.part.rbmpp = settings->map[15];
            VDP2_REG_MPABRB = vdp2_settings.MPABRB.all;
            VDP2_REG_MPCDRB = vdp2_settings.MPCDRB.all;
            VDP2_REG_MPEFRB = vdp2_settings.MPEFRB.all;
            VDP2_REG_MPGHRB = vdp2_settings.MPGHRB.all;
            VDP2_REG_MPIJRB = vdp2_settings.MPIJRB.all;
            VDP2_REG_MPKLRB = vdp2_settings.MPKLRB.all;
            VDP2_REG_MPMNRB = vdp2_settings.MPMNRB.all;
            VDP2_REG_MPOPRB = vdp2_settings.MPOPRB.all;
         }
         break;
      default: break;
   }

   VDP2_REG_RPMD = settings->rotation_mode;

   // RAMCTL's RDBS bits need to be set here(rs's equivalent to the vram access cycles)
   VDP2_REG_RAMCTL &= 0xFF00;
   VDP2_REG_RAMCTL |= CalcRDBSbit(settings->map_offset, 0x3); // |
//                      CalcRDBSbit((settings->parameteraddr & 0x7FFFF) >> 17, 0x1);

   // Reset Parameter Read Enable settings here

   // Rotation Parameter Table Address
   vdp2_settings.RPTA = settings->parameter_addr >> 1;
   VDP2_REG_RPTAU = vdp2_settings.RPTA >> 16;
   VDP2_REG_RPTAL = vdp2_settings.RPTA;

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_RBG0, 1);
      
   vdp2_settings.CHCTLB.all &= 0x00FF;
   vdp2_settings.CHCTLB.part.r0chcn = settings->color & 0x7;

   // If Bitmap, set bitmap settings
   if (settings->is_bitmap)
   {
      // Bitmap Enabled
      vdp2_settings.CHCTLB.all |= (settings->bitmap_size << 9) | 0x200;

      // Special/Extra settings
      vdp2_settings.BMPNB.part.r0bmpr = settings->special_priority & 0x1;
      vdp2_settings.BMPNB.part.r0bmcc = settings->special_color_calc & 0x1;
      vdp2_settings.BMPNB.part.r0bmp = settings->extra_palette_num & 0x7;
      VDP2_REG_BMPNB = vdp2_settings.BMPNB.all;
   }
   else
   {
      // Tile Enabled
      vdp2_settings.CHCTLB.part.r0chsz |= settings->char_size & 0x1;
      if (settings->pattern_name_size & 0x1)
      {
         // 1 Word
         vdp2_settings.PNCR.part.r0pnb = 1;
         vdp2_settings.PNCR.part.r0cnsm = settings->flip_function;
         vdp2_settings.PNCR.part.r0spr = settings->special_priority;
         vdp2_settings.PNCR.part.r0scc = settings->special_color_calc;
         vdp2_settings.PNCR.part.r0splt = settings->extra_palette_num;
         vdp2_settings.PNCR.part.r0scn = settings->extra_char_num;
      }
      else
         // 2 Words
         vdp2_settings.PNCR.part.r0pnb = 0;

      // Pattern Name Control Register
      VDP2_REG_PNCR = vdp2_settings.PNCR.all;
   }

   VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;

   vdp2_settings.BGON.part.rbg0_trans_enab = settings->transparent_bit;

   // Map offset Register
   VDP2_REG_MPOFR = vdp2_settings.MPOFR.all;

   // Character Control Register
   VDP2_REG_CHCTLB = vdp2_settings.CHCTLB.all;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;

   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_rbg0_deinit(void)
{
   vdp2_settings.BGON.part.rbg0_enab = 0;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////

int vdp_rbg1_init(screen_settings_struct *settings)
{
   // First make sure RBG0 is enabled
   if (!vdp2_settings.BGON.part.rbg0_enab)
      return VDP_ERR_CONFLICT;

   vdp2_settings.BGON.part.rbg1_enab = 1;

   // Disable all scroll screens
   vdp2_settings.BGON.part.nbg0_enab = 0;
   vdp2_settings.BGON.part.nbg1_enab = 0;
   vdp2_settings.BGON.part.nbg2_enab = 0;
   vdp2_settings.BGON.part.nbg3_enab = 0;

   // Force RBG0 to use parameter A
   VDP2_REG_RPMD = 0;

   // Rotation data bank select must be disabled for B0/B1
   VDP2_REG_RAMCTL = VDP2_REG_RAMCTL & 0xFF0F;

   // Normalize rotation settings

   // Set Screen-Over to a default value
   vdp2_settings.PLSZ.part.rbovr = 0;
   vdp2_settings.MPOFR.part.rbmp = settings->map_offset;

   // Reset Parameter Read Enable settings here(not sure if it's needed for RBG1)

   // Initialize table
   init_rotation_table((vdp2_settings.RPTA << 1) + 0x80);

   // Adjust Priority(set to a default value)
   vdp_set_priority(SCREEN_RBG1, 1);
      
   // RBG1 uses some of the same registers as NBG0
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

      vdp2_settings.PLSZ.part.rbplsz = settings->plane_size & 0x3;

      // The following could probably be optimized
      vdp2_settings.MPABRB.part.rbmpa = settings->map[0];
      vdp2_settings.MPABRB.part.rbmpb = settings->map[1];
      vdp2_settings.MPCDRB.part.rbmpc = settings->map[2];
      vdp2_settings.MPCDRB.part.rbmpd = settings->map[3];
      vdp2_settings.MPEFRB.part.rbmpe = settings->map[4];
      vdp2_settings.MPEFRB.part.rbmpf = settings->map[5];
      vdp2_settings.MPGHRB.part.rbmpg = settings->map[6];
      vdp2_settings.MPGHRB.part.rbmph = settings->map[7];
      vdp2_settings.MPIJRB.part.rbmpi = settings->map[8];
      vdp2_settings.MPIJRB.part.rbmpj = settings->map[9];
      vdp2_settings.MPKLRB.part.rbmpk = settings->map[10];
      vdp2_settings.MPKLRB.part.rbmpl = settings->map[11];
      vdp2_settings.MPMNRB.part.rbmpm = settings->map[12];
      vdp2_settings.MPMNRB.part.rbmpn = settings->map[13];
      vdp2_settings.MPOPRB.part.rbmpo = settings->map[14];
      vdp2_settings.MPOPRB.part.rbmpp = settings->map[15];
      VDP2_REG_MPABRB = vdp2_settings.MPABRB.all;
      VDP2_REG_MPCDRB = vdp2_settings.MPCDRB.all;
      VDP2_REG_MPEFRB = vdp2_settings.MPEFRB.all;
      VDP2_REG_MPGHRB = vdp2_settings.MPGHRB.all;
      VDP2_REG_MPIJRB = vdp2_settings.MPIJRB.all;
      VDP2_REG_MPKLRB = vdp2_settings.MPKLRB.all;
      VDP2_REG_MPMNRB = vdp2_settings.MPMNRB.all;
      VDP2_REG_MPOPRB = vdp2_settings.MPOPRB.all;
   }

   VDP2_REG_PLSZ = vdp2_settings.PLSZ.all;

   vdp2_settings.BGON.part.nbg0_trans_enab = settings->transparent_bit;

   // Map offset Register
   VDP2_REG_MPOFR = vdp2_settings.MPOFR.all;

   // Character Control Register
   VDP2_REG_CHCTLA = vdp2_settings.CHCTLA.all;

   // Enable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;

   return VDP_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_rbg1_deinit(void)
{
   vdp2_settings.BGON.part.rbg1_enab = 0;

   // Disable Screen
   VDP2_REG_BGON = vdp2_settings.BGON.all;
}

//////////////////////////////////////////////////////////////////////////////
