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

#define DSTA_DSP_ACCESS		(1 << 22)
#define DSTA_BBUS_ACCESS	(1 << 21)
#define DSTA_ABUS_ACCESS	(1 << 20)
#define DSTA_DMA1_INT		(1 << 17)
#define DSTA_DMA0_INT		(1 << 16)
#define DSTA_DMA2_STDBY		(1 << 13)
#define DSTA_DMA2_OP			(1 << 12)
#define DSTA_DMA1_STDBY		(1 << 9)
#define DSTA_DMA1_OP			(1 << 8)
#define DSTA_DMA0_STDBY		(1 << 5)
#define DSTA_DMA0_OP			(1 << 4)
#define DSTA_DSP_DMA_STDBY	(1 << 1)
#define DSTA_DSP_DMA_OP		(1 << 0)

//////////////////////////////////////////////////////////////////////////////

int scu_dma_init(void)
{
   scu_dma_stop_all();
   SCU_REG_D0EN = 0;
   SCU_REG_D1EN = 0;
   SCU_REG_D2EN = 0;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int scu_dma_start(int chan, void *src, void *dst, u32 size, u32 add, u32 mode)
{
   // Check first to make sure the channel has no pending DMA operation going
   switch (chan)
   {
      case 0:
         // Make sure everything is disabled
         SCU_REG_D0EN = 0;
         // Write the read address
         SCU_REG_D0R = (u32)src;
         // Write the write address
         SCU_REG_D0W = (u32)dst;
         // Write the transfer number
         SCU_REG_D0C = size;

         SCU_REG_D0AD = add;
         SCU_REG_D0MD = mode;

         if ((mode & 0x7) == 0x7)
            SCU_REG_D0EN = 0x101;
         else
            SCU_REG_D0EN = 0x100;
         break;
      case 1:
         // Make sure everything is disabled
         SCU_REG_D1EN = 0;
         // Write the read address
         SCU_REG_D1R = (u32)src;
         // Write the write address
         SCU_REG_D1W = (u32)dst;
         // Write the transfer number
         SCU_REG_D1C = size;

         SCU_REG_D1AD = add;
         SCU_REG_D1MD = mode;

         if ((mode & 0x7) == 0x7)
            SCU_REG_D1EN = 0x101;
         else
            SCU_REG_D1EN = 0x100;
         break;
      case 2:
         // Make sure everything is disabled
         SCU_REG_D2EN = 0;
         // Write the read address
         SCU_REG_D2R = (u32)src;
         // Write the write address
         SCU_REG_D2W = (u32)dst;
         // Write the transfer number
         SCU_REG_D2C = size;

         SCU_REG_D2AD = add;
         SCU_REG_D2MD = mode;

         if ((mode & 0x7) == 0x7)
            SCU_REG_D2EN = 0x101;
         else
            SCU_REG_D2EN = 0x100;
         break;
      default:
         return IAPETUS_ERR_INVALIDARG;
   }

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

BOOL is_scu_dma_running(int chan)
{
   switch (chan)
   {
      case 0:
         if (SCU_REG_DSTA & DSTA_DMA0_STDBY || SCU_REG_DSTA & DSTA_DMA0_OP)
            return TRUE;
      case 1:
         if (SCU_REG_DSTA & DSTA_DMA1_STDBY || SCU_REG_DSTA & DSTA_DMA1_OP)
            return TRUE;
      case 2:
         if (SCU_REG_DSTA & DSTA_DMA2_STDBY || SCU_REG_DSTA & DSTA_DMA2_OP)
            return TRUE;
      default:
         return FALSE;
   }
}

//////////////////////////////////////////////////////////////////////////////

void scu_dma_stop_all(void)
{
   SCU_REG_DSTP = 1;
}

//////////////////////////////////////////////////////////////////////////////


