/*  Copyright 2009,2013 Theo Berkau

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

#ifndef ARCOMM_H
#define ARCOMM_H
	
struct fi_struct
{
	char *name;
	u32 pid;
	int page_size; // in words
	int num_pages; 
	int rom_size; // in words
	int needs_page_erase;
	void (*erase_flash_all)(struct fi_struct *flash_info);
	void (*erase_flash)(struct fi_struct *flash_info, volatile u16 *page, int num_pages);
	void (*write_flash)(struct fi_struct *flash_info, volatile u16 *page, u16 *data, int num_pages);
};

typedef struct fi_struct flash_info_struct;

void arcl_init_handler(int vector, u32 patchaddr, u16 patchinst, u32 codeaddr);

void ar_get_product_id(u16 *vendorid, u16 *deviceid);
int ar_init_flash_io(flash_info_struct *flash_info);
void ar_erase_flash_all(flash_info_struct *flash_info);
void ar_erase_flash(flash_info_struct *flash_info, volatile u16 *page, int num_pages);
void ar_write_flash(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages);
int ar_verify_write_flash(flash_info_struct *flash_info, volatile u16 *page, u16 *data, int num_pages);
#endif
