#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "states/routine_states.h"
#include "satiator_functions.h"

bool writeItemShortcut(const char * ini, const char * fullpath)
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(ini, st, sizeof(statbuf));
    int size = -1;
    if(fp >= 0)
    {
        size = st->size;
        fp = s_open(ini, FA_WRITE | FA_READ | FA_OPEN_ALWAYS);
    } else
    {
        fp = s_open(ini, FA_WRITE | FA_CREATE_ALWAYS);
    }
    if (fp < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        return false;
    }

    if(size > -1)
        s_seek(fp, -5, SEEK_END); // 5 bytes puts us before the [END] tag
    else
        s_write(fp, "[START]\r\n", 9);
    s_write(fp, fullpath, strlen(fullpath));
    s_write(fp, "\r\n", 2);
    s_write(fp, "[END]", 5);
    s_close(fp);
    
    // change back to the current dir
    s_chdir(currentDirectory);
    return true;
}
bool itemIsInIni(const char * ini, const char * fullpath)
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(ini, st, sizeof(statbuf));
    if(fp < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        return false;
    }

    // open favs ini for reading
    fp = s_open(ini, FA_READ | FA_OPEN_ALWAYS);
    if (fp < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        return false;
    }
    // read  the file and verify if the selected game is there
    char * oneline = jo_malloc(256);
    uint32_t bytes;
    while(strncmp(oneline, "[START]", 7))
    {
        oneline = s_gets(oneline, 256, fp, &bytes, st->size);
    }
    oneline = s_gets(oneline, 256, fp, &bytes, st->size);
    while(strncmp(oneline, "[END]", 5))
    {
        if(!strncmp(oneline, fullpath, strlen(fullpath)))
        {
            s_close(fp);
            jo_free(oneline);
            // change back to the current dir
            s_chdir(currentDirectory);
            return true;
        }
        oneline = s_gets(oneline, 256, fp, &bytes, st->size);
    }
    s_close(fp);
    jo_free(oneline);

    // change back to the current dir
    s_chdir(currentDirectory);
    return false;
}
bool deleteSelectedShortcutFromIni(const char * ini)
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    // delete the old file
    s_unlink(ini);

    // open favs ini for writing
    int fp = s_open(ini, FA_WRITE | FA_CREATE_NEW);
    if (fp < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        return false;
    }
    
    s_write(fp, "[START]\r\n", 9);
    for(int i=0;i< dirEntyCount;i++)
    {
        if(i != selectedDirEntry)
        {
            s_write(fp, dirEntries[i].name, strlen(dirEntries[i].name));
            s_write(fp, "\r\n", 2);
        }
    }
    s_write(fp, "[END]", 5);
    s_close(fp);

    // change back to the current dir
    s_chdir(currentDirectory);
    return true;
}
void loadIniList(char * fn, bool sort)
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");
    strcpy(currentDirectory, "/satiator-rings");
    truncatedList = false;
    dirEntyCount = 0;
    selectedDirEntry = 0;
    listOffset = 0;

    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(fn, st, sizeof(statbuf));
    if (fp >=0)
    {
        fp = s_open(fn, FA_READ | FA_OPEN_EXISTING);
        if (fp >= 0) {
            char * oneline = jo_malloc(256);
            uint32_t bytes;
            while(strncmp(oneline, "[START]", 7))
            {
                oneline = s_gets(oneline, 256, fp, &bytes, st->size);
            }
            oneline = s_gets(oneline, 256, fp, &bytes, st->size);
            while(strncmp(oneline, "[END]", 5))
            {
                if(strlen(oneline) > 99)
                    continue;
                strcpy(dirEntries[dirEntyCount].name, oneline);
                if (oneline[strlen(oneline) - 4] == '.')
                    dirEntries[dirEntyCount].type = DIR_SHORTCUT_GAME;
                else
                    dirEntries[dirEntyCount].type = DIR_SHORTCUT_FOLDER;
                dirEntyCount++;
                if(dirEntyCount == MAX_LOADED_DIR_ENTRIES)
                {
                    truncatedList = true;
                    break;
                }
                oneline = s_gets(oneline, 256, fp, &bytes, st->size);
            }
            s_close(fp);
            jo_free(oneline);
        }
    }
    for(int i=dirEntyCount; i < MAX_LOADED_DIR_ENTRIES; i++)
    {
        dirEntries[i].type = DIR_NULL;
    }
    if(sort)
        sortDirEntries();
}