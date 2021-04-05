/*  Copyright (c) 2017 James Laird-Wah
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/. */

#include <jo/jo.h>
#include <string.h>
#include <stdio.h>
// #include <sys/types.h>
// #include <sys/stat.h>
#include "../endian.h"
#include <stdlib.h>
#include <stdarg.h>
#include "cdparse.h"

#include "../iapetus/iapetus.h"
#include "../jhloader.h"
#include "../../satiator_functions.h"
#include "../../sprite_manager.h"
#include "../../debug.h"

#ifdef TEST
    #define dbgprintf printf
    #include "../satiator-types.h"
#else
    #include "../../libsatiator/satiator.h"
#endif

char *cdparse_error_string = NULL;

void cdparse_set_error(const char *fmt, ...) {
   va_list ap;
   if (cdparse_error_string)
       jo_free(cdparse_error_string);

   char buf[1];
   va_start(ap, fmt);
   int len = vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   cdparse_error_string = jo_malloc(len+1);
   va_start(ap, fmt);
   vsnprintf(cdparse_error_string, len+1, fmt, ap);
   va_end(ap);
}

enum SATIATOR_ERROR_CODE iso2desc(const char *infile, const char *outfile) {
    centerTextVblank(20, "Reading ISO File");
    s_stat_t *st = (s_stat_t*)statbuf;
    int ret = s_stat(infile, st, sizeof(statbuf)-1);
    if (ret < 0) {
        cdparse_set_error("Could not stat ISO file");
        return SATIATOR_FILE_STAT_ERR;
    }
    
    centerTextVblank(20, "Creating Desc File");
    int fd = s_open(outfile, FA_WRITE|FA_CREATE_ALWAYS);
    if (fd < 0) {
        cdparse_set_error("Can't open output file");
        return SATIATOR_OPEN_FILE_ERR;
    }
    satiatorWriteU16(fd, htole16(1));                       // [u16] h_nseg;
    satiatorWriteU32(fd, htole32(150));                     // [u32] desc.start;
    satiatorWriteU32(fd, htole32(st->size / 2048));         // [u32] desc.length;
    satiatorWriteU32(fd, htole32(0));                       // [u32] desc.file_offset;
    satiatorWriteU32(fd, htole32(2 + sizeof(seg_desc_t)));  // [u32] desc.filename_offset;
    satiatorWriteU16(fd, htole16(0));                       // [u16] flags // nothing
    satiatorWriteU16(fd, htole16(2048));                    // [u16] desc.secsize;
    satiatorWriteU8(fd, 1);                                 // [u8] desc.track;
    satiatorWriteU8(fd, 1);                                 // [u8] desc.index;
    satiatorWriteU8(fd, 0x41);                              // [u8] desc.q_mode;

    uint8_t filename_len = strlen(infile);
    satiatorWriteU8(fd, filename_len);
    satiatorWriteData(fd, infile, filename_len);

    centerTextVblank(20, "Completing");
    s_close(fd);
    return SATIATOR_SUCCESS;
}

enum SATIATOR_ERROR_CODE image2desc(const char *infile, const char *outfile) {

   if (cdparse_error_string)
      jo_free(cdparse_error_string);
   cdparse_error_string = NULL;

   const char *dot = strrchr(infile, '.');
   if (!dot) {
      cdparse_set_error("Unrecognised file extension - no dot in filename");
      return SATIATOR_FILE_EXT_ERR;
   }

   const char *extension = dot + 1;

   if (!strncmp(extension, "cue", 3))
      return cue2desc(infile, outfile);

   if (!strncmp(extension, "iso", 3))
   {
      strcpy(filenames[0], infile);
      return iso2desc(infile, outfile);
   }

   cdparse_set_error("Unrecognised file extension '%s'", dot);
   return SATIATOR_FILE_EXT_ERR;
}
