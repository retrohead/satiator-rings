#include <jo/jo.h>
#include <stdlib.h>
#include <stdio.h>
#include "satiator/iapetus/iapetus.h"
#include "satiator/endian.h"
#include "satiator/jhloader.h"
#include "satiator_functions.h"

enum SATIATOR_ERROR_CODE satiatorWriteData(int fd, const void * buff, unsigned int size)
{
    int result = 0;
    for(unsigned int b = 0; b < size; )
    {
        unsigned int count;
        count = MIN(size - b, S_MAXBUF);
        result = s_write(fd, buff + b, count);
        s_sync(fd);
        if(result <= 0)
        {
            s_close(fd);
            return -1;
        }
        b += count;
    }
    return size;
}

enum SATIATOR_ERROR_CODE satiatorWriteU8(int fd, uint8_t val)
{
    char data[1];
    data[0] = (val & 0xFF);
    return satiatorWriteData(fd, &data, 1);
}

enum SATIATOR_ERROR_CODE satiatorWriteU16(int fd, uint16_t val)
{
    // byte flipped
    char data[1];
    data[0] = (val >> 8);
    if(satiatorWriteData(fd, &data, 1) < 0)
        return SATIATIOR_WRITE_ERR;
    data[0] = (val & 0xFF);
    if(satiatorWriteData(fd, &data, 1) < 0)
        return SATIATIOR_WRITE_ERR;
    return 2;
}

enum SATIATOR_ERROR_CODE satiatorWriteU32(int fd, uint32_t val)
{
    int data = val;
    return satiatorWriteData(fd, &data, sizeof(int));
}

enum SATIATOR_ERROR_CODE iso2desc(const char *infile, const char *outfile) {
    char statbuf[280] = {0};
    s_stat_t *st = (s_stat_t*)statbuf;
    int ret = s_stat(infile, st, sizeof(statbuf)-1);
    if (ret < 0) {
        return SATIATIOR_FILE_STAT_ERR;
    }
    int fd = s_open(outfile, FA_WRITE|FA_CREATE_ALWAYS);
    if (fd < 0) {
        return SATIATIOR_OPEN_FILE_ERR;
    }
    satiatorWriteU16(fd, htole16(1));                       // [u32] h_nseg;
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

    s_close(fd);
    return SATIATIOR_SUCCESS;
}

enum SATIATOR_ERROR_CODE file2desc(const char *infile, const char *outfile) {    
    return iso2desc(infile, outfile);
}

enum SATIATOR_ERROR_CODE satiatorEmulateDesc(char * descfile)
{
    //fadeout(0x20);

    satiator_cart_header_t *cart = s_find_cartridge();
    if (cart && cart->header_version >= 1)
        cart->install_soft_reset_hook();

    //restore_vdp_mem();

    s_emulate(descfile);
    while (is_cd_present());
    while (!is_cd_present());
    s_mode(s_cdrom);
    int ret = boot_disc();
    // jo_core_exit_to_multiplayer();
    //s_mode(s_api);   // failed, restore order
    //s_emulate("");  // close the old file
    //fadein(0x20);
    return SATIATIOR_SUCCESS;
}

enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn)
{
    int ret = file2desc(fn, "emu.desc");
    if (ret < 0) {
        return false;
    }
    return satiatorEmulateDesc("emu.desc");
}

int satiatorExecutableFilter(dirEntry *entry) {
    if (entry->type == DIR_DIRECTORY)
        return 1;

    int len = strlen(entry->name);
    if (!strncmp(&entry->name[len-4], ".cue", 4))
        return 1;
    if (!strncmp(&entry->name[len-4], ".iso", 4))
        return 1;
    if (!strncmp(&entry->name[len-4], ".dec", 4))
        return 1;
    return 0;
}