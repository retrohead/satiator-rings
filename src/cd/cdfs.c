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

#include <string.h>
#include "../iapetus.h"

//////////////////////////////////////////////////////////////////////////////

typedef struct
{
  u16 group_id;
  u16 user_id;
  u16 attributes;
  u16 signature;
  u8 file_number;
  u8 reserved[5];
} xa_rec_struct;

typedef struct
{
  u8 record_size;
  u8 xa_record_size;
  u32 lba;
  u32 size;
  u8 date_year;
  u8 date_month;
  u8 date_day;
  u8 date_hour;
  u8 date_minute;
  u8 date_second;
  u8 gmt_offset;
  u8 flags;
  u8 file_unit_size;
  u8 interleave_gap_size;
  u16 volume_sequence_number;
  u8 name_length;
  char name[32];
  xa_rec_struct xa_record;
} dir_rec_struct;

u8 *dir_tbl;
u8 *sect_buf;
int sect_buffered;
int dir_tbl_size;
int root_lba;
int root_size;

//////////////////////////////////////////////////////////////////////////////

void copy_dir_record(u8 *buffer, dir_rec_struct *dir_rec)
{
  u8 *temp_pointer;

  temp_pointer = buffer;

  memcpy(&dir_rec->record_size, buffer, sizeof(dir_rec->record_size));
  buffer += sizeof(dir_rec->record_size);

  memcpy(&dir_rec->xa_record_size, buffer, sizeof(dir_rec->xa_record_size));
  buffer += sizeof(dir_rec->xa_record_size);

  buffer += sizeof(dir_rec->lba);
  memcpy(&dir_rec->lba, buffer, sizeof(dir_rec->lba));
  buffer += sizeof(dir_rec->lba);

  buffer += sizeof(dir_rec->size);
  memcpy(&dir_rec->size, buffer, sizeof(dir_rec->size));
  buffer += sizeof(dir_rec->size);

  dir_rec->date_year = buffer[0];
  dir_rec->date_month = buffer[1];
  dir_rec->date_day = buffer[2];
  dir_rec->date_hour = buffer[3];
  dir_rec->date_minute = buffer[4];
  dir_rec->date_second = buffer[5];
  dir_rec->gmt_offset = buffer[6];
  buffer += 7;

  dir_rec->flags = buffer[0];
  buffer += sizeof(dir_rec->flags);

  dir_rec->file_unit_size = buffer[0];
  buffer += sizeof(dir_rec->file_unit_size);

  dir_rec->interleave_gap_size = buffer[0];
  buffer += sizeof(dir_rec->interleave_gap_size);

  buffer += sizeof(dir_rec->volume_sequence_number);
  memcpy(&dir_rec->volume_sequence_number, buffer, sizeof(dir_rec->volume_sequence_number));
  buffer += sizeof(dir_rec->volume_sequence_number);

  dir_rec->name_length = buffer[0];
  buffer += sizeof(dir_rec->name_length);

  memset(dir_rec->name, 0, sizeof(dir_rec->name));
  memcpy(dir_rec->name, buffer, dir_rec->name_length);
  buffer += dir_rec->name_length;

  // handle padding
  buffer += (1 - dir_rec->name_length % 2);

  memset(&dir_rec->xa_record, 0, sizeof(dir_rec->xa_record));

  // Sadily, this is the best way I can think of for detecting XA records
  if ((dir_rec->record_size - (buffer - temp_pointer)) == 14)
  {
     memcpy(&dir_rec->xa_record.group_id, buffer, sizeof(dir_rec->xa_record.group_id));
     buffer += sizeof(dir_rec->xa_record.group_id);

     memcpy(&dir_rec->xa_record.user_id, buffer, sizeof(dir_rec->xa_record.user_id));
     buffer += sizeof(dir_rec->xa_record.user_id);

     memcpy(&dir_rec->xa_record.attributes, buffer, sizeof(dir_rec->xa_record.attributes));
     buffer += sizeof(dir_rec->xa_record.attributes);

     memcpy(&dir_rec->xa_record.signature, buffer, sizeof(dir_rec->xa_record.signature));
     buffer += sizeof(dir_rec->xa_record.signature);

     memcpy(&dir_rec->xa_record.file_number, buffer, sizeof(dir_rec->xa_record.file_number));
     buffer += sizeof(dir_rec->xa_record.file_number);

     memcpy(dir_rec->xa_record.reserved, buffer, sizeof(dir_rec->xa_record.reserved));
     buffer += sizeof(dir_rec->xa_record.reserved);
  }
}

//////////////////////////////////////////////////////////////////////////////

