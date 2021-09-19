#include <jo/jo.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "bios.h"
#include "satiator/iapetus/iapetus.h"
#include "satiator/disc_format/cdparse.h"
#include "satiator/jhloader.h"
#include "satiator_functions.h"
#include "save_functions.h"
#include "debug.h"
#include "options_file.h" // needed for desc file caching

enum SATIATOR_STATE satiatorState = SATIATOR_STATE_NOT_FOUND;
extern void addItemToRecentHistory();
int patchModeRequired = 0;
char statbuf[280];
char gameId[11];  // needed for per-game saves

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
            satiatorCreateDirectory(SAVE_FOLDERNAME);     
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

uint16_t satiatorReadU16(int fd)
{
    uint16_t val = 0;

    // byte flipped
    uint8_t data[2];
    s_read(fd, (char *)data, 2);
    val = data[0] | (data[1] << 8);
    return val;
}

bool satiatorLoadFirstFileInDesc(const char * filename)
{
    s_stat_t *st = (s_stat_t*)statbuf;
    int stret = s_stat(filename, st, sizeof(statbuf)-1);
    if (stret < 0) {
        cdparse_set_error("Could not stat desc file");
        return false;
    }
    int fr = s_open(filename, FA_READ|FA_OPEN_EXISTING);
    if (fr < 0) {
        cdparse_set_error("Could not open specified file");
        s_close(fr);
        return false;
    }
    uint16_t nseg = satiatorReadU16(fr);
    uint32_t bytesRead = 2 + nseg * sizeof(seg_desc_t);
    s_seek(fr, bytesRead, SEEK_SET);
    uint8_t namelen;
    s_read(fr, &namelen, 1);
    s_read(fr, filenames[0], namelen);
    filenames[0][namelen] = '\0';
    s_close(fr);
    return true;
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

// verify the image that is about to be loaded in the desc file needs patching, returns 1 if its at 0x00 or 2 if its at 0x10 <--Not true, fix this comment
// return the 10 character gameId as a null-terminated 11 byte string
int satiatorVerifyPatchDescFileImage(const char * curRegion, char * outGameId)
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
    int verifyLoc1 = 0x00;
    int gameidLoc = 0x20;
    int verifyLoc2 = 0xdc0;
    int regionLoc2 = 0xe04;
    patchModeRequired = 0;
    
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
        // try the 2nd veryify location
        verifyLoc1 = 0x10;
        gameidLoc = 0x30;
        verifyLoc2 = 0xf00;
        regionLoc2 = 0xf44;
        patchModeRequired = 1;
        s_seek(fp, verifyLoc1, SEEK_SET);
        s_read(fp, checkStr, 16);

        if(strncmp("SEGA SEGASATURN ", checkStr, 16)) {
            cdparse_set_error("Verify Failure 1=%s", checkStr);
            s_close(fp);
            return -1;
        }
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
    //extract gameid and trim before returning
    char rawGameId[MAX_SAVE_COMMENT];
    char * firstSpace;
    rawGameId[sizeof(rawGameId)-1] = '\0'; //null terminate raw buffer
    s_seek(fp, gameidLoc, SEEK_SET);
    s_read(fp, rawGameId, 10);
    firstSpace = strchr(rawGameId,' '); 
    if (firstSpace != NULL) {
        *firstSpace = '\0'; //null terminate at the first space
    }
    strcpy(outGameId,rawGameId);

    s_close(fp);
    centerTextVblank(21, " ");
    if(!strcmp(regionStr, checkStr))
    {
        centerTextVblank(20, "Adding To Recent History");
        // no patching needed
        addItemToRecentHistory();
        
        if(options[OPTIONS_PERGAME_SAVE] >= 1)
        {
            centerTextVblank(20, "Imaging Per-Game Save Memory to Console");
            satiatorPreparePerGameSRAM(options[OPTIONS_PERGAME_SAVE] -1);
        }
        return 0;
    }
    return 1;
}

