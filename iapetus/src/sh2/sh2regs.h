/*  Copyright 2007,2013-2014 Theo Berkau

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

#define SH2_REG_M_FRT_IC			(*(volatile u16 *)0x21000000)
#define SH2_REG_S_FRT_IC			(*(volatile u16 *)0x21800000)

#define SH2_REG_TIER					(*(volatile u8 *)0xFFFFFE10)
#define SH2_REG_FTCSR				(*(volatile u8 *)0xFFFFFE11)

#define SH2_REG_FRC_W(val)			(*(volatile u8 *)0xFFFFFE12) = 0; \
											(*(volatile u8 *)0xFFFFFE13) = 0;

#define SH2_REG_FRC_R				(((*(volatile u8 *)0xFFFFFE12) << 8) | (*(volatile u8 *)0xFFFFFE13))

#define SH2_REG_TCR					(*(volatile u8 *)0xFFFFFE16)

#define SH2_REG_IPRB					(*(volatile u16 *)0xFFFFFE60)

#define SH2_REG_WTCSR_R				(*(volatile u8 *)0xFFFFFE80)
#define SH2_REG_WTCSR_W(val)		(*((volatile u16 *)0xFFFFFE80) = 0xA500 | (u8)(val))
#define SH2_REG_WTCNT_R				(*(volatile u8 *)0xFFFFFE81)
#define SH2_REG_WTCNT_W(val)		(*((volatile u16 *)0xFFFFFE80) = 0x5A00 | (u8)(val))

#define SH2_REG_CCR					(*(volatile u16 *)0xFFFFFE92)

#define SH2_REG_IPRA					(*(volatile u16 *)0xFFFFFEE2)
#define SH2_REG_VCRWDT				(*(volatile u16 *)0xFFFFFEE4)

#define SH2_REG_RTCSR_R				(*(volatile u32 *)0xFFFFFFF0)
#define SH2_REG_RTCSR_W(val)		(*((volatile u32 *)0xFFFFFFF0) = 0xA55A0000 | (u16)(val))
#define SH2_REG_RTCNT_R				(*(volatile u32 *)0xFFFFFFF4)
#define SH2_REG_RTCNT_W(val)		(*((volatile u32 *)0xFFFFFFF4) = 0xA55A0000 | (u16)(val))
#define SH2_REG_RTCOR_R				(*(volatile u32 *)0xFFFFFFF8)
#define SH2_REG_RTCOR_W(val)		(*((volatile u32 *)0xFFFFFFF8) = 0xA55A0000 | (u16)(val))
