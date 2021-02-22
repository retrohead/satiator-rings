/*  Copyright 2007, 2013 Theo Berkau

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

#define SAR0        (*(volatile u32 *)0xFFFFFF80)
#define DAR0        (*(volatile u32 *)0xFFFFFF84)
#define TCR0        (*(volatile u32 *)0xFFFFFF88)
#define CHCR0       (*(volatile u32 *)0xFFFFFF8C)
#define VCRDMA0     (*(volatile u32 *)0xFFFFFFA0)
#define DRCR0       (*(volatile u8 *)0xFFFFFE71)

#define SAR1        (*(volatile u32 *)0xFFFFFF90)
#define DAR1        (*(volatile u32 *)0xFFFFFF94)
#define TCR1        (*(volatile u32 *)0xFFFFFF98)
#define CHCR1       (*(volatile u32 *)0xFFFFFF9C)
#define VCRDMA1     (*(volatile u32 *)0xFFFFFFA8)
#define DRCR1       (*(volatile u8 *)0xFFFFFE72)

#define DMAOR       (*(volatile u32 *)0xFFFFFFB0)

//////////////////////////////////////////////////////////////////////////////

int sh2_dma_init(void)
{
   sh2_dma_stop_all();
   CHCR0 = 0;
   CHCR1 = 0;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int sh2_dma_start(int chan, void *src, void *dst, u32 size, u32 mode)
{
   if (chan == 0)
   {
      // Set Source
      SAR0 = (u32)src;
      // Set Destination
      DAR0 = (u32)dst;
      // Set Transfer Count
      TCR0 = size;
      // Use external request
      DRCR0 = 0;
      // Set the Mode and Enable DMA
      CHCR0 = (CHCR0 & 0x0) | mode | 0x1;

      return IAPETUS_ERR_OK;
   }
   else if (chan == 1)
   {
      // Set Source
      SAR1 = (u32)src;
      // Set Destination
      DAR1 = (u32)dst;
      // Set Transfer Count
      TCR1 = size;
      // Use external request
      DRCR1 = 0;
      // Set the Mode and Enable DMA
      CHCR1 = (CHCR1 & 0x0) | mode | 0x1;

      return IAPETUS_ERR_OK;
   }

   return IAPETUS_ERR_INVALIDARG;
}

//////////////////////////////////////////////////////////////////////////////

BOOL is_sh2_dma_running(int chan)
{
//   if (DMAOR & 0x1)
   {
      // Check if transfer end flag is set
      if (chan == 0 && !(CHCR0 & 0x2))
         return TRUE;
      else if (chan == 1 && !(CHCR1 & 0x2))
         return TRUE;
   }

   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

void sh2_dma_stop_all(void)
{
   DMAOR = DMAOR & 0xE;
}

//////////////////////////////////////////////////////////////////////////////
