/*  Copyright (c) 2017 James Laird-Wah
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, you can obtain one at http://mozilla.org/MPL/2.0/. */

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <jo/jo.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "../endian.h"
#include "cdparse.h"
#include "../../satiator_functions.h"
#include "../../debug.h"

char *my_strtok( char **stringp, const char *delim )
{
    assert( delim );
    if( *stringp == NULL ) return NULL;
    char *next = strstr( *stringp, delim );
    char *orig = *stringp;
    if( next == NULL )
    *stringp = NULL;
    else
    {
    *next = '\0';
    *stringp = next + strlen( delim );
    }
    return orig;
}
char *my_strtok_r(char *str, const char *delim, char **save)
{
    char *res, *last;

    if( !save )
        return my_strtok(&str, delim);
    if( !str && !(str = *save) )
        return NULL;
    last = str + strlen(str);
    if( (*save = res = my_strtok(&str, delim)) )
    {
        *save += strlen(res);
        if( *save < last )
            (*save)++;
        else
            *save = NULL;
    }
    return res;
}

char *my_strdup(const char *str) {
    size_t len = strlen(str);
    char *x = (char *)jo_malloc(len+1); /* 1 for the null terminator */
    if(!x) return NULL; /* malloc could not allocate memory */
    memcpy(x,str,len+1); /* copy the string into the new buffer */
    return x;
}


#ifdef TEST
    #define dbgprintf printf
    #include "../satiator-types.h"
#else
    #include "../satiator.h"
    #ifdef assert
        #undef assert
    #endif
    #define assert(arg)
#endif

static int whitespace(char c) {
    switch (c) {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return 1;
        default:
            return 0;
    }
}

typedef enum {
    e_ok = 0,
    e_no_cue_file,
    e_bad_cue_file,
    e_bad_track_file,
    e_bad_out_file,
} cue2desc_error_t;

typedef enum {
    df_binary = 0,
    df_wave,
} data_format_t;

typedef struct {
    uint8_t track, index, q_mode;
    uint32_t start, length;
    uint32_t file_offset;
    int filename_index;
    uint16_t secsize;
} segment_t;

typedef struct __attribute__((packed)) {
    char magic_riff[4];
    uint32_t riff_size;
    char magic_wave[4];
    char magic_fmt[4];
    uint32_t fmt_size;
    uint16_t codec;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
} wave_header_t;

int cur_fileindex;
uint32_t cur_filesize;
uint8_t cur_track, cur_q_mode;
uint16_t cur_secsize;
data_format_t cur_data_format;

segment_t segments[MAX_SEGMENTS];
char filenames[MAX_FILENAMES][256];
int n_filenames;
int nseg, open_seg;

static segment_t *alloc_seg(void) {
    nseg++;
    if(nseg >= MAX_SEGMENTS)
    {
        return NULL;
    }
    segment_t *seg = &segments[nseg-1];
    seg->filename_index = -1;
    return seg;
}

static cue2desc_error_t write_desc_file(const char *filename) {
    centerTextVblank(20, "Writing Desc File");
    int out = s_open(filename, FA_WRITE|FA_CREATE_ALWAYS);
    if (out < 0) {
        cdparse_set_error("Can't open output file");
        return e_bad_out_file;
    }
    uint32_t filename_offsets[n_filenames];

    int offset = s_seek(out, 2 + nseg * sizeof(seg_desc_t), SEEK_SET);
    for (int i=0; i<n_filenames; i++) {
        centerTextVblank(20, "Writing Filename %d of %d", i + 1, n_filenames);
        filename_offsets[i] = offset;

        uint8_t namelen = strlen(filenames[i]);
        satiatorWriteU8(out, namelen);
        satiatorWriteData(out, filenames[i], namelen);
        offset += namelen + 1;
    }

    // careful here: Saturn is big-endian but we have to write everything little-endian

    s_seek(out, 0, SEEK_SET);
    satiatorWriteU16(out, htole16(nseg));                                           // [u16] h_nseg;

    for (int i=0; i<nseg; i++) {
        centerTextVblank(20, "Writing Segment %d of %d", i + 1, nseg);
        segment_t *seg = &segments[i];
        seg_desc_t desc;
        desc.track = seg->track;
        desc.index = seg->index;
        desc.q_mode = seg->q_mode;
        desc.start = htole32(seg->start);
        desc.length = htole32(seg->length);

        if (seg->filename_index >= 0) {
            desc.secsize = htole16(seg->secsize);
            desc.filename_offset = htole32(filename_offsets[seg->filename_index]);
            desc.file_offset = htole32(seg->file_offset);
        } else
        {
            desc.secsize = htole16(0);
            desc.filename_offset = htole32(0);
            desc.file_offset = htole32(0);
        }
        desc.flags = htole16(0);

        s_write(out, &desc, sizeof(seg_desc_t));

        /*
        // 23 bytes for each sector
        satiatorWriteU32(out, desc.start);                                      // [u32] desc.start;
        satiatorWriteU32(out, desc.length);                                  // [u32] desc.length;
        satiatorWriteU32(out, desc.file_offset);                         // [u32] desc.file_offset;
        satiatorWriteU32(out, desc.filename_offset);    // [u32] desc.filename_offset;
        satiatorWriteU16(out, desc.flags);                                       // [u16] flags // nothing
        satiatorWriteU16(out, desc.secsize);                             // [u16] desc.secsize;

        satiatorWriteU8(out, desc.track);                                             // [u8] desc.track;
        satiatorWriteU8(out, desc.index);                                             // [u8] desc.index;
        satiatorWriteU8(out, desc.q_mode);                                            // [u8] desc.q_mode;
        */
    }

    s_close(out);
    return e_ok;
}


