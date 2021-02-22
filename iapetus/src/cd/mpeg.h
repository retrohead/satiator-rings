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

#ifndef MPEG_H
#define MPEG_H

typedef struct
{
   u8 status;
   u8 play_status;
   u16 v_counter;
   u8 picture_info;
   u8 mpeg_audio_status;
   u16 mpeg_video_status;
} mpeg_status_struct;

#define MS_PS_VIDEO_STOPPED   (1 << 0)
#define MS_PS_VIDEO_PREPING1  (1 << 1)
#define MS_PS_VIDEO_PREPING2  (3 << 0)
#define MS_PS_VIDEO_PLAYING   (1 << 2)
#define MS_PS_VIDEO_CHANGING  (5 << 0)
#define MS_PS_VIDEO_RECEIVING (6 << 0)
#define MS_PS_DEC_STOPPED     (1 << 0)

#define MS_PS_AUDIO_STOPPED   (1 << 4)
#define MS_PS_AUDIO_PREPING1  (1 << 5)
#define MS_PS_AUDIO_PREPING2  (3 << 4)
#define MS_PS_AUDIO_PLAYING   (1 << 6)
#define MS_PS_AUDIO_CHANGING  (5 << 4)
#define MS_PS_AUDIO_RECEIVING (6 << 4)

#define MS_AS_DECODE_OP       (1 << 0)
#define MS_AS_ILLEGAL_AUDIO   (1 << 3)
#define MS_AS_BUFFER_EMPTY    (1 << 4)
#define MS_AS_ERROR           (1 << 5)
#define MS_AS_LEFT_OUTPUT     (1 << 6)
#define MS_AS_RIGHT_OUTPUT    (1 << 7)

#define MS_VS_DECODE_OP       (1 << 0)
#define MS_VS_DISPLAYING      (1 << 1)
#define MS_VS_PAUSED          (1 << 2)
#define MS_VS_FROZEN          (1 << 3)
#define MS_VS_LAST_FRAME      (1 << 4)
#define MS_VS_ODD_FIELD       (1 << 5)
#define MS_VS_FRAME_UPDATE    (1 << 6)
#define MS_VS_ERROR           (1 << 7)
#define MS_VS_OUT_PREP_DONE   (1 << 8)
#define MS_VS_FIRST_FRAME     (1 << 9)
#define MS_VS_BUFFER_EMPTY    (1 << 10)

#define MPEG_NO_CHANGE -1

#define MPEG_INT_VSTM_READY      (1 << 0)
#define MPEG_INT_VSTM_CHANGE     (1 << 1)
#define MPEG_INT_VOUT_READY      (1 << 2)
#define MPEG_INT_VOUT_START      (1 << 3)
#define MPEG_INT_VDEC_ERROR      (1 << 4)
#define MPEG_INT_VSTM_ERROR      (1 << 5)
#define MPEG_INT_VBCON_ERROR     (1 << 6)
#define MPEG_INT_VSTM_DATA_ERROR (1 << 7)
#define MPEG_INT_PICT_START      (1 << 8)
#define MPEG_INT_GOP_START       (1 << 9)
#define MPEG_INT_SEQ_END         (1 << 10)
#define MPEG_INT_SEQ_START       (1 << 11)
#define MPEG_INT_VSECT_TRIGGER   (1 << 12)
#define MPEG_INT_VSECT_EOR       (1 << 13)
#define MPEG_INT_ASECT_TRIGGER   (1 << 14)
#define MPEG_INT_ASECT_EOR       (1 << 15)
#define MPEG_INT_ASTM_READY      (1 << 16)
#define MPEG_INT_ASTM_CHANGE     (1 << 17)
#define MPEG_INT_AOUT_READY      (1 << 18)
#define MPEG_INT_AOUT_START      (1 << 19)
#define MPEG_INT_ADEC_ERROR      (1 << 20)
#define MPEG_INT_ASTM_ERROR      (1 << 21)
#define MPEG_INT_ABCON_ERROR     (1 << 22)
#define MPEG_INT_ASTM_DATA_ERROR (1 << 23)

enum SMVM
{
   SMVM_MOVIE=0,
   SMVM_PICT=1,
   SMVM_HD_MOVIE=2,
   SMVM_HD_PICT=3,
   SMVM_NOCHANGE=-1
};

enum SMDT
{
   SMDT_VSYNC=0,
   SMDT_HOST=1,
   SMDT_NOCHANGE=-1
};

enum SMOM
{
   SMOM_VDP2=0,
   SMOM_HOST=1,
   SMOM_NOCHANGE=-1
};

