/*  Copyright 2009, 2013-2014 Theo Berkau
    
    Flash code based on code by Ex-Cyber, antime

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

#define AR_5555         *((volatile u16 *)0x2200AAAA)
#define AR_AAAA         *((volatile u16 *)0x22005554)
#define AR_VENDOR       *((volatile u16 *)0x22000000)
#define AR_DEVICE       *((volatile u16 *)0x22000002)

#define AR_FLASH_ADDR   ((volatile u16 *)0x22000000)

#define CMD_PID_ENTRY   0x9090
#define CMD_PID_EXIT    0xF0F0
#define CMD_PAGE_WRITE  0xA0A0

void ar_erase_flash_all_SST29EE010(flash_info_struct *flash_info);
void ar_erase_flash_SST29EE010(flash_info_struct *flash_info, volatile u16 *page, int num_pages);
void ar_write_flash_SST29EE010(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages);
void ar_erase_flash_all_AM29F010B(flash_info_struct *flash_info);
void ar_erase_flash_AM29F010B(flash_info_struct *flash_info, volatile u16 *page, int num_pages);
void ar_write_flash_AM29F010B(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages);
int ar_get_product_index(u16 vendorid, u16 deviceid);

static flash_info_struct flash_info_list[] = {
   { "Silicon Storage Technology SST29EE010", 0xBFBF0707, 128, 1024, 131072, 0, ar_erase_flash_all_AM29F010B, ar_erase_flash_SST29EE010, ar_write_flash_SST29EE010 },
	{ "Silicon Storage Technology SST29EE020", 0xBFBF1010, 128, 2048, 262144, 0, ar_erase_flash_all_AM29F010B, ar_erase_flash_SST29EE010, ar_write_flash_SST29EE010 },
	{ "Silicon Storage Technology SST29LE020", 0xBFBF1212, 128, 2048, 262144, 0, ar_erase_flash_all_AM29F010B, ar_erase_flash_SST29EE010, ar_write_flash_SST29EE010 },
	{ "Silicon Storage Technology SST39SF010A", 0xBFBFB5B5, 4096, 32, 131072, 1, ar_erase_flash_all_AM29F010B, ar_erase_flash_AM29F010B, ar_write_flash_AM29F010B },
	{ "Silicon Storage Technology SST39SF020" , 0xBFBFB6B6, 4096, 32, 262144, 1, ar_erase_flash_all_AM29F010B, ar_erase_flash_AM29F010B, ar_write_flash_AM29F010B },
	{ "Silicon Storage Technology SST39SF040" , 0xBFBFB7B7, 4096, 32, 524288, 1, ar_erase_flash_all_AM29F010B, ar_erase_flash_AM29F010B, ar_write_flash_AM29F010B },
   { "Atmel AT29C010", 0x1F1FD5D5, 128, 1024, 131072, 0, ar_erase_flash_all_SST29EE010, ar_erase_flash_SST29EE010, ar_write_flash_SST29EE010 },
	{ "AMD AM29F010B", 0x01012020, 16384, 8, 131072, 1, ar_erase_flash_all_AM29F010B, ar_erase_flash_AM29F010B, ar_write_flash_AM29F010B },
};

static int num_supported_flash=sizeof(flash_info_list)/sizeof(flash_info_struct);

void arcl_init_handler(int vector, u32 patch_addr, u16 patch_inst, u32 code_addr)
{
   int i;

   // Copy over AR handler from 0x02003024-0x02003147 to codeaddr+8
   for (i = 0; i < 292; i+=4)
       *((u32 *)(code_addr+8+i)) = *((u32 *)(0x02003024+i));
   
   // Patch code address+8 so the bra call is correct(fix me)
   *((u16 *)(code_addr+0x8)) = *((u16 *)(code_addr+0xA));
   *((u16 *)(code_addr+0xA)) = 0x0009;

   // Patch interrupt with AR handler
   *((u32 *)(0x06000000 + (vector << 2))) = code_addr + 0x8;

   // Read patch address, write it to code address+0xC
   *((u16 *)(code_addr+0xC)) = *((u16 *)patch_addr);

   // Write patch instruction to patch address
   *((u16 *)patch_addr) = patch_inst;

   // Write a dummy code(not sure if this is needed)
   *((u32 *)(code_addr)) = 0x06000000;
   *((u32 *)(code_addr+4)) = 0x00000000; // flag to keep handler from running multiple times at the same time
   *((u32 *)(code_addr+0x11C)) = code_addr;   
}

void ar_command(u16 cmd)
{
  // Unlock Flash
  AR_5555 = 0xAAAA;
  AR_AAAA = 0x5555;
  // Write Command
  AR_5555 = cmd;
}

void ar_get_product_id(u16 *vendor_id, u16 *device_id)
{
	// All of the vdp_vsync() delays in this function are to support Atmel AT29C010, which asks a 10 ms delay.
	// It's easily confused by the SST command too, it seems.

	// Old SST devices only support this proprietary command.
	// New SST devices still support it.
	ar_command(0x8080);
	ar_command(0x6060);
	*vendor_id = AR_VENDOR;
	*device_id = AR_DEVICE;
	ar_command(CMD_PID_EXIT);

	if (*vendor_id == 0xbfbf)
		return;

	// JEDEC standard method
	ar_command(CMD_PID_ENTRY);
	*vendor_id = AR_VENDOR;
	*device_id = AR_DEVICE;
	ar_command(CMD_PID_EXIT);

	// some devices don't work with delays, so bail out if we already got a valid ID
	if (ar_get_product_index(*vendor_id, *device_id) >= 0)
		return;

	// JEDEC standard method, with delays. the AT29C010 datasheet specifies a 10 ms delay
	vdp_vsync();
	vdp_vsync();
	ar_command(CMD_PID_ENTRY);
	vdp_vsync();
	*vendor_id = AR_VENDOR;
	*device_id = AR_DEVICE;
	ar_command(CMD_PID_EXIT);
	vdp_vsync();
	vdp_vsync();
}

int ar_get_product_index(u16 vendorid, u16 deviceid)
{
	int i;
	u32 pid = (vendorid << 16) | deviceid;
	for (i = 0; i < num_supported_flash; i++)
	{
		if (flash_info_list[i].pid == pid)
			return i;
	}
	return -1;
}

int ar_init_flash_io(flash_info_struct *flash_info)
{
   u16 vendor_id, device_id;
   int i;

	if (flash_info_list == NULL)
		return IAPETUS_ERR_INVALIDARG;

   ar_get_product_id(&vendor_id, &device_id);

	i = ar_get_product_index(vendor_id, device_id);
   // Make sure vendor id and device id are supported
   if (i < 0)
   {
		memset(flash_info_list, 0, sizeof(flash_info_struct));

      if (vendor_id == 0xFFFF && device_id == 0xFFFF)
         return IAPETUS_ERR_HWNOTFOUND;
      else
		{
			if (vendor_id == 0x0101)
				flash_info->name = "Unknown AMD";
			else if (vendor_id == 0x1F1F)
				flash_info->name = "Unknown Atmel";
			else if (vendor_id == 0xBFBF)
				flash_info->name = "Unknown Silicon Storage Technology";
			else
				flash_info->name = "Unknown";
         return IAPETUS_ERR_UNSUPPORTED;
		}
   }
	else
	{
		memcpy(flash_info, flash_info_list+i, sizeof(flash_info_struct));
		return IAPETUS_ERR_OK;
	}
}

void ar_erase_flash_all_SST29EE010(flash_info_struct *flash_info)
{
	ar_erase_flash_SST29EE010(flash_info, AR_FLASH_ADDR, flash_info->rom_size/flash_info->page_size/sizeof(u16));
}

void ar_erase_flash_SST29EE010(flash_info_struct *flash_info, volatile u16 *page, int num_pages)
{
  int i,j;

  for (i = 0; i < num_pages; i++)
  {
     ar_command(CMD_PAGE_WRITE);
     for(j = 0; j < flash_info->page_size; j++)
     {
        page[0] = 0xFFFF;
        page++;
     }
     while (page[-1] != 0xffff)
         ;
  }
}

void ar_write_flash_SST29EE010(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages)
{
  int i,j;

  for (i = 0; i < num_pages; i++)
  {
     ar_command(CMD_PAGE_WRITE);
     for(j = 0; j < flash_info->page_size; j++)
     {
        page[0] = data[0];
        page++;
        data++;
     }
     while (page[-1] != data[-1])
         ;
  }
}

void ar_erase_flash_all_AM29F010B(flash_info_struct *flash_info)
{
	// Unlock Flash
	AR_5555 = 0xAAAA;
	AR_AAAA = 0x5555;
	// Erase Sequence
	AR_5555 = 0x8080;
	AR_5555 = 0xAAAA;
	AR_AAAA = 0x5555;
	AR_5555 = 0x1010;

	while(((*AR_FLASH_ADDR) & 0x8080) == 0) {}
}

void ar_erase_flash_AM29F010B(flash_info_struct *flash_info, volatile u16 *page, int num_pages)
{
	int i;
	for (i = 0; i < num_pages; i++)
	{
	   volatile u16 *buf;

		// Unlock Flash
		AR_5555 = 0xAAAA;
		AR_AAAA = 0x5555;
		// Erase Sequence
		AR_5555 = 0x8080;
		AR_5555 = 0xAAAA;
		AR_AAAA = 0x5555;

		buf = page + (i * flash_info->page_size);
		*buf = 0x3030;

		while (((*buf) & 0x8080) == 0) {}
	}
}

void ar_write_flash_AM29F010B(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages)
{
	int i, j;
	u16 tmp;

	for (i = 0; i < num_pages; i++)
	{
		for (j = 0; j < flash_info->page_size; j++)
		{
			tmp = data[0];
			ar_command(CMD_PAGE_WRITE);
			page[0] = tmp;
			while ((page[0] & 0x8080) != (tmp & 0x8080)) {}
			while ((page[0] & 0x8080) != (tmp & 0x8080)) {}
			while ((page[0] & 0x8080) != (tmp & 0x8080)) {}
			page++;
			data++;
		}
	}
}

void ar_erase_flash_all(flash_info_struct *flash_info)
{
	int old_mask=interrupt_get_level_mask();
	flash_info->erase_flash_all(flash_info);
	interrupt_set_level_mask(old_mask);
}

void ar_erase_flash(flash_info_struct *flash_info, volatile u16 *page, int num_pages)
{
	int old_mask=interrupt_get_level_mask();
	flash_info->erase_flash(flash_info, page, num_pages);
	interrupt_set_level_mask(old_mask);
}

void ar_write_flash(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages)
{
	int old_mask=interrupt_get_level_mask();
	flash_info->write_flash(flash_info, page, data, num_pages);
	interrupt_set_level_mask(old_mask);
}

int ar_verify_write_flash(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages)
{
   int i;

   for (i = 0; i < (num_pages * flash_info->page_size); i++)
   {
      if (page[i] != data[i])
         return FALSE;
   }
   return TRUE;
}
