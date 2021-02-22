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
#include <stdio.h>
#include <string.h>
#include "file.h"

//////////////////////////////////////////////////////////////////////////////

int _open_r _PARAMS ((struct _reent *r, const char *file, int flags, int mode))
{
   int i;
   int device=-1;
   int fd;

   for (i = 0; devoptab_list[i] != NULL; i++)
   {
      if (strncmp(devoptab_list[i]->name, file, strlen(devoptab_list[i]->name)) == 0)
         device = i;
   }

   if (device < 0 || (fd = devoptab_list[device]->open(r, file, flags, mode)) == -1)
      return -1;

   return ((device << 16) | (fd & 0xFFFF));
}

//////////////////////////////////////////////////////////////////////////////

