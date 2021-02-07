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

#include "../iapetus.h"

//////////////////////////////////////////////////////////////////////////////

int mpeg_exec_command(u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs)
{
   if (!(CDB_REG_HIRQ & HIRQ_MPCM))
      return IAPETUS_ERR_MPEGCMD;

   return cd_exec_command(hirq_mask, cd_cmd, cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_debug_exec_command(font_struct *font, u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs)
{
   if (!(CDB_REG_HIRQ & HIRQ_MPCM))
      return IAPETUS_ERR_MPEGCMD;

   return cd_debug_exec_command(font, hirq_mask, cd_cmd, cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

BOOL is_mpeg_card_present()
{
   hw_info_struct hw_info;

   if (cd_get_hw_info(&hw_info) != IAPETUS_ERR_OK)
      return FALSE;
   
   // Is MPEG card available?   
   if ((hw_info.hw_flag & 0x02) == 0)
   	   return FALSE;
   
   // Has MPEG card been authenticated?
   if (hw_info.mpeg_ver == 0)
   {
       if (bios_is_mpeg_card_present(1) != 0)
       {
          if (bios_is_mpeg_card_present(1) != 0)
          	  return FALSE;
       }
   }
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_get_status(mpeg_status_struct *mpeg_status)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   // Authenticate disc
   cd_cmd.CR1 = 0x9000;
   cd_cmd.CR2 = 0x0001;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_OK)
      return ret;

   mpeg_status->status = cd_cmd_rs.CR1 >> 8;
   mpeg_status->play_status = cd_cmd_rs.CR1 & 0xFF;
   mpeg_status->v_counter = cd_cmd_rs.CR2;
   mpeg_status->picture_info = cd_cmd_rs.CR3 >> 8;
   mpeg_status->mpeg_audio_status = cd_cmd_rs.CR3 & 0xFF;
   mpeg_status->mpeg_video_status = cd_cmd_rs.CR4;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_interrupt_mask(u32 mask)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x9200 | (mask >> 16);
   cd_cmd.CR2 = mask;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_init ()
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   screen_settings_struct settings;

   // Make sure MPEG card is present and authenticated
   if (!is_mpeg_card_present())
      return IAPETUS_ERR_AUTH;

   // Now Initialize MPEG card
   cd_cmd.CR1 = 0x9300;
   cd_cmd.CR2 = 0x0001;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   if ((ret = cd_exec_command(HIRQ_MPED, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_OK)
      return ret;

   // Have to wait a bit if we're using software timer
   if (!cd_wait_hirq(HIRQ_MPCM))
      return IAPETUS_ERR_MPEGCMD;

   if (!cd_wait_hirq(HIRQ_MPCM))
      return IAPETUS_ERR_MPEGCMD;

   // Setup MPEG mode
   if ((ret = mpeg_set_mode(SMVM_NOCHANGE, SMDT_NOCHANGE, SMOM_NOCHANGE, SMSM_NTSC_I)) != IAPETUS_ERR_OK)
      return ret;

   // Enable the external audio through SCSP
   sound_external_audio_enable(7, 7);

   // Setup NBG1 as EXBG
   settings.is_bitmap = TRUE;
   settings.bitmap_size = BG_BITMAP512x256;
   settings.transparent_bit = 0;
   settings.color = BG_32786COLOR;
   settings.special_priority = 0;
   settings.special_color_calc = 0;
   settings.extra_palette_num = 0;
   settings.map_offset = 0;

   return vdp_exbg_init(&settings);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_con(enum STM_SEL stm_sel, mpeg_con_struct *mpeg_con_audio, mpeg_con_struct *mpeg_con_video)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x9A00 | mpeg_con_audio->con_mode;
   cd_cmd.CR2 = (mpeg_con_audio->lay_search << 8) | mpeg_con_audio->buf_num;
   cd_cmd.CR3 = (stm_sel << 8) | mpeg_con_video->con_mode;
   cd_cmd.CR4 = (mpeg_con_video->lay_search << 8) | mpeg_con_video->buf_num;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_get_con(enum STM_SEL stm_sel, mpeg_con_struct *mpeg_con_audio, mpeg_con_struct *mpeg_con_video)
{
   int ret;
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x9B00;
   cd_cmd.CR2 = 0x0000;
   cd_cmd.CR3 = (stm_sel << 8);
   cd_cmd.CR4 = 0x0000;

   if ((ret = mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_OK)
      return ret;

   mpeg_con_audio->con_mode = cd_cmd.CR1;
   mpeg_con_audio->lay_search = cd_cmd.CR2 >> 8;
   mpeg_con_audio->buf_num = cd_cmd.CR2;
   mpeg_con_video->con_mode = cd_cmd.CR3;
   mpeg_con_video->lay_search = cd_cmd.CR4 >> 8;
   mpeg_con_video->buf_num = cd_cmd.CR4;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_mode(enum SMVM video_mode, enum SMDT dec_timing_mode, enum SMOM out_mode, enum SMSM scanline_mode)
{
   int i;

   for(i = 0; i < 50; i++)
   {
      int ret;
      cd_cmd_struct cd_cmd;
      cd_cmd_struct cd_cmd_rs;

      // Authenticate disc
      cd_cmd.CR1 = 0x9400 | (video_mode & 0xFF);
      cd_cmd.CR2 = (dec_timing_mode << 8) | (out_mode & 0xFF);
      cd_cmd.CR3 = scanline_mode << 8;
      cd_cmd.CR4 = 0x0000;

      if ((ret = mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_BUSY)
         return ret;

      if (ret == IAPETUS_ERR_OK)
         return ret;
   }

   return IAPETUS_ERR_BUSY;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_stream(enum STM_SEL stm_sel, mpeg_stream_struct *mpeg_stream_audio, mpeg_stream_struct *mpeg_stream_video)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x9D00 | (mpeg_stream_audio->mode & 0xFF);
   cd_cmd.CR2 = (mpeg_stream_audio->id << 8) | (mpeg_stream_audio->cn & 0xFF);
   cd_cmd.CR3 = (stm_sel << 8) | (mpeg_stream_video->mode & 0xFF);
   cd_cmd.CR4 = (mpeg_stream_video->id << 8) | (mpeg_stream_video->cn & 0xFF);

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_decoding(u8 mute, u16 pause_time, u16 freeze_time)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0x9600 | (mute & 0xFF);
   cd_cmd.CR2 = pause_time;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = freeze_time;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_display(BOOL disp_show, u8 fb_num)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xA000;
   cd_cmd.CR2 = (disp_show << 8) | (fb_num & 0xFF);
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_window(enum SWCT type, s16 x, s16 y)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xA100 | (type & 0xFF);
   cd_cmd.CR2 = TRUE;
   cd_cmd.CR3 = x;
   cd_cmd.CR4 = y;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_border_color(u16 color)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xA200;
   cd_cmd.CR2 = color;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_fade(u8 y_gain, u8 c_gain)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xA300;
   cd_cmd.CR2 = (y_gain << 8) | (c_gain & 0xFF);
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = 0x0000;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_set_video_effects(u8 intp, u8 lumikey, u8 mosaic_width, u8 mosaic_height, u8 blur_width, u8 blur_height)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;

   cd_cmd.CR1 = 0xA400;
   cd_cmd.CR2 = (intp << 8) | lumikey;
   cd_cmd.CR3 = (mosaic_width << 8) | mosaic_height;
   cd_cmd.CR4 = (blur_width << 8) | blur_height;

   return mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_play(file_struct *file)
{
   cd_cmd_struct cd_cmd;
   cd_cmd_struct cd_cmd_rs;
   int ret;
   cd_sh_cond_struct cd_sh_cond;
   cd_range_struct cd_range;
   cd_con_struct cd_con;
   mpeg_con_struct mpeg_con_audio;
   mpeg_con_struct mpeg_con_video;
   mpeg_stream_struct mpeg_stream_audio;
   mpeg_stream_struct mpeg_stream_video;

   // Setup CD filters
   cd_range.fad = file->lba+150;
   cd_range.range = file->size / 2048;

   cd_sh_cond.channel = 0;
   cd_sh_cond.file_id = 0;
   cd_sh_cond.ci_val = 0;
   cd_sh_cond.ci_mask = 0;
   cd_sh_cond.sm_val = SM_VIDEO;
   cd_sh_cond.sm_mask = SM_VIDEO;

   cd_con.connect_flags = CD_CON_TRUE | CD_CON_FALSE;
   cd_con.true_con = 0;
   cd_con.false_con = 1;

   if ((ret = cd_set_filter(0, FM_FAD | FM_FN | FM_SM, &cd_sh_cond, &cd_range, &cd_con)) != IAPETUS_ERR_OK)
      return ret;

   cd_sh_cond.channel = 0;
   cd_sh_cond.file_id = 0;
   cd_sh_cond.ci_val = 0;
   cd_sh_cond.ci_mask = 0;
   cd_sh_cond.sm_val = SM_AUDIO;
   cd_sh_cond.sm_mask = SM_AUDIO;

   cd_con.connect_flags = CD_CON_TRUE | CD_CON_FALSE;
   cd_con.true_con = 1;
   cd_con.false_con = 0xFF;

   if ((ret = cd_set_filter(1, FM_FAD | FM_FN | FM_SM, &cd_sh_cond, &cd_range, &cd_con)) != IAPETUS_ERR_OK)
      return ret;

   if ((ret = mpeg_set_window(SWCT_FB_WIN_POS, 22, 0)) != IAPETUS_ERR_OK)
      return ret;
   if ((ret = mpeg_set_window(SWCT_FB_WIN_ZOOM, 15, 1)) != IAPETUS_ERR_OK)
      return ret;
   if ((ret = mpeg_set_window(SWCT_DISP_WIN_POS, 0, 8)) != IAPETUS_ERR_OK)
      return ret;
   if ((ret = mpeg_set_window(SWCT_DISP_WIN_SIZE, 320, 224)) != IAPETUS_ERR_OK)
      return ret;

   if ((ret = mpeg_set_border_color(0)) != IAPETUS_ERR_OK)
      return ret;
   if ((ret = mpeg_set_fade(0, 0)) != IAPETUS_ERR_OK)
      return ret;
   if ((ret = mpeg_set_video_effects(SVE_INTP_ALL, 0, 0, 0, 0, 0)) != IAPETUS_ERR_OK)
      return ret;

   if ((ret = mpeg_display(TRUE, 0)) != IAPETUS_ERR_OK)
      return ret;

   if ((ret = mpeg_set_mode(MPEG_NO_CHANGE, SMDT_VSYNC, SMOM_VDP2, MPEG_NO_CHANGE)) != IAPETUS_ERR_OK)
      return ret;

   if ((ret = mpeg_set_decoding(SD_UNMUTE, SD_PAUSE_OFF, SD_FREEZE_OFF)) != IAPETUS_ERR_OK)
      return ret;

   mpeg_stream_audio.mode = SSM_NONE;
   mpeg_stream_audio.id = 0;
   mpeg_stream_audio.cn = 0;

   mpeg_stream_video.mode = SSM_NONE;
   mpeg_stream_video.id = 0;
   mpeg_stream_video.cn = 0;
   if ((ret = mpeg_set_stream(STM_SEL_CURRENT, &mpeg_stream_audio, &mpeg_stream_video)) != IAPETUS_ERR_OK)
      return ret;

   // Setup MPEG connections
   mpeg_con_audio.con_mode = SCCM_SECTOR_DEL | SCCM_SYSTEM_END;
   mpeg_con_audio.lay_search = SCLS_SYSTEM;
   mpeg_con_audio.buf_num = 1;
   mpeg_con_video.con_mode = SCCM_CLEAR_VBV_WBC | SCCM_SECTOR_DEL | SCCM_SYSTEM_END;
   mpeg_con_video.lay_search = SCLS_SYSTEM | SCLS_AVSEARCH;
   mpeg_con_video.buf_num = 0;
   if ((ret = mpeg_set_con(STM_SEL_CURRENT, &mpeg_con_audio, &mpeg_con_video)) != IAPETUS_ERR_OK)
      return ret;

   // Start CD transfer
   if ((ret = cd_play_fad(0, file->lba+150, file->size / 2048)) != IAPETUS_ERR_OK)
      return ret;

   // Clear hirq flags
   CDB_REG_HIRQ = ~(HIRQ_MPED);

   // Start MPEG decoding
   cd_cmd.CR1 = 0x9500 | MPPM_SYNC;
   cd_cmd.CR2 = (MPTM_NO_CHANGE << 8) | MPTM_AUTO;
   cd_cmd.CR3 = 0x0000;
   cd_cmd.CR4 = MPCS_NO_CHANGE & 0xFF;

   if ((ret = mpeg_exec_command(0, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_OK)
      return ret;

   return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_pause(file_struct *file)
{
   return mpeg_set_decoding(MPEG_NO_CHANGE, SD_PAUSE_ON, MPEG_NO_CHANGE);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_unpause(file_struct *file)
{
   return mpeg_set_decoding(MPEG_NO_CHANGE, SD_PAUSE_OFF, MPEG_NO_CHANGE);
}

//////////////////////////////////////////////////////////////////////////////

int mpeg_stop(file_struct *file)
{
   return cd_seek_fad(0);
}

//////////////////////////////////////////////////////////////////////////////
