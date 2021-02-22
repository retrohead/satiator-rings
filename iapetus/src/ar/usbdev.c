/*  Copyright 2014 Theo Berkau
    
    Flash code based on code by cafe-alpha, who based his code on antime's

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
#include "crc.h"

#define USB_FLAGS (*(volatile u8 *)(0x22200001))
#define USB_RXF     (1 << 0)
#define USB_TXE     (1 << 1)
#define USB_PWREN   (1 << 7)
#define USB_FIFO (*(volatile u8 *)(0x22100001))

enum
{
	USBDC_FUNC_DOWNLOAD      = 1,
	USBDC_FUNC_UPLOAD        = 2,
	USBDC_FUNC_EXEC          = 3,
	USBDC_FUNC_GET_BUFF_ADDR = 4,
	USBDC_FUNC_COPYEXEC      = 5,

	USBDC_FUNC_EXIT          = 0xFE,
};

//////////////////////////////////////////////////////////////////////////////

void ud_send_byte(u8 val)
{
	while ((USB_FLAGS & USB_TXE) != 0) ;
	USB_FIFO = val;
}

//////////////////////////////////////////////////////////////////////////////

u8 ud_receive_byte(void)
{
	while ((USB_FLAGS & USB_RXF) != 0) {}
	return USB_FIFO;
}

//////////////////////////////////////////////////////////////////////////////

void ud_send_long(u32 val)
{
	ud_send_byte(val >> 24);
	ud_send_byte(val >> 16);
	ud_send_byte(val >> 8);
	ud_send_byte(val);
}

//////////////////////////////////////////////////////////////////////////////

u32 ud_receive_long(void)
{
	u32 tmp = ud_receive_byte() << 24;
	tmp |= ud_receive_byte() << 16;
	tmp |= ud_receive_byte() << 8;
	tmp |= ud_receive_byte();

	return tmp;
}

//////////////////////////////////////////////////////////////////////////////

static void ud_download(void)
{
	u8 *buf;
	u32 len, i;
	crc_t checksum = crc_init();

	buf = (u8 *)ud_receive_long();

	len = ud_receive_long();

	for (i = 0; i < len; i++)
		ud_send_byte(buf[i]);

	checksum = crc_update(checksum, buf, len);
	checksum = crc_finalize(checksum);

	ud_send_byte(checksum);
}

//////////////////////////////////////////////////////////////////////////////

static void ud_upload(void)
{
	u8 *buf;
	u32 len, i;
	crc_t checksum = crc_init();

	buf = (u8 *)ud_receive_long();
	len = ud_receive_long();

	for (i = 0; i < len; i++)
	{
		// inlining is 20K/s faster
		while ((USB_FLAGS & USB_RXF) != 0) {}
		buf[i] = USB_FIFO;
	}

	checksum = crc_update(checksum, buf, len);
	checksum = crc_finalize(checksum);

	if (checksum != ud_receive_byte())
		ud_send_byte(1);
	else
		ud_send_byte(0);
}

//////////////////////////////////////////////////////////////////////////////

static void ud_execute(void)
{
	u32 addr=ud_receive_long();
	((void (*)())addr)();
}

//////////////////////////////////////////////////////////////////////////////

static void ud_copy_and_exec(void)
{
	// Copy data from a to b and then execute
	/*u8 *src=(u8 *)*/ ud_receive_long();
	/*u32 len= */ ud_receive_long();
	/*u8 *dst=(u8 *)*/ ud_receive_long();
}

//////////////////////////////////////////////////////////////////////////////

void ud_check(u8 unused)
{
	u8 command;

	if ((USB_FLAGS & USB_RXF) != 0)
		return;

	// Configure SCU A-bus settings
	// Disabling A-bus refresh adds ~20KB/s
	SCU_REG_ABUSREFRESH = 0;

	// Removing wait states doubles speed
	SCU_REG_ABUSSRCS0CS1 = 0;

	command = ud_receive_byte();
	switch (command)
	{
	case USBDC_FUNC_DOWNLOAD:
		ud_download();
		break;

	case USBDC_FUNC_UPLOAD:
		ud_upload();
		break;

	case USBDC_FUNC_EXEC:
		ud_execute();
		break;

	case USBDC_FUNC_GET_BUFF_ADDR:
		// Send free area for uploading and execution
		ud_send_long(0); // FIXME
		break;

	case USBDC_FUNC_COPYEXEC:
		ud_copy_and_exec();
		break;

	default:
		break;
	}

	// Restore A-bus settings
	SCU_REG_ABUSREFRESH = 0x1F;
	SCU_REG_ABUSSRCS0CS1 = 0x1FF01FF0;
}

//////////////////////////////////////////////////////////////////////////////

int ud_detect(void)
{
	u16 vendor_id, device_id;

	ar_get_product_id(&vendor_id, &device_id);

	if (vendor_id == 0xBFBF && device_id == 0xB5B5 &&
		USB_FLAGS != 0xFF && USB_FIFO != 0xFF)
		return IAPETUS_ERR_OK;
	else
		return IAPETUS_ERR_HWNOTFOUND;
}

//////////////////////////////////////////////////////////////////////////////
