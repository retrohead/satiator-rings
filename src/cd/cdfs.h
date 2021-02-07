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

#ifndef CDFS_H
#define CDFS_H

typedef struct
{
   u32 lba;
   u32 size;
   u32 sect_pos;
   u32 pos;
} file_struct;

enum SEEKTYPES
{
   CDFS_SEEK_SET = 0,
   CDFS_SEEK_CUR = 1,
   CDFS_SEEK_END = 2
};

int cdfs_init(void *work_dir_tbl, int size);
int cdfs_open(const char *path, file_struct *file);
int cdfs_seek(file_struct *file, int offset, int seek_type);
int cdfs_read(u8 *buffer, int size, int num, file_struct *file);
int cdfs_close(file_struct *file);

#endif