static void handle_end_of_track_file(void) {
    if (cur_fileindex < 0)  // no file to end
        return;
    cur_fileindex = -1;

    assert(open_seg >= 0);

    segment_t *open = &segments[open_seg];
    assert(open->length == 0);

    uint32_t remain_bytes = cur_filesize - open->file_offset;
    open->length = remain_bytes / open->secsize;

    open_seg = -1;
}

static cue2desc_error_t handle_file(char *params) {
    handle_end_of_track_file();

    char *filename_start, *mode;

    char *lquot = strchr(params, '"');
    if (lquot) {
        char *rquot = strrchr(params, '"');
        *rquot = '\0';
        filename_start = lquot + 1;
        mode = rquot + 1;
    } else {
        // no quotes, just strip space
        while (*params && whitespace(*params))
            params++;
        char *space = strrchr(params, ' ');
        *space = '\0';
        filename_start = params;
        mode = space + 1;
    }

    // strip any path components from filename
    char *slash;
    if ((slash = strrchr(filename_start, '\\')))
        filename_start = slash+1;

    if ((slash = strrchr(filename_start, '/')))
        filename_start = slash+1;

    char *filename = my_strdup(filename_start);

    while (*mode && whitespace(*mode))
        mode++;

    n_filenames++;
    cur_fileindex = n_filenames - 1;
    strcpy(filenames[cur_fileindex], filename);
    s_stat_t st[280];
    int ret = s_stat(filename, st, sizeof(st)-1);
    if (ret < 0) {
        cdparse_set_error("Could not stat track file '%s'", filename);
        jo_free(filename);
        return e_bad_cue_file;
    }

    cur_filesize = st->size;

    jo_free(filename);
    if (!strcmp(mode, "BINARY")) {
        cur_data_format = df_binary;
    } else if (!strcmp(mode, "WAVE")) {
        cur_data_format = df_wave;
    } else {
        cdparse_set_error("Bad file mode '%s' (need BINARY or WAVE)", mode);
        return e_bad_cue_file;
    }
    return e_ok;
}

static cue2desc_error_t handle_track(char *params) {
    char *saveptr = NULL;
    char *s_tno = my_strtok_r(params, " ", &saveptr);
    if (!s_tno) {
        cdparse_set_error("Did not find track number in params %s", params);
        return e_bad_cue_file;
    }
    int track;
    sscanf(s_tno, "%d", &track);
    cur_track = (uint8_t)track;
    if (!cur_track || cur_track > 99) {
        cdparse_set_error("Found ""invalid track number %d", cur_track);
        return e_bad_cue_file;
    }

    char *s_mode = my_strtok_r(NULL, " ", &saveptr);
    if (!s_mode) {
        cdparse_set_error("Did not find track mode");
        return e_bad_cue_file;
    }

    if (!strcmp(s_mode, "MODE1/2048")) {
        if (cur_data_format == df_wave) {
            cdparse_set_error("Encountered track mode '%s' for WAVE data format", s_mode);
            return e_bad_cue_file;
        }
        cur_q_mode = 0x41;
        cur_secsize = 2048;
    } else if (!strcmp(s_mode, "MODE1/2352") || !strcmp(s_mode, "MODE2/2352")) {
        if (cur_data_format == df_wave) {
            cdparse_set_error("Encountered track mode '%s' for WAVE data format", s_mode);
            return e_bad_cue_file;
        }
        cur_q_mode = 0x41;
        cur_secsize = 2352;
    } else if (!strcmp(s_mode, "AUDIO")) {
        cur_q_mode = 0x01;
        cur_secsize = 2352;
    } else {
        cdparse_set_error("Unknown track mode '%s'", s_mode);
        return e_bad_cue_file;
    }
    return e_ok;
}

