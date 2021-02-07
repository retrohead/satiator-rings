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

// typedefs
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned int uintptr_t;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

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
} satisfier_info_cmd_t;

#define C_SEEK_SET  0
#define C_SEEK_CUR  1
#define C_SEEK_END  2

// CD image descriptor
typedef struct {
    uint32_t start;             // FAD
    uint32_t length;            // sectors

    uint32_t file_offset;       // byte offset within file where segment data starts
    uint32_t filename_offset;   // byte offset within descfile where filename is. zero for no data

    uint16_t flags;             // future proofing
    uint16_t secsize;           // 2048 or 2352

    uint8_t track;              // 1-99 for disc tracks.
    uint8_t index;              // typically 0 for pregap, 1 for track, others not used. track MSF counts backwards in pregap
    uint8_t q_mode;             // typically 0x41 (data) or 0x01 (audio)

} __attribute__((packed)) seg_desc_t;

#endif // _SATISFIER_H
