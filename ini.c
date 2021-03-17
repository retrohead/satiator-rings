#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "ini.h"
#include "states/routine_states.h"
#include "satiator_functions.h"

bool loadIniListFirstLine(char * fn, char * destbuf)
{
    s_chdir("/satiator-rings");
    truncatedList = false;
    dirEntryCount = 0;
    selectedDirEntry = 0;
    listOffset = 0;

    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(fn, st, sizeof(statbuf));
    if (fp >=0)
    {
        fp = s_open(fn, FA_READ | FA_OPEN_EXISTING);
        if (fp >= 0) {
            char * oneline = jo_malloc(ONE_LINE_MAX_LEN);
            strcpy(oneline, "");
            uint32_t bytes;
            while(strncmp(oneline, "[START]", 7))
            {
                oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
            }
            oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
            strcpy(destbuf, oneline);
            jo_free(oneline);
            s_close(fp);
            s_chdir(currentDirectory);
            return true;
        }
    }
    s_chdir(currentDirectory);
    return false;
}
void writeIniList(char * fn, char * deleteEntry)
{
    s_chdir("/satiator-rings");
    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(fn, st, sizeof(statbuf));
    if (fp >=0)
        s_unlink(fn);
    fp = s_open(fn, FA_WRITE | FA_CREATE_NEW);
    s_write(fp, "[START]\r\n",9);
    for(int i=0;i<dirEntryCount;i++)
    {
        if((dirEntries[i].type == DIR_SHORTCUT_FOLDER) || (dirEntries[i].type == DIR_SHORTCUT_GAME))
        {
            if(deleteEntry != NULL)
            {
                if(!strcmp(deleteEntry, dirEntries[i].name))
                    continue;
            }
            s_write(fp, dirEntries[i].name, strlen(dirEntries[i].name));
            s_write(fp, "\r\n" ,2);
        }
    }
    s_write(fp, "[END]",5);
    s_close(fp);
    s_chdir(currentDirectory);
}
bool addItemToIni(char * ini, char * fn, bool addStart, bool keepList, bool sort)
{
    bool ret = loadIniList(ini, false, fn, addStart);
    writeIniList(ini, NULL);
    if(!keepList)
    {
        for(int i=0;i<MAX_LOADED_DIR_ENTRIES;i++)
        {
            if(dirEntries[i].name != NULL)
                jo_free(dirEntries[i].name);
        }
    }
    if(sort)
        sortDirEntries();
    return ret;
}
void addDirEntryItem(char * fn)
{
    if(dirEntries[dirEntryCount].name)
        jo_free(dirEntries[dirEntryCount].name);
    dirEntries[dirEntryCount].name = NULL;
    dirEntries[dirEntryCount].name = jo_malloc(strlen(fn) + 5);
    strcpy(dirEntries[dirEntryCount].name, fn);
    if (fn[strlen(fn) - 4] == '.')
        dirEntries[dirEntryCount].type = DIR_SHORTCUT_GAME;
    else
        dirEntries[dirEntryCount].type = DIR_SHORTCUT_FOLDER;
    dirEntryCount++;
}
bool loadIniList(char * fn, bool sort, char * addItemStr, bool addAtStart)
{
    bool ret = true;
    s_chdir("/satiator-rings");
    truncatedList = false;
    dirEntryCount = 0;
    selectedDirEntry = 0;
    listOffset = 0;

    bool addItem = false;
    if(strcmp(addItemStr, ""))
        addItem = true;
    if(addItem && addAtStart)
        addDirEntryItem(addItemStr);

    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(fn, st, sizeof(statbuf));
    if (fp >=0)
    {
        fp = s_open(fn, FA_READ | FA_OPEN_EXISTING);
        if (fp >= 0) {
            char * oneline = jo_malloc(ONE_LINE_MAX_LEN);
            strcpy(oneline, "");
            uint32_t bytes;
            while(strncmp(oneline, "[START]", 7))
            {
                oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
            }
            oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
            while(strncmp(oneline, "[END]", 5))
            {
                if(addItem && !strcmp(addItemStr, oneline))
                {
                    ret = false;
                    oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
                    continue;
                }
                addDirEntryItem(oneline);
                if(dirEntryCount == MAX_LOADED_DIR_ENTRIES / 4) // using a shorter list as shortcuts could be much longer
                {
                    truncatedList = true;
                    break;
                }
                oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
            }
            s_close(fp);
            jo_free(oneline);
        }
    }
    s_chdir(currentDirectory);
    if(addItem && !addAtStart && ret)
        addDirEntryItem(addItemStr);
    for(int i=dirEntryCount; i < MAX_LOADED_DIR_ENTRIES; i++)
    {
        if(dirEntries[i].name)
            jo_free(dirEntries[i].name);
        dirEntries[i].name = NULL;
        dirEntries[i].type = DIR_NULL;
    }
    if(sort)
        sortDirEntries();
    return ret;
}