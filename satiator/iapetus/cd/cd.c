#include <jo/jo.h>
#include "cd.h"
#include "../bios.h"
#include "../sh2/sh2int.h"


//////////////////////////////////////////////////////////////////////////////

static int cd_sector_size = SECT_2048;
int sector_size_tbl[4] = { 2048, 2336, 2340, 2352 };

//////////////////////////////////////////////////////////////////////////////

void cd_write_command(cd_cmd_struct *cd_cmd)
{
   CDB_REG_CR1 = cd_cmd->CR1;
   CDB_REG_CR2 = cd_cmd->CR2;
   CDB_REG_CR3 = cd_cmd->CR3;
   CDB_REG_CR4 = cd_cmd->CR4;
}

//////////////////////////////////////////////////////////////////////////////

void cd_read_return_status(cd_cmd_struct *cd_cmd_rs)
{
   cd_cmd_rs->CR1 = CDB_REG_CR1;
   cd_cmd_rs->CR2 = CDB_REG_CR2;
   cd_cmd_rs->CR3 = CDB_REG_CR3;
   cd_cmd_rs->CR4 = CDB_REG_CR4;
}

//////////////////////////////////////////////////////////////////////////////

int cd_wait_hirq(int flag)
{
   int i;
   u16 hirq_temp;
   
   for (i = 0; i < 0x240000; i++)
   {
      hirq_temp = CDB_REG_HIRQ;
      if (hirq_temp & flag)
         return TRUE;
   }
   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

int cd_exec_command(u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs)
{
   int old_level_mask;
   u16 hirq_temp;
   u16 cd_status;

   // Mask any interrupts, we don't need to be interrupted
   old_level_mask = interrupt_get_level_mask();
   interrupt_set_level_mask(0xF);

   hirq_temp = CDB_REG_HIRQ;

   // Make sure CMOK flag is set, or we can't continue
   if (!(hirq_temp & HIRQ_CMOK))
      return IAPETUS_ERR_CMOK;

   // Clear CMOK and any other user-defined flags
   CDB_REG_HIRQ = ~(hirq_mask | HIRQ_CMOK);

   // Alright, time to execute the command
   cd_write_command(cd_cmd);

   // Let's wait till the command operation is finished
   if (!cd_wait_hirq(HIRQ_CMOK))
      return IAPETUS_ERR_TIMEOUT;

   // Read return data
   cd_read_return_status(cd_cmd_rs);

   cd_status = cd_cmd_rs->CR1 >> 8;

   // Was command good?
   if (cd_status == STATUS_REJECT)
      return IAPETUS_ERR_INVALIDARG;
   else if (cd_status & STATUS_WAIT)
      return IAPETUS_ERR_BUSY;

   // return interrupts back to normal
   interrupt_set_level_mask(old_level_mask);

   // It's all good
   return IAPETUS_ERR_OK;
}

/*
////////////////////////////////////////////////////////////////////////////

void cd_get_info_data(int size, u16 *data)
{
   int i;
   for (i = 0; i < size; i++)
      data[i] = *((volatile u16 *)0x25898000);
}


////////////////////////////////////////////////////////////////////////////

int cd_debug_exec_command(font_struct *font, u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs)
{
   int old_level_mask;
   u16 hirq_temp;
   u16 hirq;
   u16 cr1, cr2, cr3, cr4;

   // Mask any interrupts, we don't need to be interrupted
   old_level_mask = interrupt_get_level_mask();
   interrupt_set_level_mask(0xF);

   hirq_temp = CDB_REG_HIRQ;

   // Make sure CMOK flag is set, or we can't continue
   if (!(hirq_temp & HIRQ_CMOK))
      return IAPETUS_ERR_BUSY;

   // Clear CMOK and any other user-defined flags
   CDB_REG_HIRQ = ~(hirq_mask | HIRQ_CMOK);

   // Alright, time to execute the command
   cd_write_command(cd_cmd);

   if (!cd_wait_hirq(HIRQ_CMOK))
      return IAPETUS_ERR_TIMEOUT;

   // Read return data
   cd_read_return_status(cd_cmd_rs);

   hirq = CDB_REG_HIRQ;

   // Go into an endless loop showing the HIRQ and CR's
   vdp_print_text(font, 2 * 8, 20 * 8, 15, "HIRQ = ");
   vdp_print_text(font, 2 * 8, 21 * 8, 15, "CR1 = ");
   vdp_print_text(font, 2 * 8, 22 * 8, 15, "CR2 = ");
   vdp_print_text(font, 2 * 8, 23 * 8, 15, "CR3 = ");
   vdp_print_text(font, 2 * 8, 24 * 8, 15, "CR4 = ");

   cr1 = cd_cmd_rs->CR1;
   cr2 = cd_cmd_rs->CR2;
   cr3 = cd_cmd_rs->CR3;
   cr4 = cd_cmd_rs->CR4;

   interrupt_set_level_mask(old_level_mask);
   for (;;)
   {
      vdp_vsync();
      vdp_printf(font, 9 * 8, 20 * 8, 15, "%04X", hirq);
      vdp_printf(font, 9 * 8, 21 * 8, 15, "%04X", cr1);
      vdp_printf(font, 9 * 8, 22 * 8, 15, "%04X", cr2);
      vdp_printf(font, 9 * 8, 23 * 8, 15, "%04X", cr3);
      vdp_printf(font, 9 * 8, 24 * 8, 15, "%04X", cr4);

      if (per[0].but_push_once & PAD_A)
      {
         vdp_vsync();
         break;
      }
   }

   for (;;)
   {
      vdp_vsync();
      vdp_printf(font, 9 * 8, 20 * 8, 15, "%04X", hirq);
      vdp_printf(font, 9 * 8, 21 * 8, 15, "%04X", cr1);
      vdp_printf(font, 9 * 8, 22 * 8, 15, "%04X", cr2);
      vdp_printf(font, 9 * 8, 23 * 8, 15, "%04X", cr3);
      vdp_printf(font, 9 * 8, 24 * 8, 15, "%04X", cr4);

      if (per[0].but_push_once & PAD_A)
      {
         interrupt_set_level_mask(old_level_mask);
         vdp_vsync();
         break;
      }

      interrupt_set_level_mask(0xF);
      hirq=CDB_REG_HIRQ;
      cr1=CDB_REG_CR1, cr2=CDB_REG_CR2, cr3=CDB_REG_CR3, cr4=CDB_REG_CR4;
      interrupt_set_level_mask(old_level_mask);
   }

   // return interrupts back to normal
   interrupt_set_level_mask(old_level_mask);
   return IAPETUS_ERR_OK;
}

////////////////////////////////////////////////////////////////////////////

int cd_get_hw_info(hw_info_struct *hw_info)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;

   cd_cmd.CR1 = 0x0100;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_OK)
      return ret;

   hw_info->hw_flag = cd_cmd_rs.CR2 >> 8;
   hw_info->hw_ver = cd_cmd_rs.CR2;
   hw_info->mpeg_ver = cd_cmd_rs.CR3;
   hw_info->drive_ver = cd_cmd_rs.CR4 >> 8;
   hw_info->drive_rev = cd_cmd_rs.CR4;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_toc(u32 *toc)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Get TOC
   cd_cmd.CR1 = 0x0200;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_DRDY, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   if (ret == IAPETUS_ERR_OK)
   {
      // Wait for data to be ready
      if (!cd_wait_hirq(HIRQ_DRDY))
         return IAPETUS_ERR_TIMEOUT;

      cd_get_info_data(cd_cmd_rs.CR2, (u16 *)toc);
      return cd_end_transfer();
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_session_num(u8 *num)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Get Session Info
   cd_cmd.CR1 = 0x0300;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   num[0] = cd_cmd_rs.CR3 >> 8;
   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_session_info(u8 num, u32 *lba)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Get Session Info
   cd_cmd.CR1 = 0x0300 | num;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   lba[0] = ((cd_cmd_rs.CR3 & 0xFF) << 8) | cd_cmd_rs.CR4;
   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int cd_cdb_init(int standby)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // CD Init Command
   //cd_cmd.CR1 = 0x0402; // subcode-rw flag
   cd_cmd.CR1 = 0x0400;
   cd_cmd.CR2 = standby;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x040F;

   return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
}
*/
//////////////////////////////////////////////////////////////////////////////

int cd_end_transfer()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x0600;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs);

   CDB_REG_HIRQ = (~HIRQ_DRDY) | HIRQ_CMOK;

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_play_fad(int play_mode, int start_fad, int num_sectors)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;

   // Clear flags
   CDB_REG_HIRQ = ~(HIRQ_PEND|HIRQ_CSCT) | HIRQ_CMOK;

   cd_cmd.CR1 = 0x1080 | (start_fad >> 16);
   cd_cmd.CR2 = start_fad;
   cd_cmd.CR3 = (play_mode << 8) | 0x80 | (num_sectors >> 16);
   cd_cmd.CR4 = num_sectors;

   ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs);

   return ret;
}

