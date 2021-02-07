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

int cd_put_sector_data(u8 sel_num, u16 num_sectors)
{
	cd_cmd_struct cd_cmd;
	cd_cmd_struct cd_cmd_rs;

	cd_cmd.CR1 = 0x6400;
	cd_cmd.CR2 = 0x0000;
	cd_cmd.CR3 = sel_num << 8;
	cd_cmd.CR4 = num_sectors;

	return cd_exec_command(HIRQ_EHST, &cd_cmd, &cd_cmd_rs);
}

int cd_move_sector_data(u8 dst_filter, u16 sector_pos, u8 sel_num, u16 num_sectors)
{
	cd_cmd_struct cd_cmd;
	cd_cmd_struct cd_cmd_rs;

	cd_cmd.CR1 = 0x6600 | dst_filter;
	cd_cmd.CR2 = sector_pos;
	cd_cmd.CR3 = sel_num << 8;
	cd_cmd.CR4 = num_sectors;

	return cd_exec_command(0, &cd_cmd, &cd_cmd_rs);
}

int cd_get_buffer_size(u16 *blk_free, u8 *max_sel, u16 *max_blk)
{
	cd_cmd_struct cd_cmd;
	cd_cmd_struct cd_cmd_rs;
	int ret;

	cd_cmd.CR1 = 0x5000;
	cd_cmd.CR2 = 0x0000;
	cd_cmd.CR3 = 0x0000;
	cd_cmd.CR4 = 0x0000;

	if ((ret = cd_exec_command(0, &cd_cmd, &cd_cmd_rs)) != IAPETUS_ERR_OK)
		return ret;

	*blk_free = cd_cmd_rs.CR2;
	*max_sel = cd_cmd_rs.CR3 >> 8;
	*max_blk = cd_cmd_rs.CR4;

	return IAPETUS_ERR_OK;
}
