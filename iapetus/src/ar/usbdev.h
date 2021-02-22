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

#ifndef USBDEV_H
#define USBDEV_H

#include "../iapetus.h"
#include "crc.h"

void ud_send_byte(u8 val);
u8 ud_receive_byte(void);
void ud_send_long(u32 val);
u32 ud_receive_long(void);
void ud_check(u8 unused);
int ud_detect(void);

#endif