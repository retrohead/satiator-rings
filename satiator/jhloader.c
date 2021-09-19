/*  Copyright (c) 2015 James Laird-Wah
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/. */

#include <jo/jo.h>
#include <string.h>
#include <stdint.h>
#include "iapetus/iapetus.h"
#include "jhloader.h"
#include "../bios.h"
#include "../debug.h"
#include "disc_format/cdparse.h"
#include "../sci.h"

#if !BIOS_BOOT
static int emulate_bios_loadcd_init(void)
{
   sci_init();
   *(uint32_t*)0x6000278 = 0;
   *(uint32_t*)0x600027c = 0;
   int ret;
   
   sciLog("cd_abort_file\r\n");
   if((ret = cd_abort_file()) != IAPETUS_ERR_OK)
      return ret;
   sciLog("cd_end_transfer\r\n");
   if((ret = cd_end_transfer()) != IAPETUS_ERR_OK)
      return ret;
   sciLog("cd_reset_selector_all\r\n");
   if((ret = cd_reset_selector_all()) != IAPETUS_ERR_OK)
      return ret;
   sciLog("cd_set_sector_size\r\n");
   if((ret = cd_set_sector_size(SECT_2048)) != IAPETUS_ERR_OK)
      return ret;
   sciLog("cd_auth\r\n");
   if((ret = cd_auth()) != IAPETUS_ERR_OK)   // gotta make sure that's a real disc thar
      return ret;
   return 0;
}

static int set_image_region(u8 *base)
{
   const char *str = getRegionString();
   if (!str)
      return -1;
   // set region header
   memset(base + 0x40, ' ', 0x10);
   base[0x40] = str[0];
   // set region footer
   char *ptr = (char*)base + 0xe00;
   memset(ptr, ' ', 0x20);
   *(uint32_t*)ptr = 0xa00e0009;
   strcpy(ptr + 4, "For ");
   strcpy(ptr + 8, str);
   char *end = ptr + 4 + strlen(ptr + 4);
   *end = '.';
   return 0;
}

static int emulate_bios_loadcd_read(void)
{
   int ret, i;
   // it doesn matter where
   u8 *ptr = (u8 *)jo_malloc_with_behaviour(2048*16, JO_MALLOC_TRY_REUSE_BLOCK);
   if (ptr == JO_NULL)
   {
      sciLog("out of memory!\r\n");
      return -1;
   }
   sciLog("cd_read_sector\r\n");
   ret = cd_read_sector(ptr, 150, SECT_2048, 2048*16);
   if (ret < 0)
      return ret;

   sciLog("set_image_region\r\n");
   ret = set_image_region(ptr);
   if (ret < 0)
      return ret;
   sciLog("cd_put_sector_data\r\n");
   ret = cd_put_sector_data(0, 16);
   if (ret < 0)
      return ret;
   while (!(CDB_REG_HIRQ & HIRQ_DRDY)) {}

   for (i = 0; i < 2048 * 16; i+=4)
   {
      CDB_REG_DATATRNS = *(unsigned long*)(ptr + i);
   }
   sciLog("cd_end_transfer\r\n");
   if ((ret = cd_end_transfer()) != 0)
      return ret;
   while (!(CDB_REG_HIRQ & HIRQ_EHST)) {}

   sciLog("%s", (char *)ptr);
   sciLog("applying flags\r\n");
   // we need to set a flag, but it's in different places in different BIOS versions
   uint16_t *read_flag_a = (uint16_t*)0x06000380;
   uint16_t *read_flag_b = (uint16_t*)0x060003a0;
   uint16_t **read_flag_ptr_a = (uint16_t**)0x2174;
   uint16_t **read_flag_ptr_b = (uint16_t**)0x2a04;

   if (read_flag_a == *read_flag_ptr_a)
   {
      *read_flag_a = 1;
      sciLog("bios type A\r\n");
   }
   else if (read_flag_b == *read_flag_ptr_b)
   {
      *read_flag_b = 1;
      sciLog("bios type B\r\n");
   }
   else
   {
      sciLog("bios not recognised\r\n");
      return BOOT_UNRECOGNISED_BIOS;
   }
   return 0;
}

