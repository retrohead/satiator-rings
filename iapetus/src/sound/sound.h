/*  Copyright 2006-2007,2013 Theo Berkau

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

#ifndef SOUND_H
#define SOUND_H

#define SCSP_REG_MVOL    (*(volatile u8 *)0x25B00401)
#define SCSP_REG_RB      (*(volatile u16 *)0x25B00402)
#define SCSP_REG_MIBUF   (*(volatile u8 *)0x25B00405)

#define SCSP_REG_SCIEB   (*(volatile u16 *)0x25B0041E)
#define SCSP_REG_SCIPD   (*(volatile u16 *)0x25B00420)
#define SCSP_REG_SCIRE   (*(volatile u16 *)0x25B00422)

#define SCSP_REG_SCILV0  (*(volatile u8 *)0x25B00424)
#define SCSP_REG_SCILV1  (*(volatile u8 *)0x25B00426)
#define SCSP_REG_SCILV2  (*(volatile u8 *)0x25B00428)

#define SCSP_REG_COEF(i) *(((volatile u16 *)0x25B00700)+(i))
#define SCSP_REG_ADRS(i) *(((volatile u16 *)0x25B00780)+(i))
#define SCSP_REG_MPRO(i) *(((volatile u16 *)0x25B00800)+(i))

#define sound_key_off_all() (*(volatile u8 *)(0x25B00000) = 0x10)

void sound_init(void);
void sound_load_driver(u8 *data, u32 size);
void sound_external_audio_enable(u8 vol_l, u8 vol_r);

#endif