static cue2desc_error_t handle_wave_track(segment_t *seg) {
    const char* fname = filenames[seg->filename_index];
    int f = s_open(fname, FA_READ | FA_OPEN_EXISTING);
    if (f < 0) {
        cdparse_set_error("Could not open WAVE file '%s'", fname);
        return e_bad_track_file;
    }
    wave_header_t hdr;
    int size = s_seek(f, 0, SEEK_END);
    s_seek(f, 0, SEEK_SET);
    int offset = 0;
    s_read(f, &hdr, sizeof(hdr));
    offset += sizeof(hdr);
    hdr.riff_size = htole32(hdr.riff_size);
    hdr.fmt_size = htole32(hdr.fmt_size);
    hdr.codec = htole16(hdr.codec);
    hdr.channels = htole16(hdr.channels);
    hdr.sample_rate = htole32(hdr.sample_rate);
    hdr.byte_rate = htole32(hdr.byte_rate);
    hdr.block_align = htole16(hdr.block_align);
    hdr.bits_per_sample = htole16(hdr.bits_per_sample);
    if (memcmp(hdr.magic_riff, "RIFF", 4)) {
        cdparse_set_error("Invalid RIFF header in file '%s': %.4s", fname, hdr.magic_riff);
        return e_bad_track_file;
    }
    if (memcmp(hdr.magic_wave, "WAVE", 4)) {
        cdparse_set_error("Invalid WAVE header in file '%s': %.4s", fname, hdr.magic_wave);
        return e_bad_track_file;
    }
    if (memcmp(hdr.magic_fmt, "fmt ", 4)) {
        cdparse_set_error("Invalid fmt header in file '%s': %.4s", fname, hdr.magic_fmt);
        return e_bad_track_file;
    }
    if (hdr.fmt_size != 16) {
        cdparse_set_error("fmt size in '%s' must be 16 (PCM), not %d", fname, hdr.fmt_size);
        return e_bad_track_file;
    }
    if (hdr.codec != 1) {
        cdparse_set_error("Format of '%s' must be 1 (PCM), not %d", fname, hdr.codec);
        return e_bad_track_file;
    }
    if (hdr.channels != 2) {
        cdparse_set_error("Channels of '%s' must be 2 (stereo), not %d", fname, hdr.channels);
        return e_bad_track_file;
    }
    if (hdr.sample_rate != 44100) {
        cdparse_set_error("Sample rate of '%s' must be 44100Hz, not %d", fname, hdr.sample_rate);
        return e_bad_track_file;
    }
    if (hdr.byte_rate != 44100 * 2 * 16/8) {
        cdparse_set_error("Byte rate of '%s' must be 44100*2*16/8, not %d", fname, hdr.byte_rate);
        return e_bad_track_file;
    }
    if (hdr.block_align != 2 * 16/8) {
        cdparse_set_error("Block alignment of '%s' must be 2*16/8, not %d", fname, hdr.block_align);
        return e_bad_track_file;
    }
    if (hdr.bits_per_sample != 16) {
        cdparse_set_error("Sample depth of '%s' must be 16-bit, not %d", fname, hdr.bits_per_sample);
        return e_bad_track_file;
    }
    // { chunk name, chunk length }
    uint32_t buf[2];
    s_read(f, &buf, sizeof(buf));
    offset += sizeof(buf);
    // skip other chunks until we find "data"
    while (memcmp(&buf[0], "data", 4)) {
        s_seek(f, htole32(buf[1]), SEEK_CUR);
        offset += htole32(buf[1]);
        s_read(f, &buf, sizeof(buf));
        offset += sizeof(buf);
        if (offset >= size) {
            s_close(f);
            cdparse_set_error("Reached end of '%s' while looking for data chunk", fname);
            return e_bad_track_file;
        }
    }
    // here's where the actual audio data starts in the file
    seg->file_offset += offset;
    s_close(f);
    return e_ok;
}