void soft_reset()
{
    unsigned short *p;
    int i;

    /* Don't forget to stop pad interrupts. */
    //pad_shutdown();

    //if(softres_flags & SC_SOFTRES_VDP2RAM)
    //{
        /* Reset VDP2 VRAM/CRAM. */
        memset((void*)0x25E00000, 0, 512*1024);
        memset((void*)0x25F00000, 0,   4*1024);
    //}

    //if(softres_flags & SC_SOFTRES_VDP2REGS)
    //{
        /* Reset VDP2 regs.
         * Note : registers not modified by JoEngine are commented out.
         */
        VDP2_REG_TVMD   = 0x8000;
        VDP2_REG_EXTEN  = 0x0000;//
        VDP2_REG_TVSTAT = 0x0202;//
        VDP2_REG_VRSIZE = 0x0000;//
        VDP2_REG_HCNT   = 0x0000;//
        VDP2_REG_VCNT   = 0x00F1;//
        VDP2_REG_RAMCTL = 0x0000;
        VDP2_REG_CYCA0L = 0x0F44;
        VDP2_REG_CYCA0U = 0xFFFF;
        VDP2_REG_CYCA1L = 0xFFFF;
        VDP2_REG_CYCA1U = 0xFFFF;
        VDP2_REG_CYCB0L = 0xFFFF;
        VDP2_REG_CYCB0U = 0xFFFF;
        VDP2_REG_CYCB1L = 0xFFFF;
        VDP2_REG_CYCB1U = 0xFFFF;
        VDP2_REG_BGON   = 0x0001;
        VDP2_REG_MZCTL  = 0x0000;//
        VDP2_REG_SFSEL  = 0x0000;//
        VDP2_REG_SFCODE = 0x0000;//
        VDP2_REG_CHCTLA = 0x0000;
        VDP2_REG_CHCTLB = 0x0000;
        VDP2_REG_BMPNA  = 0x0000;//
        VDP2_REG_BMPNB  = 0x0000;//
        VDP2_REG_PNCN0  = 0x8000;
        VDP2_REG_PNCN1  = 0x0000;
        VDP2_REG_PNCN2  = 0x0000;
        VDP2_REG_PNCN3  = 0x0000;
        VDP2_REG_PNCR   = 0x0000;
        VDP2_REG_PLSZ   = 0x0000;//
        VDP2_REG_MPOFN  = 0x0000;
        VDP2_REG_MPOFR  = 0x0000;//
        VDP2_REG_MPABN0 = 0x0303;
        VDP2_REG_MPCDN0 = 0x0303;
        VDP2_REG_MPABN1 = 0x0000;
        VDP2_REG_MPCDN1 = 0x0000;
        VDP2_REG_MPABN2 = 0x0000;//
        VDP2_REG_MPCDN2 = 0x0000;//
        VDP2_REG_MPABN3 = 0x0000;//
        VDP2_REG_MPCDN3 = 0x0000;//
        VDP2_REG_MPABRA = 0x0000;
        VDP2_REG_MPCDRA = 0x0000;
        VDP2_REG_MPEFRA = 0x0000;
        VDP2_REG_MPGHRA = 0x0000;
        VDP2_REG_MPIJRA = 0x0000;
        VDP2_REG_MPKLRA = 0x0000;
        VDP2_REG_MPMNRA = 0x0000;
        VDP2_REG_MPOPRA = 0x0000;
        VDP2_REG_MPABRB = 0x0000;
        VDP2_REG_MPCDRB = 0x0000;
        VDP2_REG_MPEFRB = 0x0000;
        VDP2_REG_MPGHRB = 0x0000;
        VDP2_REG_MPIJRB = 0x0000;
        VDP2_REG_MPKLRB = 0x0000;
        VDP2_REG_MPMNRB = 0x0000;
        VDP2_REG_MPOPRB = 0x0000;
        VDP2_REG_SCXIN0 = 0x0000;
        VDP2_REG_SCXDN0 = 0x0000;
        VDP2_REG_SCYIN0 = 0x0000;
        VDP2_REG_SCYDN0 = 0x0000;//
        VDP2_REG_ZMXIN0  = 0x00010000;
        VDP2_REG_ZMYIN0  = 0x00010000;
        VDP2_REG_SCXIN1 = 0x0000;//
        VDP2_REG_SCXDN1 = 0x0000;//
        VDP2_REG_SCYIN1 = 0x0000;//
        VDP2_REG_SCYDN1 = 0x0000;//
        VDP2_REG_ZMXIN1  = 0x00010000;
        VDP2_REG_ZMYIN1  = 0x00010000;
        VDP2_REG_SCXN2  = 0x0000;//
        VDP2_REG_SCYN2  = 0x0000;//
        VDP2_REG_SCXN3  = 0x0000;//
        VDP2_REG_SCYN3  = 0x0000;//
        VDP2_REG_ZMCTL  = 0x0000;//
        VDP2_REG_SCRCTL = 0x0000;//
        VDP2_REG_VCSTAU  = 0x00000000;//
        VDP2_REG_LSTA0U  = 0x00000000;//
        VDP2_REG_LSTA1U  = 0x00000000;//
        VDP2_REG_LCTAU   = 0x00000000;//
        JO_VDP2_BKTAU  = 0x0000;
        JO_VDP2_BKTAL  = 0x4000;
        VDP2_REG_RPMD   = 0x0000;//
        VDP2_REG_RPRCTL = 0x0000;//
        VDP2_REG_KTCTL  = 0x0000;//
        VDP2_REG_KTAOF  = 0x0000;//
        VDP2_REG_OVPNRA = 0x0000;//
        VDP2_REG_OVPNRB = 0x0000;//
        VDP2_REG_RPTAU   = 0x0;
        VDP2_REG_RPTAL   = 0x0000;
        VDP2_REG_WPSX0  = 0x0000;//
        VDP2_REG_WPSY0  = 0x0000;//
        VDP2_REG_WPEX0  = 0x0000;//
        VDP2_REG_WPEY0  = 0x0000;//
        VDP2_REG_WPSX1  = 0x0000;//
        VDP2_REG_WPSY1  = 0x0000;//
        VDP2_REG_WPEX1  = 0x0000;
        VDP2_REG_WPEY1  = 0x0000;
        VDP2_REG_WCTLA  = 0x0000;//
        VDP2_REG_WCTLB  = 0x0000;//
        VDP2_REG_WCTLC  = 0x0000;//
        VDP2_REG_WCTLD  = 0x0000;//
        VDP2_REG_LWTA0L  = 0x00000000;//
        VDP2_REG_LWTA1L  = 0x00000000;//
        VDP2_REG_SPCTL  = 0x0020;
        VDP2_REG_SDCTL  = 0x0000;//
        VDP2_REG_CRAOFA = 0x0000;//
        VDP2_REG_CRAOFB = 0x0000;//
        VDP2_REG_LNCLEN = 0x0000;//
        VDP2_REG_SFPRMD = 0x0000;//
        VDP2_REG_CCCTL  = 0x0000;//
        VDP2_REG_SFCCMD = 0x0000;//
        VDP2_REG_PRISA  = 0x0000;
        VDP2_REG_PRISB  = 0x0000;
        VDP2_REG_PRISC  = 0x0000;
        VDP2_REG_PRISD  = 0x0000;
        VDP2_REG_PRINA  = 0x0007;
        VDP2_REG_PRINB  = 0x0000;
        VDP2_REG_PRIR   = 0x0000;
        VDP2_REG_CCRSA  = 0x0000;//
        VDP2_REG_CCRSB  = 0x0000;//
        VDP2_REG_CCRSC  = 0x0000;//
        VDP2_REG_CCRSD  = 0x0000;//
        VDP2_REG_CCRNA  = 0x0000;//
        VDP2_REG_CCRNB  = 0x0000;//
        VDP2_REG_CCRR   = 0x0000;//
        VDP2_REG_CCRLB  = 0x0000;//
        VDP2_REG_CLOFEN = 0x0001;//
        VDP2_REG_CLOFSL = 0x0000;//
        VDP2_REG_COAR   = 0x0200;//
        VDP2_REG_COAG   = 0x0200;//
        VDP2_REG_COAB   = 0x0200;//
        VDP2_REG_COBR   = 0x0000;//
        VDP2_REG_COBG   = 0x0000;//
        VDP2_REG_COBB   = 0x0000;//
    //}

    //if(softres_flags & SC_SOFTRES_VDP1REGS)
    //{
        /* Reset VDP1 regs.
         * Note : registers not modified by Pseudo Saturn Kai are commented out.
         */
        VDP1_REG_TVMR = 0x0000;
        VDP1_REG_FBCR = 0x0000;
        VDP1_REG_PTMR = 0x0000;
        VDP1_REG_EWDR = 0x0000;
        VDP1_REG_EWLR = 0x0000;
        VDP1_REG_EWRR = 0x0000;
        VDP1_REG_EDSR = 0x0;
        VDP1_REG_ENDR = 0x0000;//
    //}

    //if(softres_flags & SC_SOFTRES_VDP1RAM)
    //{
        /* Set first command to END, then force end of VDP1 drawing.
         * (= write to "Draw Forced Termination Register")
         */
        p = (unsigned short*)0x25c00000;
        *p = 0x8000;
        p = (unsigned short*)(0x25c00000+0x100000+0x0C);
        *p = 0x0000;

        /* Reset all VDP1 commands table / VRAM. */
        p = (unsigned short*)0x25c00000;
        for(i=0; i<((512*1024)/sizeof(unsigned short)); i++)
        {
            //*p++ = _vdp1_cmd[i&0xF];
            *p++ = ((i&0xF) == 0 ? 0x8000 : 0x0000);
        }

        /* Reset VDP1 frame buffer. */
        memset((void*)0x25C80000, 0, 256*1024);
    //}
}
#endif

int boot_disc(void)
{
   int ret;
#if BIOS_BOOT
   // authentic boot
   ret = bios_loadcd_init();
   ret = bios_loadcd_read();
#else
   // region free boot
   if ((ret = emulate_bios_loadcd_init()) < 0)
   {
      cdparse_set_error("emulate_bios_loadcd_init %d", ret);
      return ret;
   }
   if ((ret = emulate_bios_loadcd_read()) < 0)
   {
      cdparse_set_error("emulate_bios_loadcd_read %d", ret);
      return ret;
   }
   init_sprites();
   init_background();
   soft_reset();
#endif

   do {
      sciLog("BOOTING DISC\r\n");
       ret = bios_loadcd_boot();
       vdp_wait_vblankin();
       vdp_wait_vblankout();
   } while (ret == 1);
      cdparse_set_error("bios_loadcd_boot %d", ret);
   return ret;
}
