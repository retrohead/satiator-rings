/*  Copyright (c) 2015 James Laird-Wah
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/. */

#define MAX_SEGMENTS 300
#define MAX_FILENAMES 100

extern char *cdparse_error_string;
extern char filenames[MAX_FILENAMES][256];
enum SATIATOR_ERROR_CODE image2desc(const char *infile, const char *outfile);

int cue2desc(const char *cue_file, const char *desc_file);
void cdparse_set_error(const char *fmt, ...);