/*
//////////////////////////////////////////////////////////////////////////////

int cd_seek_fad(int seekfad)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x1180 | (seekfad >> 16);
   cd_cmd.CR2 = seekfad;
   cd_cmd.CR3 = 0;
   cd_cmd.CR4 = 0;

   return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int cd_get_subcode(enum SUBCODE_TYPE type, u16 *data, u8 *flags)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int i;

   cd_cmd.CR1 = 0x2000 | (type & 0xFF);
   cd_cmd.CR2 = 0;
   cd_cmd.CR3 = 0;
   cd_cmd.CR4 = 0;

   for (i = 0; i < 15; i++)
   {
      ret = cd_exec_command(HIRQ_DRDY, &cd_cmd, &cd_cmd_rs);

      if (ret == IAPETUS_ERR_OK)
      {
         *flags = cd_cmd_rs.CR4 & 0xFF;

         // Wait for data to be ready
         if (!cd_wait_hirq(HIRQ_DRDY))
            return IAPETUS_ERR_TIMEOUT;

         cd_get_info_data(cd_cmd_rs.CR2, data);
         return cd_end_transfer();
      }
      else if (ret != IAPETUS_ERR_BUSY)
         break;
   }

   return ret;
}
*/
//////////////////////////////////////////////////////////////////////////////