// patch the image that is about to be loaded in the desc file, should be ran after image2desc
// make sure you verify first to set the patch mode
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
    int regionLoc1 = 0x40;
    int regionLoc2 = 0xe04;
    if (patchModeRequired == 1)
    {
        // some files have a different location for the region flags
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

enum SATIATOR_ERROR_CODE satiatorPreparePerGameSRAM(int slot)
{
    enum SATIATOR_ERROR_CODE ret;
    
    centerTextVblank(20, "Backing up current SRAM"); // double vblank to try and address the satiator hanging during sd ops
    centerTextVblank(20, "Backing up current SRAM"); 
    ret = saveCreateSaveDirectory("_BACKUP", 0);
    if (ret == SATIATOR_WRITE_ERR) 
    {
        jo_nbg2_printf(2,  22,"Error: saveCreateSaveDirectory - Out of Space?");
        return ret;
    }
    ret = saveCopyInternalMemoryToSaveDirectory("_BACKUP", 0);
    if ((int)ret != SAVE_SUCCESS)
    {
        jo_nbg2_printf(2,  22,"Error: saveCopyInternalMemoryToSaveDirectory");
        return ret;
    }

    centerTextVblank(20, "Clearing SRAM");
    ret = saveClearInternalMemory();
    if ((int)ret != SAVE_SUCCESS)
    {
        jo_nbg2_printf(2,  22,"Error: saveClearInternalMemory");
        return ret;
    }
    
    centerTextVblank(20, "Creating Game Save Directory"); // double vblank to try and address the satiator hanging during sd ops
    centerTextVblank(20, "Creating Game Save Directory");
    ret = saveCreateSaveDirectory(gameId, slot);
    if(ret == SATIATOR_ALREADY_EXISTS) //directory already exists, copy existing BUPs to SRAM
    {
        centerTextVblank(20, "Directory Exists, Copying Previous Saves To SRAM"); // double vblank to try and address the satiator hanging during sd ops
        centerTextVblank(20, "Directory Exists, Copying Previous Saves To SRAM");
        ret = saveCopySaveDirectoryToInternalMemory(gameId, slot);
        if(ret != SATIATOR_SUCCESS)
        {
            jo_nbg2_printf(2,  22,"Error: saveCopySaveDirectoryToInternalMemory");
            return ret;
        }
    }
    centerTextVblank(20, "Marking SRAM with active GameID");
    ret = saveSaveGameIdToInternalMemory(gameId, slot);
    if ((int)ret != SAVE_SUCCESS)
    {
            jo_nbg2_printf(2,  22,"Error: saveSaveGameIdToInternalMemory");
            return ret;
    }
    return SATIATOR_SUCCESS;
}

// launch the orignal menu
int satiatorLaunchOriginalMenu()
{
    s_chdir("/");
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat("menu.bin", st, sizeof(statbuf)-1);
    if (fr < 0)
        return -1;
    char * data = (char *)0x200000;
    // try open the file
    fr = s_open("menu.bin", FA_READ | FA_OPEN_EXISTING);
    if (fr < 0)
    {
        s_chdir(currentDirectory);
        return -1;
    }
    s_seek(fr, 0x1000, SEEK_SET);
    uint32_t readBytes = 0x1000;
    while(readBytes < st->size)
    {
        uint32_t readSize = S_MAXBUF;
        if(st->size - readBytes < readSize)
            readSize = st->size - readBytes;
        int ret = s_read(fr, data + (readBytes - 0x1000), readSize);
        if(ret != (int)readSize)
        {
            s_close(fr);
            return -1;
        }
        readBytes += readSize;
    }
    s_close(fr);
    //((void(*)(void))0x200000)();
    void (*entry)(uint32_t) = (void*)0x200000;
    entry(S_BOOT_NO_AUTOLOAD);
    return 1; // should never get here
}
// try launching a file, return an error if it fails, make sure filename[0] contains the file with the region data before running this
enum SATIATOR_ERROR_CODE satiatorLaunchDescFile(char * fn)
{
    #if BIOS_BOOT
    centerTextVblank(20, "Verifying Region");
    int ret = satiatorVerifyPatchDescFileImage(getRegionString(),gameId);
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
    return satiatorEmulateDesc(fn);
}

void elf_launch(const char *filename);
extern const char *elf_error;
enum SATIATOR_ERROR_CODE satiatorTryLaunchELF(char * fn)
{
    elf_launch(fn);
    if(elf_error)
    {
        cdparse_set_error(elf_error);
        return SATIATOR_LAUNCH_ERR;
    }
    return SATIATOR_SUCCESS; // should never get here
}
enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn)
{
    if (!strncmp(fn, "origmenu", 8))
    {
        centerTextVblank(20, "Booting Menu");
        return satiatorLaunchOriginalMenu();
    }else if (!strncasecmp(&fn[strlen(fn) - 4], ".elf", 4))
    {
        return satiatorTryLaunchELF(fn);
    } else if (!strncasecmp(&fn[strlen(fn) - 5], ".desc", 5))
    {
        if(satiatorLoadFirstFileInDesc(fn))
            return satiatorLaunchDescFile(fn);
        else
            return SATIATOR_LAUNCH_ERR;
    } else
    {
        if(options[OPTIONS_DESC_CACHE] == 1)
        {
            // see if a desc file exists in this directory already
                s_stat_t *st = (s_stat_t*)statbuf;
                int stret = s_stat("emu.desc", st, sizeof(statbuf)-1);
                if (stret < 0)
                {
                    cdparse_set_error("no desc file present");
                } else
                {
                    enum SATIATOR_ERROR_CODE launchCode = satiatorTryLaunchFile("emu.desc");
                    if(launchCode != SATIATOR_LAUNCH_ERR)
                        return launchCode;
                    cdparse_set_error("defaulting to image");
                }
        }
        int ret = image2desc(fn, "emu.desc");
        if (ret != SATIATOR_SUCCESS) {
            return SATIATOR_CREATE_DESC_ERR;
        }
    }
    return satiatorLaunchDescFile("emu.desc");
}

int satiatorExecutableFilter(dirEntry *entry) {
    if (entry->type == DIR_DIRECTORY)
        return 1;

    int len = strlen(entry->name);
    if (!strncasecmp(&entry->name[len-4], ".cue", 4))
        return 1;
    if (!strncasecmp(&entry->name[len-4], ".iso", 4))
        return 1;
    if (!strncasecmp(&entry->name[len-4], ".elf", 4))
        return 1;
    return 0;
}

// Read one line of data with a maximum buffer size
char * s_gets(char *buf, uint32_t maxsize, int fd, uint32_t *bytesRead, uint32_t totalBytes)
{
    if(*bytesRead >= totalBytes)
        return buf;
    char c[2];
    strcpy(buf, "");
    for(uint32_t i = 0;i< maxsize;i++)
    {
        int j = s_read(fd, &c, 1);
        *bytesRead = *bytesRead + 1;
        if(j <= 0)
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
        if(*bytesRead >= totalBytes)
        {
            break;
        }
    }
    if(strlen(buf) < maxsize)
        buf[strlen(buf)] = '\0';
    return buf;
}

