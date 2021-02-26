#pragma once
#include <stdint.h>
#include "satiator/satiator.h"
#include "satiator/satiator-types.h"
#include "satiator/disc_format/cdparse.h"
#include "direntry.h"

enum SATIATOR_ERROR_CODE
{
    SATIATIOR_SUCCESS,
    SATIATIOR_OPEN_FILE_ERR,
    SATIATIOR_READ_ERR,
    SATIATIOR_WRITE_ERR,
    SATIATIOR_CREATE_DESC_ERR,
    SATIATIOR_FILE_STAT_ERR,
    SATIATIOR_FILE_EXT_ERR
};
extern int satiatorExecutableFilter(dirEntry *entry);

extern enum SATIATOR_ERROR_CODE satiatorTryLaunchFile(char * fn);
extern enum SATIATOR_ERROR_CODE satiatorWriteData(int fd, const void * buff, unsigned int size);
extern enum SATIATOR_ERROR_CODE satiatorWriteU8(int fd, uint8_t val);
extern enum SATIATOR_ERROR_CODE satiatorWriteU16(int fd, uint16_t val);
extern enum SATIATOR_ERROR_CODE satiatorWriteU32(int fd, uint32_t val);
extern char * s_gets(char *buf, int maxsize, int fd, uint32_t *bytesRead, uint32_t totalBytes);