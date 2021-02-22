/*  Copyright 2007,2013 Theo Berkau

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

#ifndef TIMER_H
#define TIMER_H

#define TIMER_RTC       0
#define TIMER_FRT       1
#define TIMER_WDT       2
#define TIMER_HBLANK    3

int timer_setup(int type, u32 *freq);
u32 timer_counter();
void timer_delay(u32 freq, int ms);

#endif
