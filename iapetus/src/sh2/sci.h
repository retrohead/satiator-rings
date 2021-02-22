/*  Copyright 2006,2013 Theo Berkau

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

#ifndef SCI_H
#define SCI_H

#define SCI_REG_SMR     *((volatile u8 *)0xFFFFFE00)
#define SCI_REG_BRR     *((volatile u8 *)0xFFFFFE01)
#define SCI_REG_SCR     *((volatile u8 *)0xFFFFFE02)
#define SCI_REG_TDR     *((volatile u8 *)0xFFFFFE03)
#define SCI_REG_SSR     *((volatile u8 *)0xFFFFFE04)
#define SCI_REG_RDR     *((volatile u8 *)0xFFFFFE05)

#define SCR_TIE         0x80
#define SCR_RIE         0x40
#define SCR_TE          0x20
#define SCR_RE          0x10
#define SCR_MPIE        0x08
#define SCR_TEIE        0x04

#define SSR_TDRE        0x80
#define SSR_RDRF        0x40
#define SSR_ORER        0x20
#define SSR_FER         0x10
#define SSR_PER         0x08
#define SSR_TEND        0x04
#define SSR_MPB         0x02
#define SSR_MPBT        0x01

#define IPRB *((volatile u16 *)0xFFFFFE60)
#define VCRA *((volatile u16 *)0xFFFFFE62)

enum SCI_SM
{
   SERIALMODE_ASYNC=0x00000000,
   SERIALMODE_CLOCKEDSYNC=0x00800000
};


int sci_init(enum SCI_SM settings);
u8 sci_transfer_byte_no_wait(u8 val, BOOL *success);
void sci_send_byte(u8 val);
u8 sci_receive_byte();
void sci_send_word(u16 val);
void sci_send_long(u32 val);
u16 sci_receive_word();
u32 sci_receive_long();
int sci_remote_execute(void *buffer, u32 addr, u32 size);
int sci_sync_out();
int sci_sync_in();
void sci_handler();
#endif