int cdfs_init(void *work_dir_tbl, int size)
{
   int ret;
   dir_rec_struct dir_rec;

   dir_tbl = work_dir_tbl;
   dir_tbl_size = size;
   if (size < 4096)
      return IAPETUS_ERR_INVALIDARG;

   sect_buf = work_dir_tbl+2048;
   sect_buffered = 0;

   // Read in lba 16
   if ((ret = cd_read_sector(dir_tbl, 166, SECT_2048, 2048)) != IAPETUS_ERR_OK)
      return ret;

   copy_dir_record(dir_tbl+0x9C, &dir_rec);

   // Ok, now we have the root directory(that's good enough for now)
   root_lba = dir_rec.lba;
   root_size = (dir_rec.size / 2048);

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cdfs_open(const char *path, file_struct *file)
{
   char *p;
   char dir_name[13];
   dir_rec_struct dir_rec;
   u32 lba;
   u32 size;
   int sectors_left;
   u8 *work_buffer;
   int done=0;
   int ret;

   // Starting with the root directory, go through each subdirectory,
   // read the directory table, and find the next subdirectory. Once we're
   // in the correct level, parse through the table and find the file.

   if ((ret = cd_read_sector(dir_tbl, 150+root_lba, SECT_2048, 2048)) != IAPETUS_ERR_OK)
      return ret;

   lba = root_lba + 1;
   sectors_left = root_size - 1;
   work_buffer = dir_tbl;

   if (path == NULL)
   {
      int i;
      // Get first file(I may remove this feature yet)
      for (i = 0; i < 3; i++)
      {
         if (work_buffer[0] == 0)
            return IAPETUS_ERR_FILENOTFOUND;

         copy_dir_record(work_buffer, &dir_rec);
         work_buffer += dir_rec.record_size;
      }

      // We're done.
      file->lba = dir_rec.lba;
      file->size = dir_rec.size;
      file->sect_pos = 0;
      file->pos = 0;

      return IAPETUS_ERR_OK;
   }

   while(!done)
   {
      if ((p = strchr(path, '\\')) == NULL)
      {
         // It's just the filename now
         done = 1;
         size = strlen(path);
      }
      else
         size = p - path;

      strncpy(dir_name, path, size);
      dir_name[size] = '\0';
      path += size+1;

      // Now that we've got the current subdirectory's name, let's find its
      // table.
      for (;;)
      {
         copy_dir_record(work_buffer, &dir_rec);
         work_buffer += dir_rec.record_size;

         if (strcmp(dir_rec.name, dir_name) == 0 ||
             strncmp(dir_rec.name, dir_name, strlen(dir_name)) == 0)
         {
            lba = dir_rec.lba;
            sectors_left = dir_rec.size / 2048;
            break;
         }
           
         // If record size of the next entry is zero, it means we're at the end
         // of the directory record data in the current sector
         if (work_buffer[0] == 0)
         {
            // Let's see if we can read in another sector yet
            if (sectors_left > 0)
            {
               // Read in new sector
               if ((ret = cd_read_sector(dir_tbl, 150+lba, SECT_2048, 2048)) != IAPETUS_ERR_OK)
                  return ret;
               lba++;
               sectors_left--;
               work_buffer = dir_tbl;
            }
            else
               // We can't, let's bail
               return IAPETUS_ERR_FILENOTFOUND;
         }        
      }

      if (done)
         break;

      // Ok, we've found the next directory table, time to read it
      if ((ret = cd_read_sector(dir_tbl, 150+lba, SECT_2048, 2048)) != IAPETUS_ERR_OK)
         return ret;
      lba++;
      sectors_left--;
      work_buffer = dir_tbl;
   }

   // Ok, time to fill out the file structure
   file->lba = lba;
   file->size = dir_rec.size;
   file->sect_pos = 0;
   file->pos = 0;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cdfs_seek(file_struct *file, int offset, int seek_type)
{
   switch(seek_type)
   {
      case CDFS_SEEK_SET:
         file->sect_pos = offset / 2048;
         file->pos = offset % 2048;
         break;
      case CDFS_SEEK_CUR:
         file->sect_pos = file->sect_pos + ((file->pos + offset) / 2048);
         file->pos = (file->pos + offset) % 2048;
         break;
      case CDFS_SEEK_END:
         file->sect_pos = (file->size - 1 - offset) / 2048;
         file->pos = (file->size - 1 - offset) % 2048;
         break;
      default:
         return IAPETUS_ERR_INVALIDARG;
   }

   // Change the CD read position here if need be
   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cdfs_read(u8 *buffer, int size, int num, file_struct *file)
{
   int ret;

   // Only sector-aligned transfers supported for now
   if (file->pos == 0)
   {
      // Straight sectors reads. Nice and fast
      if ((ret = cd_read_sector(buffer, 150+file->lba+file->sect_pos, SECT_2048, size * num)) == IAPETUS_ERR_OK)
         file->sect_pos += (size * num);
      return ret;
   }
   else
      return IAPETUS_ERR_UNSUPPORTED;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cdfs_close(file_struct *file)
{
   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////
