#include <jo/jo.h>
#include <stdlib.h>
#include <stdio.h>
#include "bios.h"
#include "satiator/iapetus/iapetus.h"
#include "satiator/disc_format/cdparse.h"
#include "satiator/jhloader.h"
#include "satiator_functions.h"
#include "debug.h"

enum SATIATOR_STATE satiatorState = SATIATOR_STATE_NOT_FOUND;
extern void addItemToRecentHistory();

enum SATIATOR_ERROR_CODE satiatorCreateDirectory(char * dir)
{
    int ret = s_opendir(dir);
    if (ret == 0) {
        return SATIATOR_ALREADY_EXISTS;
    }
    ret = s_mkdir(dir);
    if (ret < 0) {
        return SATIATOR_WRITE_ERR;
    }
    return SATIATOR_SUCCESS;
}

void initSatiator()
{
    if(satiatorState == SATIATOR_STATE_WORKING)
        return;
    strcpy(currentDirectory, "/");
    if(s_mode(s_api) == 0)
    {
        int result = 0;
        result = s_opendir(".");
        if(result != 0)
        {
            satiatorState = SATIATOR_STATE_NOT_WORKING;
        }
        else 
        {
            satiatorState = SATIATOR_STATE_WORKING;
            s_chdir("/");
            satiatorCreateDirectory("/satiator-rings");
        }
    } else
    {
        satiatorState = SATIATOR_STATE_NOT_FOUND;
    }
}

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
        return SATIATOR_WRITE_ERR;
    data[0] = (val & 0xFF);
    if(satiatorWriteData(fd, &data, 1) < 0)
        return SATIATOR_WRITE_ERR;
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
    if(boot_disc() < 0)
    {
        s_mode(s_api);   // failed, restore order
        s_emulate("");  // close the old file
        //fadein(0x20);
        return SATIATOR_LAUNCH_ERR;
    } else
    {
        return SATIATOR_SUCCESS;
    }
}

