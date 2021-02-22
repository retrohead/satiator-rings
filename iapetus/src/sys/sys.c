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

#include <stdio.h>
#include <reent.h>
#include "../iapetus.h"
#include "file.h"

const devoptab_t *devoptab_list[] = {
   &dotab_stdin,
   &dotab_stdout,
   &dotab_stderr,
   &dotab_cd,
   &dotab_bup,
   NULL
};

//////////////////////////////////////////////////////////////////////////////
/*
void *_sbrk_r _PARAMS ((struct _reent *, ptrdiff_t))
{
}
*/
//////////////////////////////////////////////////////////////////////////////
/*
int _stat_r _PARAMS ((struct _reent *, const char *, struct stat *))
{
}
*/
//////////////////////////////////////////////////////////////////////////////
/*
_CLOCK_T_ _times_r _PARAMS ((struct _reent *, struct tms *))
{
}
*/
//////////////////////////////////////////////////////////////////////////////