int cd_connect_cd_to_filter(u8 filter_num)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x3000;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = filter_num << 8;
   cd_cmd.CR4 = 0x0000;

   return cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);
}
/*
//////////////////////////////////////////////////////////////////////////////

int cd_set_filter_range(u8 filter_num, cd_range_struct *cd_range)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4000 | ((cd_range->fad >> 16) & 0xFF);
   cd_cmd.CR2 = cd_range->fad;
   cd_cmd.CR3 = (filter_num << 8) | ((cd_range->range >> 16) & 0xFF);
   cd_cmd.CR4 = cd_range->range;

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_filter_subheader_conditions(u8 filter_num, cd_sh_cond_struct *sh_cond)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4200 | sh_cond->channel;
   cd_cmd.CR2 = (sh_cond->sm_mask << 8) | sh_cond->ci_mask;
   cd_cmd.CR3 = (filter_num << 8) | sh_cond->file_id;
   cd_cmd.CR4 = (sh_cond->sm_val << 8) | sh_cond->ci_val;

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

static int cd_set_filter_mode(u8 filter_num, u8 mode)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4400 | mode;
   cd_cmd.CR2 = 0;
   cd_cmd.CR3 = (filter_num << 8);
   cd_cmd.CR4 = 0;

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_filter_connection(u8 filter_num, cd_con_struct *cd_con)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x4600 | cd_con->connect_flags;
   cd_cmd.CR2 = (cd_con->true_con << 8) | cd_con->false_con;
   cd_cmd.CR3 = (filter_num << 8);
   cd_cmd.CR4 = 0;

   ret = cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);

   // Wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return ret;
}

//////////////////////////////////////////////////////////////////////////////

int cd_set_filter(u8 filter_num, u8 mode, cd_sh_cond_struct *sh_cond, cd_range_struct *cd_range, cd_con_struct *cd_con)
{
   int ret;

   if ((ret = cd_set_filter_mode(filter_num, mode)) != IAPETUS_ERR_OK)
      return ret;

   if (sh_cond)
   {
      if ((ret = cd_set_filter_subheader_conditions(filter_num, sh_cond)) != IAPETUS_ERR_OK)
         return ret;
   }

   if (cd_range)
   {
      if ((ret = cd_set_filter_range(filter_num, cd_range)) != IAPETUS_ERR_OK)
         return ret;
   }

   if (cd_con)
   {
      if ((ret = cd_set_filter_connection(filter_num, cd_con)) != IAPETUS_ERR_OK)
         return ret;
   }

   return IAPETUS_ERR_OK;
}
*/
//////////////////////////////////////////////////////////////////////////////

