#pragma once

#include <jo/jo.h>
#include "Save-Game-Copier/backends/backend.h"

#define MAX_SAVE_SLOTS 20
#define SAVE_FOLDERNAME "/satiator-saves"
#define SAVE_MAGICFILENAME "SAVRING"
#define SAVE_MAGICFILESIZE 0
#define SAVE_MAGICBLOCKSIZE 0
#define SAVE_MAXDIRPATHSIZE sizeof(SAVE_FOLDERNAME)+MAX_SAVE_COMMENT+10

#define COUNTOF(x) sizeof(x)/sizeof(x[0])

enum SAVE_ERROR_CODE
{
    SAVE_SUCCESS,
    SAVE_ERROR,
    SAVE_NO_GAMEID_FOUND,
    SAVE_OUT_OF_SPACE
};


extern void initSaves();
enum SAVE_ERROR_CODE saveBulkCopyBetweenDevices(int sourceDevice, int destinationDevice, char * gameid, int slot);
enum SAVE_ERROR_CODE saveBulkDeleteDevice(int targetDevice,char * gameid, int slot);
extern enum SATIATOR_ERROR_CODE saveCreateSaveDirectory(char * gameid, int slot);
extern enum SAVE_ERROR_CODE saveClearInternalMemory();
extern enum SAVE_ERROR_CODE saveClearSaveDirectory(char * gameid, int slot);
extern enum SAVE_ERROR_CODE saveCopySaveDirectoryToInternalMemory(char * gameid, int slot);
extern enum SAVE_ERROR_CODE saveCopyInternalMemoryToSaveDirectory(char * gameid, int slot);
extern enum SAVE_ERROR_CODE saveReadGameIdInInternalMemory(char * gameid, unsigned int * slot);
extern enum SAVE_ERROR_CODE saveSaveGameIdToInternalMemory(char * gameid, int slot);