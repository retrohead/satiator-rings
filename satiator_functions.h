#pragma once
#include <stdint.h>
#include "satiator/satiator.h"
#include "satiator/satiator-types.h"
#include "direntry.h"

enum SATIATOR_ERROR_CODE
{
    SATIATIOR_SUCCESS,
    SATIATIOR_OPEN_FILE_ERR,
    SATIATIOR_READ_ERR,
    SATIATIOR_WRITE_ERR,
    SATIATIOR_CREATE_DESC_ERR,
    SATIATIOR_FILE_STAT_ERR
};
extern uint16_t vdp1_stash[0x40000];
extern int satiatorExecutableFilter(dirEntry *entry);

extern enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn);
extern enum SATIATOR_ERROR_CODE satiatorWriteData(int fd, const void * buff, unsigned int size);
extern enum SATIATOR_ERROR_CODE satiatorWriteU8(int fd, uint8_t val);
extern enum SATIATOR_ERROR_CODE satiatorWriteU16(int fd, uint16_t val);
extern enum SATIATOR_ERROR_CODE satiatorWriteU32(int fd, uint32_t val);