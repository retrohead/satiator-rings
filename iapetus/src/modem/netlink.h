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

#ifndef NETLINK_H
#define NETLINK_H

#define LSR_DATAAVAIL           0x01
#define LSR_OVERRUNERR          0x02
#define LSR_PARITYERR           0x04
#define LSR_FRAMINGERR          0x08
#define LSR_BSRECEIVED          0x10
#define LSR_THREMPTY            0x20
#define LSR_THREMPTYANDLINEIDLE 0x40
#define LSR_FIFODATAERROR       0x80

#define MSR_CTSCHANGE           0x01
#define MSR_DSRCHANGE           0x02
#define MSR_RITRAILEDGE         0x04
#define MSR_CDCHANGE            0x08
#define MSR_CTS                 0x10
#define MSR_DSR                 0x20
#define MSR_RI                  0x40
#define MSR_CD                  0x80

void write_uart(u8 num, u8 val);
u8 read_uart(u8 num);
int netlink_init();
void netlink_transmit_character(u8 val);
void netlink_transmit_string(const char *string);
int netlink_receive_character(u8 *data);

#define read_rbr() read_uart(0x0)
#define write_thr(n) write_uart(0x0, n)

#define read_ier() read_uart(0x1)
#define write_ier(n) write_uart(0x1, n)

#define read_iir() read_uart(0x2)
#define write_fcr(n) write_uart(0x2, n)

#define read_lcr() read_uart(0x3)
#define write_lcr(n) write_uart(0x3, n)

#define read_mcr() read_uart(0x4)
#define write_mcr(n) write_uart(0x4, n)

#define read_lsr() read_uart(0x5)

#define read_msr() read_uart(0x6)

#define read_scr() read_uart(0x7)
#define write_scr(n) write_uart(0x7, n)

static inline u8 ReadDLL()
{
    u8 oldlcr;
    u8 ret;
    oldlcr = read_lcr();
    write_lcr(oldlcr | 0x80);
    ret = read_uart(0x0);
    write_lcr(oldlcr);
    return ret;
}
static inline void write_dll(u8 val)
{
    u8 oldlcr;
    oldlcr = read_lcr();
    write_lcr(oldlcr | 0x80);
    write_uart(0x0, val);
    write_lcr(oldlcr);
}

static inline u8 read_dlm()
{
    u8 oldlcr;
    u8 ret;
    oldlcr = read_lcr();
    write_lcr(oldlcr | 0x80);
    ret = read_uart(0x1);
    write_lcr(oldlcr);
    return ret;
}
static inline void write_dlm(u8 val)
{
    u8 oldlcr;
    oldlcr = read_lcr();
    write_lcr(oldlcr | 0x80);
    write_uart(0x1, val);
    write_lcr(oldlcr);
}

#endif
