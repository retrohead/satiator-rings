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
#include "../iapetus.h"
#include "file.h"

//////////////////////////////////////////////////////////////////////////////

int stdin_open(struct _reent *r, const char *path, int flags, int mode)
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

int stdin_close(struct _reent *r, int fd)
{
   return 0;
}

//////////////////////////////////////////////////////////////////////////////

_ssize_t stdin_write(struct _reent *r, int fd, const char *ptr, int len)
{
   return -1; // Doesn't support writing
}

//////////////////////////////////////////////////////////////////////////////

_ssize_t stdin_read(struct _reent *r, int fd, char *ptr, int len)
{
   if (per[0].id == PER_KEYBOARD)
   {
      return len; // fix me
   }

   return -1;
}

//////////////////////////////////////////////////////////////////////////////

const devoptab_t dotab_stdin = {
   "stdin",
   stdin_open,
   stdin_close,
   stdin_write,
   stdin_read
};

//////////////////////////////////////////////////////////////////////////////

