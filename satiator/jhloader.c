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
   cd_auth();   // gotta make sure that's a real disc thar
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
   char *ptr = (char *)jo_malloc_with_behaviour(2048*16, JO_MALLOC_TRY_REUSE_BLOCK);
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
   ret = cd_put_sector_data(0, 8);
   if (ret < 0)
      return ret;
   while (!(CDB_REG_HIRQ & HIRQ_DRDY)) {}

   for (i = 0; i < 2048 * 8; i+=4)
      CDB_REG_DATATRNS = *(uint32_t*)(ptr + i);
   sciLog("cd_end_transfer\r\n");
   if ((ret = cd_end_transfer()) != 0)
      return ret;
   while (!(CDB_REG_HIRQ & HIRQ_EHST)) {}

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
#endif
   do {
      sciLog("BOOTING DISC\r\n");
       ret = bios_loadcd_boot();
       vdp_wait_vblankin();
       vdp_wait_vblankout();
   } while (ret == 1);
   return ret;
}
