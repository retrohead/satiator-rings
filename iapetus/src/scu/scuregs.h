/*  Copyright 2007,2013-2014 Theo Berkau

    This file is part of Lapetus.

    Lapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Lapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SCUREGS_H
#define SCUREGS_H

#define SCU_REG_D0R   (*(volatile u32 *)0x25FE0000)
#define SCU_REG_D0W   (*(volatile u32 *)0x25FE0004)
#define SCU_REG_D0C   (*(volatile u32 *)0x25FE0008)
#define SCU_REG_D0AD  (*(volatile u32 *)0x25FE000C)
#define SCU_REG_D0EN  (*(volatile u32 *)0x25FE0010)
#define SCU_REG_D0MD  (*(volatile u32 *)0x25FE0014)

#define SCU_REG_D1R   (*(volatile u32 *)0x25FE0020)
#define SCU_REG_D1W   (*(volatile u32 *)0x25FE0024)
#define SCU_REG_D1C   (*(volatile u32 *)0x25FE0028)
#define SCU_REG_D1AD  (*(volatile u32 *)0x25FE002C)
#define SCU_REG_D1EN  (*(volatile u32 *)0x25FE0030)
#define SCU_REG_D1MD  (*(volatile u32 *)0x25FE0034)

#define SCU_REG_D2R   (*(volatile u32 *)0x25FE0040)
#define SCU_REG_D2W   (*(volatile u32 *)0x25FE0044)
#define SCU_REG_D2C   (*(volatile u32 *)0x25FE0048)
#define SCU_REG_D2AD  (*(volatile u32 *)0x25FE004C)
#define SCU_REG_D2EN  (*(volatile u32 *)0x25FE0050)
#define SCU_REG_D2MD  (*(volatile u32 *)0x25FE0054)

#define SCU_REG_DSTP				(*(volatile u32 *)0x25FE0060)
#define SCU_REG_DSTA				(*(volatile u32 *)0x25FE0070)

#define SCU_REG_PPAF				(*(volatile u32 *)0x25FE0080)
#define SCU_REG_PPD				(*(volatile u32 *)0x25FE0084)
#define SCU_REG_PDA				(*(volatile u32 *)0x25FE0088)
#define SCU_REG_PDD				(*(volatile u32 *)0x25FE008C)

#define SCU_REG_T0C				(*(volatile u32 *)0x25FE0090)
#define SCU_REG_T1S				(*(volatile u32 *)0x25FE0094)
#define SCU_REG_T1MD				(*(volatile u32 *)0x25FE0098)

#define SCU_REG_IMS				(*(volatile u32 *)0x25FE00A0)
#define SCU_REG_IST				(*(volatile u32 *)0x25FE00A4)

#define SCU_REG_ABUSSRCS0CS1	(*(volatile u32 *)0x25FE00B0)
#define SCU_REG_ABUSREFRESH	(*(volatile u32 *)0x25FE00B8)

#define SCU_REG_RSEL  (*(volatile u32 *)0x25FE00C4)
#define SCU_REG_VER   (*(volatile u32 *)0x25FE00C8)

#endif