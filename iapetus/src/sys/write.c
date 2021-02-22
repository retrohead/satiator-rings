/*  Copyright 2008 Theo Berkau

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

#include <reent.h>
#include "file.h"

//////////////////////////////////////////////////////////////////////////////

_ssize_t _write_r _PARAMS ((struct _reent *r, int fd, const void *ptr, size_t len))
{
   int device = fd >> 16;
   return devoptab_list[device]->write(r, fd & 0xFFFF, ptr, len);
}

//////////////////////////////////////////////////////////////////////////////

