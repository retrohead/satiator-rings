/*  Copyright 2005-2007,2013 Theo Berkau

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

#ifndef BIOS_H
#define BIOS_H

#define INDIRECT_CALL(addr, return_type, ...)       (**(return_type(**)(__VA_ARGS__)) addr)

#define bios_run_cd_player              INDIRECT_CALL(0x0600026C, void,     void                            )
#define bios_is_mpeg_card_present       INDIRECT_CALL(0x06000274, int,      int filtno                      )
#define bios_get_mpeg_rom               INDIRECT_CALL(0x06000298, int,      u32 index, u32 size, u32 addr   )
#define bios_check_cd_auth              INDIRECT_CALL(0x06000270, int,      int mode                        )   //!< mode 0 -> check, 1 -> do auth
#define bios_set_scu_interrupt          INDIRECT_CALL(0x06000300, void,     u32 vector, void *func          )
#define bios_get_scu_interrupt          INDIRECT_CALL(0x06000304, void *,   u32 vector                      )
#define bios_set_sh2_interrupt          INDIRECT_CALL(0x06000310, void,     u32 vector, void *func          )
#define bios_get_sh2_interrupt          INDIRECT_CALL(0x06000314, void *,   u32 vector                      )
#define bios_set_clock_speed            INDIRECT_CALL(0x06000320, void,     u32 mode                        )
#define bios_get_clock_speed()          (*(volatile u32*)0x06000324)
#define bios_set_scu_interrupt_mask     INDIRECT_CALL(0x06000340, void,     u32 bits                        )
#define bios_change_scu_interrupt_mask  INDIRECT_CALL(0x06000344, void,     u32 mask, u32 bits              )
#define bios_get_scu_interrupt_mask()   (*(volatile u32*)0x06000348)

//! Prepare for Load CD operation.
/*!
 *  A Saturn CD may be read and booted using the bios_loadcd_ calls in order init, read, boot.
 *  Negative return values indicate failures.
 *
 *  bios_loadcd_init():
 *      - resets selectors and the usual things
 *      - resets some BIOS state variables
 *      - starts a disc authentication
 */
#define bios_loadcd_init                INDIRECT_CALL(0x0600029c, int,      void                            )
//! Start reading for Load CD.
/*!
 *  bios_loadcd_read():
 *      - checks the CD auth result
 *      - starts a Play command for the first 16 disc sectors, into selector 0
 *      - sets some BIOS flags
 */
#define bios_loadcd_read                INDIRECT_CALL(0x060002cc, int,      void                            )
//! Attempt to boot loaded disc
/*!
 *  Must not be called until the disc read is complete.
 *  bios_loadcd_boot():
 *      - reads the sector data from the CDB to 0x06002000
 *      - checks the SEGA SEGASATURN header
 *      - checks the IP size, security code
 *      - checks the region
 *      - boots the disc if checks pass
 *
 *  Return values include:
 *      -1 - bad header
 *      -4 - bad security code
 *      -8 - bad region
 *      +1 - not a Saturn disc
 */
#define bios_loadcd_boot                INDIRECT_CALL(0x06000288, int,      void                            )

#endif
