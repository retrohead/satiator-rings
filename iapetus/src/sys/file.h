/*  Copyright 2008,2013 Theo Berkau

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

#ifndef FILE_H
#define FILE_H

typedef struct {
   const char *name;
   int (*open) (struct _reent *r, const char *path, int flags, int mode );
   int (*close) (struct _reent *r, int fd );
   _ssize_t (*write) (struct _reent *r, int fd, const char *ptr, int len);
   _ssize_t (*read) (struct _reent *r, int fd, char *ptr, int len);
} devoptab_t;

extern const devoptab_t dotab_stdin;
extern const devoptab_t dotab_stdout;
extern const devoptab_t dotab_stderr;
extern const devoptab_t dotab_cd;
extern const devoptab_t dotab_bup;
extern const devoptab_t *devoptab_list[];

#endif