static cue2desc_error_t handle_index(char *params) {
    int index, mm, ss, ff;
    int n = sscanf(params, "%d %d:%d:%d", &index, &mm, &ss, &ff);
    if (n != 4) {
        cdparse_set_error("Could not parse index '%s'", params);
        return e_bad_cue_file;
    }

    segment_t *seg = alloc_seg();
    if(!seg)
    {
        cdparse_set_error("Not enough free segments", params);
        return e_bad_cue_file;
    }
    seg->track = cur_track;
    seg->index = index;
    seg->q_mode = cur_q_mode;
    seg->secsize = cur_secsize;

    uint32_t fad = ff + 75*(ss + 60*mm);
    seg->filename_index = cur_fileindex;
    seg->file_offset = fad * cur_secsize;

    if (cur_data_format == df_wave) {
        cue2desc_error_t result = handle_wave_track(seg);
        if (result != e_ok) {
            return result;
        }
    }

    if (open_seg >= 0) {
        segment_t *open = &segments[open_seg];
        uint32_t remain_bytes = seg->file_offset - open->file_offset;
        open->length = remain_bytes / open->secsize;
    }

    open_seg = nseg-1;

    return e_ok;
}

static cue2desc_error_t handle_pregap(char *params) {
    int mm, ss, ff;
    int n = sscanf(params, "%d:%d:%d", &mm, &ss, &ff);
    if (n != 3) {
        cdparse_set_error("Could not parse pregap '%s'", params);
        return e_bad_cue_file;
    }
    uint32_t fad = ff + 75*(ss + 60*mm);

    segment_t *seg = alloc_seg();
    seg->track = cur_track;
    seg->index = 0;
    seg->length = fad;
    seg->q_mode = cur_q_mode;

    return e_ok;
}

int cue2desc(const char *cue_file, const char *desc_file) {
    cue2desc_error_t ret = e_ok;

    s_stat_t *st = (s_stat_t*)statbuf;
    int stret = s_stat(cue_file, st, sizeof(statbuf)-1);
    if (stret < 0) {
        cdparse_set_error("Could not stat cue file");
        return e_no_cue_file;
    }

    int fp = s_open(cue_file, FA_READ);
    if (fp < 0) {
        cdparse_set_error("Could not open specified cue file");
        return e_no_cue_file;
    }
    cur_filesize = cur_track = cur_secsize = 0;
    cur_fileindex = open_seg = -1;
    nseg = 0;
    n_filenames = 0;

    char buf[512];
    char *line;
    uint32_t bytesRead = 0;
    while (bytesRead < st->size)
    {
        centerTextVblank(20, "Parsing Cue file %d%%", (int)(((double)bytesRead / (double)st->size) * 100));
        line = s_gets(buf, sizeof(buf), fp, &bytesRead, st->size);
        // Trim whitespace at front
        while (*line && whitespace(*line))
            line++;
        if (!*line)
            continue;
        // Trim whitespace at end
        while(whitespace(line[strlen(line) - 1]))
            line[strlen(line) - 1] = '\0';

        // Find command word
        char *space = strchr(line, ' ');
        if (!space) {
            cdparse_set_error("No space in line");
            ret = e_bad_cue_file;
            continue;
        }
        *space = '\0';
        char *params = space + 1;

        if (!strcmp(line, "CATALOG")
         || !strcmp(line, "CDTEXTFILE")
         || !strcmp(line, "FLAGS")
         || !strcmp(line, "ISRC")
         || !strcmp(line, "PERFORMER")
         || !strcmp(line, "POSTGAP")
         || !strcmp(line, "REM")
         || !strcmp(line, "SONGWRITER")
         || !strcmp(line, "TITLE"))
            continue;   // recognised, but unsupported

        if (!strcmp(line, "FILE"))
            ret = handle_file(params);
        else if (!strcmp(line, "TRACK"))
            ret = handle_track(params);
        else if (!strcmp(line, "INDEX"))
            ret = handle_index(params);
        else if (!strcmp(line, "PREGAP"))
            ret = handle_pregap(params);
        else
            ret = e_bad_cue_file;

        if (ret != e_ok){
            goto out;
        }
    }
    centerTextVblank(20, "Parsing Cue file 100%%");

    handle_end_of_track_file();

    uint32_t disc_fad = 150;
    for (int i=0; i<nseg; i++) {
        segments[i].start = disc_fad;
        disc_fad += segments[i].length;
    }
    ret = write_desc_file(desc_file);

out:
    s_close(fp);
    return ret;
}