static int cd_reset_selector(u8 reset_flags, u8 sel_num)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Reset Selector Command
   cd_cmd.CR1 = 0x4800 | reset_flags;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = (sel_num << 8);
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // wait for function to finish
   while (!(CDB_REG_HIRQ & HIRQ_ESEL)) {}

   return IAPETUS_ERR_OK;
}

////////////////////////////////////////////////////////////////////////////

int cd_reset_selector_one(int sel_num)
{
   return cd_reset_selector(0, sel_num);
}

//////////////////////////////////////////////////////////////////////////////

int cd_reset_selector_all()
{
   return cd_reset_selector(0xFC, 0);
}

////////////////////////////////////////////////////////////////////////////

int cd_is_data_ready(int sel_num)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x5100;
   cd_cmd.CR2 = 0;
   cd_cmd.CR3 = (sel_num << 8);
   cd_cmd.CR4 = 0;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return IAPETUS_ERR_OK;

   // Return the number of sectors ready
   return cd_cmd_rs.CR4;
}

////////////////////////////////////////////////////////////////////////////////

int cd_set_sector_size(int size)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_sector_size = size;

   cd_cmd.CR1 = 0x6000 | (size & 0xFF);
   cd_cmd.CR2 = size << 8;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return cd_exec_command(HIRQ_ESEL, &cd_cmd, &cd_cmd_rs);
}


//////////////////////////////////////////////////////////////////////////////

int cd_get_then_delete_sector_data(u8 sel_num, u16 sector_pos, u16 num_sectors)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x6300;
   cd_cmd.CR2 = sector_pos;
   cd_cmd.CR3 = sel_num << 8;
   cd_cmd.CR4 = num_sectors;

   return cd_exec_command(HIRQ_EHST, &cd_cmd, &cd_cmd_rs);
}

// //////////////////////////////////////////////////////////////////////////////

int cd_abort_file()
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   // Abort File Command
   cd_cmd.CR1 = 0x7500;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs);
}

// //////////////////////////////////////////////////////////////////////////////

// int cd_transfer_data(u32 num_sectors, u32 *buffer)
// {
//    u32 i;
//    int ret;

//    // Setup a transfer from cd buffer to wram, then delete data
//    // from cd buffer
//    if ((ret = cd_get_then_delete_sector_data(0, 0, num_sectors)) != 0)
//       return ret;

//    // Wait till data is ready
//    if (!cd_wait_hirq(HIRQ_DRDY | HIRQ_EHST))
//    	   return IAPETUS_ERR_TIMEOUT;

//    // Do transfer
//    for (i = 0; i < ((num_sectors * sector_size_tbl[cd_sector_size]) / 4); i++)
//       buffer[i] = CDB_REG_DATATRNS; // this can also be done in word units as well

//    if ((ret = cd_end_transfer()) != 0)
//       return ret;

//    return IAPETUS_ERR_OK;
// }


////////////////////////////////////////////////////////////////////////////

int cd_transfer_data_bytes(u32 num_bytes, u32 *buffer)
{
   u32 i;
   int ret;
   int num_sectors=num_bytes / sector_size_tbl[cd_sector_size];

   if (num_bytes % sector_size_tbl[cd_sector_size])
      num_sectors++;

   // Setup a transfer from cd buffer to wram, then delete data
   // from cd buffer
   if ((ret = cd_get_then_delete_sector_data(0, 0, num_sectors)) != 0)
      return ret;

   // Wait till data is ready
   if (!cd_wait_hirq(HIRQ_DRDY | HIRQ_EHST))
   	   return IAPETUS_ERR_TIMEOUT;

   // Do transfer
   for (i = 0; i < (num_bytes >> 2); i++)
   {
      buffer[0] = CDB_REG_DATATRNS; // this can also be done in word units as well
      buffer++;
   }

   // Get the remainder
   if (num_bytes % 4)
   {
      u32 data;
      u8 *datapointer=(u8 *)&data;

      data = CDB_REG_DATATRNS;

      for (i = 0; i < (num_bytes % 4); i++)
         ((u8 *)buffer)[i] = datapointer[i];
   }

   if ((ret = cd_end_transfer()) != 0)
      return ret;

   return IAPETUS_ERR_OK;
}

// //////////////////////////////////////////////////////////////////////////////

// int cd_init()
// {
//    int ret;

