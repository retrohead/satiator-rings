/*  Copyright 2006,2013-2014 Theo Berkau

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

#ifndef COMMLINK_H
#define COMMLINK_H

u8 cl_exchange_byte(u8 val);
u8 cl_receive_byte(void);
void cl_send_long(u32 val);
u32 cl_receive_long(void);
void cl_check1(u8 unused);
void cl_check2(u8 val);
void commlink_start_service(void);
void commlink_stop_service(void);
void cl_set_service_func(void (*func)(u8));

#endif
