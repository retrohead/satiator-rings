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

int bup_open(struct _reent *r, const char *path, int flags, int mode)
{
   return 0; // fix me
}

//////////////////////////////////////////////////////////////////////////////

int bup_close(struct _reent *r, int fd)
{
   return 0; // fix me
}

//////////////////////////////////////////////////////////////////////////////

_ssize_t bup_write(struct _reent *r, int fd, const char *ptr, int len)
{
   return len; // fix me
}

//////////////////////////////////////////////////////////////////////////////

_ssize_t bup_read(struct _reent *r, int fd, char *ptr, int len)
{
   return len; // fix me
}

//////////////////////////////////////////////////////////////////////////////

const devoptab_t dotab_bup = {
   "bup",
   bup_open,
   bup_close,
   bup_write,
   bup_read
};
