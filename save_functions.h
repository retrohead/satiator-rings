#pragma once

#include <jo/jo.h>
#include "Save-Game-Copier/backends/backend.h"

#define SAVE_FOLDERNAME "/satiator-saves"
#define SAVE_MAGICFILENAME "SAVRING"
#define SAVE_MAGICFILESIZE 52
#define SAVE_MAXDIRPATHSIZE sizeof(SAVE_FOLDERNAME)+MAX_SAVE_COMMENT // the appended slash and null termination byte is accounted for as sizeof and MAX_SAVE_COMMENT include an additional byte for null term

#define COUNTOF(x) sizeof(x)/sizeof(x[0])

enum SAVE_ERROR_CODE
{
    SAVE_SUCCESS,
    SAVE_ERROR,
    SAVE_NO_GAMEID_FOUND,
    SAVE_OUT_OF_SPACE
};


extern void initSaves();
enum SAVE_ERROR_CODE saveBulkCopyBetweenDevices(int sourceDevice, int destinationDevice, char * gameid);
enum SAVE_ERROR_CODE saveBulkDeleteDevice(int targetDevice,char * gameid);
extern enum SATIATOR_ERROR_CODE saveCreateSaveDirectory(char * gameid);
extern enum SAVE_ERROR_CODE saveClearInternalMemory();
extern enum SAVE_ERROR_CODE saveClearSaveDirectory(char * gameid);
extern enum SAVE_ERROR_CODE saveCopySaveDirectoryToInternalMemory(char * gameid);
extern enum SAVE_ERROR_CODE saveCopyInternalMemoryToSaveDirectory(char * gameid);
extern enum SAVE_ERROR_CODE saveReadGameIdInInternalMemory(char * gameid);
extern enum SAVE_ERROR_CODE saveSaveGameIdToInternalMemory(char * gameid);