//    // Abort any file transfers that may be currently going
//    if ((ret = cd_abort_file()) != 0)
//       return ret;

//    // Init CD Block
//    if ((ret = cd_cdb_init(0)) != 0)
//       return ret;

//    // End any previous cd buffer data transfers
//    if ((ret = cd_end_transfer()) != 0)
//       return ret;

//    // Reset all buffer partitions, partition output connectors, all filter
//    // conditions, all filter input connectors, etc.
//    if ((ret = cd_reset_selector_all()) != 0)
//       return ret;

//    return IAPETUS_ERR_OK;
// }

//////////////////////////////////////////////////////////////////////////////

int cd_get_stat(cd_stat_struct *cd_status)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;

   cd_cmd.CR1 = 0x0000;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   cd_status->status = cd_cmd_rs.CR1 >> 8;
   cd_status->flag = (cd_cmd_rs.CR1 >> 4) & 0xF;
   cd_status->repeat_cnt = cd_cmd_rs.CR1 & 0xF;
   cd_status->ctrl_addr = cd_cmd_rs.CR2 >> 8;
   cd_status->track = cd_cmd_rs.CR2 & 0xFF;
   cd_status->index = cd_cmd_rs.CR3 >> 8;
   cd_status->FAD = ((cd_cmd_rs.CR3 & 0xFF) << 16) | cd_cmd_rs.CR4;

   return IAPETUS_ERR_OK;
}


////////////////////////////////////////////////////////////////////////////

int is_cd_auth(u16 *disc_type_auth)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xE100;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   // If command fails, assume it's not authenticated
   if (cd_exec_command(0, &cd_cmd, &cd_cmd_rs) != IAPETUS_ERR_OK)
      return FALSE;

   if (disc_type_auth)
      *disc_type_auth = cd_cmd_rs.CR2;

   // Disc type Authenticated:
   // 0x00: No CD/Not Authenticated
   // 0x01: Audio CD
   // 0x02: Regular Data CD(not Saturn disc)
   // 0x03: Copied/Pirated Saturn Disc
   // 0x04: Original Saturn Disc
   if (cd_cmd_rs.CR2 != 0x04 && cd_cmd_rs.CR2 != 0x02 && cd_cmd_rs.CR2 != 0x01)
      return FALSE;

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int cd_auth()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   u16 auth;
   cd_stat_struct cd_status;
   int i;

   // Clear hirq flags
   CDB_REG_HIRQ = ~(HIRQ_DCHG | HIRQ_EFLS);

   // Authenticate disc
   cd_cmd.CR1 = 0xE000;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_EFLS, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // wait till operation is finished
   while (!(CDB_REG_HIRQ & HIRQ_EFLS)) {}

   // Wait until drive has finished seeking
   for (;;)
   {
      // wait a bit
      for (i = 0; i < 100000; i++) { }

      if (cd_get_stat(&cd_status) != 0) continue;

      if (cd_status.status == STATUS_PAUSE) break;
      else if (cd_status.status == STATUS_FATAL) return IAPETUS_ERR_UNKNOWN;
   }

   // Was Authentication successful?
   if (!is_cd_auth(&auth))
      return IAPETUS_ERR_AUTH;

   return IAPETUS_ERR_OK;
}


//////////////////////////////////////////////////////////////////////////////

int cd_stop_drive()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   cd_stat_struct cd_status;
   int i;

   // CD Init Command
   cd_cmd.CR1 = 0x0400;
   cd_cmd.CR2 = 0x0001;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x040F;

   if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
      return ret;

   // Wait till operation is finished(fix me)

   // Wait till drive is stopped
   for (;;)
   {
      // wait a bit
      for (i = 0; i < 100000; i++) { }

      if (cd_get_stat(&cd_status) != 0) continue;

      if (cd_status.status == STATUS_STANDBY) break;
      else if (cd_status.status == STATUS_FATAL) return IAPETUS_ERR_UNKNOWN;
   }

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

// int cd_start_drive()
// {
//    int ret;
//    cd_cmd_struct cd_cmd;
//    cd_cmd_struct cd_cmd_rs;

//    // CD Init Command
//    cd_cmd.CR1 = 0x0400;
//    cd_cmd.CR2 = 0x0000;
//    cd_cmd.CR3 = 0x0000;
//    cd_cmd.CR4 = 0x040F;

//    if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != 0)
//       return ret;

