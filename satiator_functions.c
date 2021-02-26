#include <jo/jo.h>
#include <stdlib.h>
#include <stdio.h>
#include "satiator/iapetus/iapetus.h"
#include "satiator/disc_format/cdparse.h"
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
    boot_disc();
    //s_mode(s_api);   // failed, restore order
    //s_emulate("");  // close the old file
    //fadein(0x20);
    return SATIATIOR_SUCCESS;
}

enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn)
{
    if (!strncmp(&fn[strlen(fn) - 5], ".desc", 5))
        return satiatorEmulateDesc(fn);
    int ret = image2desc(fn, "emu.desc");
    if (ret != SATIATIOR_SUCCESS) {
        return SATIATIOR_CREATE_DESC_ERR;
    }
    return satiatorEmulateDesc("emu.desc");
}

int satiatorExecutableFilter(dirEntry *entry) {
    if (entry->type == DIR_DIRECTORY)
        return 1;

    int len = strlen(entry->name);
    if (!strncmp(&entry->name[len-4], ".cue", 4))
        return 1;
    //if (!strncmp(&entry->name[len-4], ".bin", 4))
    //    return 1;
    if (!strncmp(&entry->name[len-4], ".iso", 4))
        return 1;
    if (!strncmp(&entry->name[len-5], ".desc", 5))
        return 1;
    return 0;
}

// Read one line of data with a maximum buffer size
char * s_gets(char *buf, int maxsize, int fd, uint32_t *bytesRead, uint32_t totalBytes)
{
    char c[2];
    strcpy(buf, "");
    for(int i = 0;i< maxsize;i++)
    {
        int j = s_read(fd, &c, 1);
        *bytesRead = *bytesRead + 1;
        if(j < 0)
            break;
        c[1] = '\0';
        if(c[0] == '\n')
            break;
        if(c[0] == '\r')
            continue;
        strcat(buf, c);
        if(*bytesRead == totalBytes)
            break;
    }
    return buf;
}