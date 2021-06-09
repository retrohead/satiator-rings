#pragma once
#include <stdint.h>
#include "libsatiator/satiator.h"
#include "satiator/disc_format/cdparse.h"
#include "direntry.h"

enum SATIATOR_ERROR_CODE
{
    SATIATOR_SUCCESS,
    SATIATOR_OPEN_FILE_ERR,
    SATIATOR_READ_ERR,
    SATIATOR_WRITE_ERR,
    SATIATOR_CREATE_DESC_ERR,
    SATIATOR_FILE_STAT_ERR,
    SATIATOR_FILE_EXT_ERR,
    SATIATOR_LAUNCH_ERR,
    SATIATOR_PATCH_FAILURE,
    SATIATOR_PATCH_REQUIRED,
    SATIATOR_ALREADY_EXISTS
};
enum SATIATOR_STATE
{
    SATIATOR_STATE_WORKING,
    SATIATOR_STATE_NOT_FOUND,
    SATIATOR_STATE_NOT_WORKING
};
extern char statbuf[280];
extern char gameId[11];

extern enum SATIATOR_STATE satiatorState;
extern int satiatorExecutableFilter(dirEntry *entry);

extern void initSatiator();
extern enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn);
extern enum SATIATOR_ERROR_CODE satiatorWriteData(int fd, const void * buff, unsigned int size);
extern enum SATIATOR_ERROR_CODE satiatorWriteU8(int fd, uint8_t val);
extern enum SATIATOR_ERROR_CODE satiatorWriteU16(int fd, uint16_t val);
extern enum SATIATOR_ERROR_CODE satiatorWriteU32(int fd, uint32_t val);
extern enum SATIATOR_ERROR_CODE satiatorEmulateDesc(char * descfile);
extern enum SATIATOR_ERROR_CODE satiatorCreateDirectory(char * dir);
extern enum SATIATOR_ERROR_CODE satiatorPreparePerGameSRAM();
extern uint16_t satiatorReadU16(int fd);
extern int satiatorLaunchOriginalMenu();
extern char * s_gets(char *buf, uint32_t maxsize, int fd, uint32_t *bytesRead, uint32_t totalBytes);
extern bool satiatorPatchDescFileImage(const char * curRegion);