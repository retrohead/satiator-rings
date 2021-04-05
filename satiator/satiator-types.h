/*  Copyright 2017 James Laird-Wah

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

/*! \file satiator.h
    \brief Saturn Satiator emulation bits.
*/
#pragma once
#ifndef _SATISFIER_H
#define _SATISFIER_H

#include <STRING.H>
#include <stdint.H>


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long int u32;

typedef enum {
    c_get_status = 0x90,
    c_write_buffer,
    c_read_buffer,

    c_mkfs = 0x94,
    c_info,
    c_settime,

    c_open = 0xA0,
    c_close,
    c_seek,
    c_read,
    c_write,
    c_truncate,
    c_stat,
    c_rename,
    c_unlink,
    c_mkdir,
    c_opendir,
    c_readdir,
    c_chdir,
    c_emulate,
} satiator_cmd_t;

typedef enum {
    i_fw_version = 0,
    i_bootloader_version,
    i_serial_number,
    i_sd_latency,
} satisfier_info_cmd_t;

#define C_SEEK_SET  0
#define C_SEEK_CUR  1
#define C_SEEK_END  2

static char statBuf[280]

#endif // _SATISFIER_H