//    // wait till operation is finished(fix me)

//    return IAPETUS_ERR_OK;
// }

//////////////////////////////////////////////////////////////////////////////

int is_cd_present()
{
   cd_stat_struct cd_status;

   // If command fails, assume disc isn't present
   if (cd_get_stat(&cd_status) != 0)
      return 0;

   // Check status
   switch (cd_status.status & 0xF)
   {
      case STATUS_BUSY:
         return 0;
      case STATUS_OPEN:
         return 0;
      case STATUS_NODISC:
         return 0;
      default: break;
   }

   return 1;
}


//////////////////////////////////////////////////////////////////////////////

int cd_read_sector(void *buffer, u32 FAD, int sector_size, int num_bytes)
{
   int ret;
   int done=0;
   // Figure out how many sectors we actually have to read
   int num_sectors=num_bytes / sector_size_tbl[cd_sector_size];

   if (num_bytes % sector_size_tbl[cd_sector_size] != 0)
      num_sectors++;

   if ((ret = cd_set_sector_size(sector_size)) != 0)
      return ret;
   // Set Filter
/*
//   if ((ret = CDSetFilter(0, 1, 0, 0xFF)) != 0)
//   if ((ret = CDSetFilter(0, 0x80, 0, 0xFF)) != 0)
   if ((ret = CDSetFilter(0, 0x10, 0, 1)) != 0)
      return ret;
*/
   // Clear partition 0
   if ((ret = cd_reset_selector_one(0)) != 0)
      return ret;

   // Connect CD device to filter 0
   if ((ret = cd_connect_cd_to_filter(0)) != 0)
      return ret;

   // Start reading sectors
   if ((ret = cd_play_fad(0, FAD, num_sectors)) != 0)
      return ret;

   while (done != 1)
   {
      u32 sectorstoread=0;
      u32 bytes_to_read;

      // Wait until there's data ready
      while ((sectorstoread = cd_is_data_ready(0)) == IAPETUS_ERR_OK) {}

      if ((int)(sectorstoread * sector_size_tbl[cd_sector_size]) > num_bytes)
         bytes_to_read = num_bytes;
      else
         bytes_to_read = sectorstoread * sector_size_tbl[cd_sector_size];

      // Setup a transfer from cd buffer to wram, then delete data
      // from cd buffer
      if ((ret = cd_transfer_data_bytes(bytes_to_read, buffer)) != IAPETUS_ERR_OK)
      {
         return ret;
      }

      num_bytes -= bytes_to_read;
      buffer += bytes_to_read;
      if (num_bytes <= 0)
         done = 1;
   }

   return IAPETUS_ERR_OK;
}

// //////////////////////////////////////////////////////////////////////////////

// int play_cd_audio(u8 audio_track, u8 repeat, u8 vol_l, u8 vol_r)
// {
//    cd_cmd_struct cd_cmd;
//    cd_cmd_struct cd_cmd_rs;
//    int ret;

//    sound_external_audio_enable(vol_l, vol_r);

//    if ((ret = cd_set_sector_size(SECT_2048)) != 0)
//       return ret;

//    // Clear partition 0
//    if ((ret = cd_reset_selector_one(0)) != 0)
//       return ret;

//    // Connect CD device to filter 0
//    if ((ret = cd_connect_cd_to_filter(0)) != 0)
//       return ret;

//    // Clear flags
//    CDB_REG_HIRQ = ~(HIRQ_PEND|HIRQ_CSCT) | HIRQ_CMOK;

//    cd_cmd.CR1 = 0x1000;
//    cd_cmd.CR2 = (audio_track << 8) | 0x01;
//    cd_cmd.CR3 = repeat << 8;
//    cd_cmd.CR4 = (audio_track << 8) | 0x63;

//    ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs);

//    return ret;
// }

// //////////////////////////////////////////////////////////////////////////////

// int stop_cd_audio(void)
// {
//    cd_cmd_struct cd_cmd;
//    cd_cmd_struct cd_cmd_rs;

//    // Do a default seek, that should stop the cd from playing
//    cd_cmd.CR1 = 0x11FF;
//    cd_cmd.CR2 = 0xFFFF;
//    cd_cmd.CR3 = 0xFFFF;
//    cd_cmd.CR4 = 0xFFFF;

//    return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
// }

// //////////////////////////////////////////////////////////////////////////////