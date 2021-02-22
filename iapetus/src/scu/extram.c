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

int extram_size=0;

void extram_init()
{
	*((u16 *)0x257EFFFE) = 1;
	if ((*(u8 *)0x24FFFFFF) == 0x5A)
		extram_size = 1;
	else if ((*(u8 *)0x24FFFFFF) == 0x5C)
		extram_size = 4;
	else
		extram_size = 0;

	if (extram_size != 0)
	{
		SCU_REG_ABUSSRCS0CS1 = 0x23301FF0;
		SCU_REG_ABUSREFRESH = 0x00000013;
	}
}