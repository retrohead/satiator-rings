/*  Copyright 2014 Theo Berkau

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

int sci_init(enum SCI_SM settings)
{
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void sci_send_byte(u8 val)
{
}

//////////////////////////////////////////////////////////////////////////////

u8 sci_receive_byte()
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

u8 sci_transfer_byte(u8 val)
{
	sci_send_byte(val);
	return sci_receive_byte();
}

//////////////////////////////////////////////////////////////////////////////

u8 sci_transfer_byte_nowait(u8 val, BOOL *success)
{
	return 0;
}

//////////////////////////////////////////////////////////////////////////////

void sci_send_word(u16 val)
{
	sci_transfer_byte((u8)(val >> 8));
	sci_transfer_byte((u8)val);
}

//////////////////////////////////////////////////////////////////////////////

void sci_send_long(u32 val)
{
	sci_transfer_byte((u8)(val >> 24));
	sci_transfer_byte((u8)(val >> 16));
	sci_transfer_byte((u8)(val >> 8));
	sci_transfer_byte((u8)val);
}

//////////////////////////////////////////////////////////////////////////////

u16 sci_receive_word()
{
	u16 ret = sci_transfer_byte(0x00) << 8;
	ret |= sci_transfer_byte(0x00);

	return ret;
}

//////////////////////////////////////////////////////////////////////////////

u32 sci_receive_long()
{
	u32 ret;

	ret = (sci_transfer_byte(0x00) << 24);
	ret |= (sci_transfer_byte(0x00) << 16);
	ret |= (sci_transfer_byte(0x00) << 8);
	ret |= sci_transfer_byte(0x00);

	return ret;
}

//////////////////////////////////////////////////////////////////////////////

int sci_remote_execute(void *buffer, u32 addr, u32 size)
{
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int sci_sync_out()
{
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

int sci_sync_in()
{
	return IAPETUS_ERR_OK;
}

//////////////////////////////////////////////////////////////////////////////

void sci_handler()
{
}

//////////////////////////////////////////////////////////////////////////////
