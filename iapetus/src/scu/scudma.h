/*  Copyright 2007,2013 Theo Berkau

    This file is part of Lapetus.

    Lapetus is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Lapetus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Lapetus; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef DMASCU_H
#define DMASCU_H

int scu_dma_init(void);
int scu_dma_start(int chan, void *src, void *dst, u32 size, u32 add, u32 mode);
BOOL is_scu_dma_running(int chan);
void scu_dma_stop_all(void);

#endif
