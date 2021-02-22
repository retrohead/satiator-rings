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

//////////////////////////////////////////////////////////////////////////////

int command_num=0;

//////////////////////////////////////////////////////////////////////////////

void vdp1_set_draw_mode(enum VDP1_DRAW_MODE mode, enum VDP1_SCREEN_MODE tv_mode, enum VDP1_INT_MODE int_mode)
{
   u16 tvmr=0;
   u16 fbcr=0;

   switch (mode)
   {
      case V1DM_AUTO:
         break;
      case V1DM_MANUALERASE:
         fbcr = 0x2;
         break;
      case V1DM_MANUALCHANGE:
         fbcr = 0x1;
         break;
      case V1DM_MANUALERASECHANGE:
         tvmr = 0x8;
         fbcr = 0x3;
         break;
      default: break;
   }

   VDP1_REG_TVMR = tvmr | tv_mode;   
   VDP1_REG_FBCR = int_mode | int_mode | fbcr;
}

//////////////////////////////////////////////////////////////////////////////

void vdp1_trigger_draw()
{
   VDP1_REG_PTMR = 0x01;
}

//////////////////////////////////////////////////////////////////////////////

void vdp1_read_fb(void *buf)
{
   u32 addr;
   for (addr = 0x25C80000; addr < 0x25CC0000; addr+=4)
   {
      *((u32 *)buf) = *((volatile u32 *)addr);
      buf = buf + 4;
   }
}

//////////////////////////////////////////////////////////////////////////////

void vdp_system_clipping(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0009;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_xc = sprite->x;
   tbl->cmd_yc = sprite->y;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_user_clipping(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0008;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_xc = sprite->x2;
   tbl->cmd_yc = sprite->y2;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_local_coordinate(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x000A;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   command_num++;
}


//////////////////////////////////////////////////////////////////////////////

void vdp_draw_normal_sprite(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0000;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_pmod.all = (u16)sprite->attr;
   tbl->cmd_colr = sprite->bank;
   tbl->cmd_srca = sprite->addr / 8;
   tbl->cmd_size = ((sprite->width / 8) << 8) | sprite->height;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_grda = sprite->gouraud_addr / 8;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_scaled_sprite(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0001;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_pmod.all = (u16)sprite->attr;
   tbl->cmd_colr = sprite->bank;
   tbl->cmd_srca = sprite->addr / 8;
   tbl->cmd_size = ((sprite->width / 8) << 8) | sprite->height;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_xb = sprite->x2;
   tbl->cmd_yb = sprite->y2;
   tbl->cmd_xc = sprite->x3;
   tbl->cmd_yc = sprite->y3;
   tbl->cmd_xd = sprite->x4;
   tbl->cmd_yd = sprite->y4;
   tbl->cmd_grda = sprite->gouraud_addr / 8;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_distorted_sprite(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0002;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_pmod.all = (u16)sprite->attr;
   tbl->cmd_colr = sprite->bank;
   tbl->cmd_srca = sprite->addr / 8;
   tbl->cmd_size = ((sprite->width / 8) << 8) | sprite->height;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_xb = sprite->x2;
   tbl->cmd_yb = sprite->y2;
   tbl->cmd_xc = sprite->x3;
   tbl->cmd_yc = sprite->y3;
   tbl->cmd_xd = sprite->x4;
   tbl->cmd_yd = sprite->y4;
   tbl->cmd_grda = sprite->gouraud_addr / 8;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_polygon(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0004;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_pmod.all = (u16)sprite->attr | 0xC0 ;
   tbl->cmd_colr = sprite->bank;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_xb = sprite->x2;
   tbl->cmd_yb = sprite->y2;
   tbl->cmd_xc = sprite->x3;
   tbl->cmd_yc = sprite->y3;
   tbl->cmd_xd = sprite->x4;
   tbl->cmd_yd = sprite->y4;
   tbl->cmd_grda = sprite->gouraud_addr / 8;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_poly_line(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0005;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_pmod.all = (u16)sprite->attr | 0xC0;
   tbl->cmd_colr = sprite->bank;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_xb = sprite->x2;
   tbl->cmd_yb = sprite->y2;
   tbl->cmd_xc = sprite->x3;
   tbl->cmd_yc = sprite->y3;
   tbl->cmd_xd = sprite->x4;
   tbl->cmd_yd = sprite->y4;
   tbl->cmd_grda = sprite->gouraud_addr / 8;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_draw_line(sprite_struct *sprite)
{
   volatile vdp1cmd_struct *tbl=(volatile vdp1cmd_struct *)(VDP1_RAM+(command_num * 0x20));

   tbl->cmd_ctrl = ((sprite->attr >> 12) & 0x7FF0) | 0x0006;
   tbl->cmd_link = sprite->link_addr / 8;
   tbl->cmd_pmod.all = (u16)sprite->attr | 0xC0;
   tbl->cmd_colr = sprite->bank;
   tbl->cmd_xa = sprite->x;
   tbl->cmd_ya = sprite->y;
   tbl->cmd_xb = sprite->x2;
   tbl->cmd_yb = sprite->y2;
   tbl->cmd_grda = sprite->gouraud_addr / 8;
   command_num++;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_start_draw_list()
{
   command_num = 0;
}

//////////////////////////////////////////////////////////////////////////////

void vdp_end_draw_list()
{
   *((volatile u16 *)(VDP1_RAM+(command_num * 0x20))) = 0x8000;
}

//////////////////////////////////////////////////////////////////////////////