enum SMSM
{
   SMSM_NTSC=0,
   SMSM_NTSC_I=1,
   SMSM_PAL=2,
   SMSM_PAL_I=3,
};

enum MPPM
{
   MPPM_SYNC=0,
   MPPM_ASYNC=1
};

enum MPTM
{
   MPTM_AUTO=0,
   MPTM_FORCED=1,
   MPTM_NO_CHANGE=-1
};

enum MPCS
{
   MPCS_ABORT=0,
   MPCS_CHANGE=1,
   MPCS_NO_CHANGE=-1
};

#define SD_MUTE_R                (1 << 0)
#define SD_MUTE_L                (1 << 1)
#define SD_UNMUTE                (1 << 2)
#define SD_PAUSE_ON              0
#define SD_PAUSE_OFF             1
#define SD_FREEZE_ON             0
#define SD_FREEZE_OFF            1

typedef struct  
{   
   u8 con_mode;
   u8 lay_search;
   u8 buf_num;
} mpeg_con_struct;

#define SCCM_EOR                 (1 << 0)
#define SCCM_SYSTEM_END          (1 << 1)
#define SCCM_SECTOR_DEL          (1 << 2)
#define SCCM_IGNORE_PTS          (1 << 3)
#define SCCM_CLEAR_VBV           (1 << 4)
#define SCCM_CLEAR_VBV_WBC       (1 << 5)

#define SCLS_SYSTEM              (0 << 0)
#define SCLS_VIDEO               (1 << 0)
#define SCLS_AUDIO               (1 << 0)

#define SCLS_VIDEOSEARCH         (1 << 7)
#define SCLS_AVSEARCH            (3 << 6)

typedef struct  
{
   u8 mode;
   u8 id;
   u8 cn;
} mpeg_stream_struct;

enum STM_SEL
{
   STM_SEL_CURRENT=0,
   STM_SEL_NEXT=1
};

#define SSM_NONE                 (0 << 0)
#define SSM_STREAM_ID_SET        (1 << 0)
#define SSM_STREAM_ID_IDENTIFY   (1 << 1)
#define SSM_CN_SET               (1 << 2)
#define SSM_CN_SET_IDENTIFY      (1 << 3)

enum SWCT
{
   SWCT_FB_WIN_POS=0,
   SWCT_FB_WIN_ZOOM=1,
   SWCT_DISP_WIN_POS=2,
   SWCT_DISP_WIN_SIZE=3,
   SWCT_DISP_WIN_OFFSET=4
};

#define SVE_INTP_YH              (1 << 0)
#define SVE_INTP_CH              (1 << 1)
#define SVE_INTP_YV              (1 << 2)
#define SVE_INTP_CV              (1 << 3)
#define SVE_INTP_ALL             (SVE_INTP_YH | SVE_INTP_CH | SVE_INTP_YV | SVE_INTP_CV)

int mpeg_exec_command(u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs);
int mpeg_debug_exec_command(font_struct *font, u16 hirq_mask, cd_cmd_struct *cd_cmd, cd_cmd_struct *cd_cmd_rs);
BOOL is_mpeg_card_present();
int is_mpeg_auth();
int mpeg_auth();
int mpeg_get_status(mpeg_status_struct *mpeg_status);
int mpeg_set_interrupt_mask(u32 mask);
int mpeg_init ();
int mpeg_set_con(enum STM_SEL stm_sel, mpeg_con_struct *mpeg_con_audio, mpeg_con_struct *mpeg_con_video);
int mpeg_get_con(enum STM_SEL stm_sel, mpeg_con_struct *mpeg_con_audio, mpeg_con_struct *mpeg_con_video);
int mpeg_set_mode(enum SMVM video_mode, enum SMDT dec_timing_mode, enum SMOM out_mode, enum SMSM scanline_mode);
int mpeg_set_stream(enum STM_SEL stm_sel, mpeg_stream_struct *mpeg_stream_audio, mpeg_stream_struct *mpeg_stream_video);
int mpeg_set_decoding(u8 mute, u16 pause_time, u16 freeze_time);
int mpeg_display(BOOL disp_show, u8 fb_num);
int mpeg_set_window(enum SWCT type, s16 x, s16 y);
int mpeg_set_border_color(u16 color);
int mpeg_set_fade(u8 y_gain, u8 c_gain);
int mpeg_set_video_effects(u8 intp, u8 lumikey, u8 mosaic_width, u8 mosaic_height, u8 blur_width, u8 blur_height);
int mpeg_play(file_struct *file);
int mpeg_pause(file_struct *file);
int mpeg_unpause(file_struct *file);
int mpeg_stop(file_struct *file);

#endif
