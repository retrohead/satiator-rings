/*  Copyright 2005-2007 Theo Berkau

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

#ifndef LAPETUS_H
#define LAPETUS_H

#include "bios.h"
#include "types.h"
#include "ar/arcomm.h"
#include "ar/commlink.h"
#include "ar/usbdev.h"
#include "cd/cd.h"
#include "cd/cdfs.h"
#include "cd/mpeg.h"
#include "cd/cdsect.h"
#include "debug/debug.h"
#include "modem/netlink.h"
#include "peripherals/smpc.h"
#include "scu/dsp.h"
#include "scu/scudma.h"
#include "scu/scuregs.h"
#include "scu/extram.h"
#include "sh2/sh2dma.h"
#include "sh2/sh2int.h"
#include "sh2/sh2regs.h"
#include "sh2/sci.h"
#include "sh2/timer.h"
#include "sound/sound.h"
#include "ui/font.h"
#include "ui/gui.h"
#include "ui/text.h"
#include "video/vdp.h"

void init_iapetus(int res);

enum IAPETUS_ERR
{
   IAPETUS_ERR_OK=0,                // Everything is good
   IAPETUS_ERR_COMM=-1,             // Communication error
   IAPETUS_ERR_HWNOTFOUND=-2,       // Hardware not found
   IAPETUS_ERR_SIZE=-3,             // Invalid size specified
   IAPETUS_ERR_INVALIDPOINTER=-4,   // Invalid pointer passed
   IAPETUS_ERR_INVALIDARG=-5,       // Invalid argument passed
   IAPETUS_ERR_BUSY=-6,             // Hardware is busy
   IAPETUS_ERR_UNKNOWN=-7,          // Unknown error
   IAPETUS_ERR_FILENOTFOUND=-8,     // File not found error
   IAPETUS_ERR_UNSUPPORTED=-9,      // Unsupported feature
   IAPETUS_ERR_TIMEOUT=-10,         // Operation timed out
   IAPETUS_ERR_UNEXPECTDATA=-11,    // Unexpected data
   IAPETUS_ERR_OUTOFMEMORY=-12,     // Ran out of memory

   // CD/MPEG Related
   IAPETUS_ERR_AUTH=-100,            // Disc/MPEG card authentication error
   IAPETUS_ERR_CMOK=-101,            // CD command ok hirq bit not set
   IAPETUS_ERR_CDNOTFOUND=-102,      // CD not found
   IAPETUS_ERR_MPEGCMD=-103,         // MPEG command hirq bit not set
};

#endif