// verify the image that is about to be loaded in the desc file needs patching
int satiatorVerifyPatchDescFileImage(const char * curRegion)
{
    centerTextVblank(21, "[>        ]");
    s_stat_t *st = (s_stat_t*)statbuf;
    int stret = s_stat(filenames[0], st, sizeof(statbuf)-1);
    if (stret < 0) {
        cdparse_set_error("Could not stat file #1");
        return -1;
    }
    centerTextVblank(21, "[>>       ]");
    const char *dot = strrchr(filenames[0], '.');
    if (!dot) {
        cdparse_set_error("Unrecognised file #1 extension - no dot in filename");
        return -1;
    }
    centerTextVblank(21, "[>>>      ]");

    const char *extension = dot + 1;
    int verifyLoc1 = 0x00;
    int verifyLoc2 = 0xdc0;
    int regionLoc2 = 0xe04;
    if (strncmp(extension, "iso", 3))
    {
        // bin files have a different location for the region flags
        verifyLoc1 = 0x10;
        verifyLoc2 = 0xf00;
        regionLoc2 = 0xf44;
    }
    
    int fp = s_open(filenames[0], FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    if (fp < 0) {
        cdparse_set_error("Could not open specified file #1");
        s_close(fp);
        return -1;
    }
    centerTextVblank(21, "[>>>>     ]");
    // VERIFY THE CD FOR PATCHING
    char checkStr[32];
    s_seek(fp, verifyLoc1, SEEK_SET);
    centerTextVblank(21, "[>>>>>    ]");

    s_read(fp, checkStr, 16);
    centerTextVblank(21, "[>>>>>>   ]");
    if(strncmp("SEGA SEGASATURN ", checkStr, 16)) {
        cdparse_set_error("Verify Failure 1=%s", checkStr);
        s_close(fp);
        return -1;
    }
    centerTextVblank(21, "[>>>>>>   ]");
    s_seek(fp, verifyLoc2, SEEK_SET);
    s_read(fp, checkStr, 16);
    if(strncmp("COPYRIGHT(C) SEG", checkStr, 16)) {
        cdparse_set_error("Could not verify image -2");
        cdparse_set_error("Verify Failure 2=%s", checkStr);
        s_close(fp);
        return -1;
    }
    centerTextVblank(21, "[>>>>>>>  ]");

    s_seek(fp, regionLoc2, SEEK_SET);
    s_read(fp, checkStr, 28);

    centerTextVblank(21, "[>>>>>>>> ]");
    char regionStr[32];
    strcpy(regionStr, "For ");
    strcat(regionStr, curRegion);
    strcat(regionStr, ".");
    checkStr[strlen(regionStr)] = '\0';
    while(strlen(regionStr) < 28)
    {
        strcat(regionStr, " ");
        strcat(checkStr, " ");
    }
    s_close(fp);
    centerTextVblank(21, " ");
    if(!strcmp(regionStr, checkStr))
    {
        centerTextVblank(20, "Adding To Recent History");
        // no patching needed
        addItemToRecentHistory();
        return 0;
    }
    return 1;
}

// patch the image that is about to be loaded in the desc file, should be ran after image2desc
// make sure you verify first
bool satiatorPatchDescFileImage(const char * curRegion)
{
    s_stat_t *st = (s_stat_t*)statbuf;
    int stret = s_stat(filenames[0], st, sizeof(statbuf)-1);
    if (stret < 0) {
        cdparse_set_error("Could not stat file #1");
        return false;
    }
    const char *dot = strrchr(filenames[0], '.');
    if (!dot) {
        cdparse_set_error("Unrecognised file #1 extension - no dot in filename");
        return false;
    }

    const char *extension = dot + 1;
    int regionLoc1 = 0x40;
    int regionLoc2 = 0xe04;
    if (strncmp(extension, "iso", 3))
    {
        // bin files have a different location for the region flags
        regionLoc1 = 0x50;
        regionLoc2 = 0xf44;
    }
    
    int fp = s_open(filenames[0], FA_READ | FA_WRITE | FA_OPEN_EXISTING);
    if (fp < 0) {
        cdparse_set_error("Could not open specified file #1");
        s_close(fp);
        return false;
    }
    
    // PATCH THE CD WITH THE NEW REGION
    char regionStr[32];
    strcpy(regionStr, "For ");
    strcat(regionStr, curRegion);
    strcat(regionStr, ".");
    while(strlen(regionStr) < 28)
    {
        strcat(regionStr, " ");
    }
    s_seek(fp, regionLoc1, SEEK_SET);
    char patchStr[17];
    patchStr[0] = regionStr[4];
    for(int i =0;i< 15;i++)
        patchStr[i + 1] = ' '; 
    patchStr[16] = '\0';
    s_write(fp, patchStr, 16);

    s_seek(fp, regionLoc2, SEEK_SET);
    s_write(fp, regionStr, 28);

    s_close(fp);
    return true;
}

// launch the orignal menu
enum SATIATOR_ERROR_CODE satiatorLaunchOriginalMenu()
{
    centerTextVblank(20, "Launching Menu");
    if(strcmp("/", currentDirectory))
        s_chdir("/");

    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat("menu.bin", st, sizeof(statbuf));
    if (fp < 0)
    {
        centerTextVblank(20, "Could Not Stat File");
        return SATIATOR_FILE_STAT_ERR;
    }

    fp = s_open("menu.bin", FA_READ | FA_OPEN_EXISTING);
    if (fp >= 0)
    {
        centerTextVblank(20, "Open Ok");
        char * buf = (char *)0x00200000;
        
        centerTextVblank(20, "Allocate Ok");
        int bytesRead = 0;
        s_seek(fp, 0x1000, SEEK_SET);
        int totalSize = (int)st->size - 0x1000;
        //int totalSize = (int)st->size;
        while(bytesRead < totalSize)
        {
            centerTextVblank(20, "Reading File %d%%", (int)(((double)bytesRead / (double)totalSize) * 100));
            int toRead = 1024;
            if(toRead > totalSize - bytesRead)
                toRead = totalSize - bytesRead;
            int b = s_read(fp, &buf[bytesRead], toRead);
            if(b <= 0)
            {
                s_close(fp);
                centerTextVblank(20, "Could Not Read File");
                return SATIATOR_READ_ERR;
            }
            bytesRead += b;
        }
        centerTextVblank(20, "Reading File 100%%");
        s_close(fp);
        s_mode(s_cdrom);
        centerTextVblank(20, "Jumping to %d bytes", bytesRead);

        // jump to the location
        (**(void(**)(void))0x00200000)();
        centerTextVblank(20, "Jumped");
        return SATIATOR_SUCCESS;
    }
    centerTextVblank(20, "Could Not Open File");
    return SATIATOR_OPEN_FILE_ERR;
}

// try launching a file, return an error if it fails
enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn)
{
    if (!strncmp(fn, "menu.bin", 8))
    {
        centerTextVblank(20, "Booting Menu");
        return satiatorLaunchOriginalMenu();
    }
    if (!strncmp(&fn[strlen(fn) - 5], ".desc", 5))
    {
        centerTextVblank(20, "Booting Disc");
        return satiatorEmulateDesc(fn);
    }
    int ret = image2desc(fn, "emu.desc");
    if (ret != SATIATOR_SUCCESS) {
        return SATIATOR_CREATE_DESC_ERR;
    }
    #if BIOS_BOOT
    centerTextVblank(20, "Verifying Region");
    ret = satiatorVerifyPatchDescFileImage(getRegionString());
    if(ret < 0)
    {
        return SATIATOR_PATCH_FAILURE;
    }
    if(ret == 1)
    {
        return SATIATOR_PATCH_REQUIRED;
    }
    #endif
    centerTextVblank(20, "Booting Disc");
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
    //if (!strncmp(&entry->name[len-5], ".desc", 5))
    //    return 1;
    return 0;
}

// Read one line of data with a maximum buffer size
char * s_gets(char *buf, uint32_t maxsize, int fd, uint32_t *bytesRead, uint32_t totalBytes)
{
    if(*bytesRead == totalBytes)
        return buf;
    char c[2];
    strcpy(buf, "");
    for(uint32_t i = 0;i< maxsize;i++)
    {
        int j = s_read(fd, &c, 1);
        *bytesRead = *bytesRead + 1;
        if(j < 0)
        {
            break;
        }
        c[1] = '\0';
        if(c[0] == '\n')
        {
            break;
        }
        if(c[0] == '\r')
            continue;
        strcat(buf, c);
        if(*bytesRead == totalBytes)
        {
            break;
        }
    }
    if(strlen(buf) < maxsize)
        buf[strlen(buf)] = '\0';
    return buf;
}

