#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "states/routine_states.h"
#include "satiator_functions.h"

bool writeUniqueIniLineAtStart(const char * ini, const char * textline, int maxLines)
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    char * oldfilename = jo_malloc(strlen(ini) + 4);
    sprintf(oldfilename, "%s.bak", ini);

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(ini, st, sizeof(statbuf));
    if(fr >= 0)
    {
        // file already exists, open it for reading
        s_rename(ini, oldfilename);

        // open old ini for reading
        fr = s_open(oldfilename, FA_READ | FA_OPEN_EXISTING);
        if (fr < 0)
        {
            // failed change back to the current dir
            jo_free(oldfilename);
            s_chdir(currentDirectory);
            return false;
        }
    }
    // open new favs ini for writing
    int fw = s_open(ini, FA_WRITE | FA_CREATE_NEW);
    if (fw < 0)
    {
        // change back to the current dir
        jo_free(oldfilename);
        s_chdir(currentDirectory);
        return false;
    }
    s_write(fw, "[START]\r\n", 9);

    int lines = 1;
    if(fr >= 0)
    {
        char * oneline = jo_malloc(256);
        uint32_t bytes;
        // find the start tag
        while(strncmp(oneline, "[START]", 7))
            oneline = s_gets(oneline, 256, fr, &bytes, st->size);
        // write our new item at the top
        s_write(fw, textline, strlen(textline));
        s_write(fw, "\r\n", 2);
        // find the end tag and write everything inbetween unless it matches this line
        oneline = s_gets(oneline, 256, fr, &bytes, st->size);
        while(strncmp(oneline, "[END]", 5))
        {
            if(!strncmp(oneline, textline, strlen(textline)))
            {
                oneline = s_gets(oneline, 256, fr, &bytes, st->size);
                continue;
            }
            s_write(fw, oneline, strlen(oneline));
            s_write(fw, "\r\n", 2);
            lines++;
            if(lines >= maxLines)
                break;
            oneline = s_gets(oneline, 256, fr, &bytes, st->size);
        }
        jo_free(oneline);
    } else
    {
        // write our new item at the top, new file
        s_write(fw, textline, strlen(textline));
        s_write(fw, "\r\n", 2);
    }
    s_write(fw, "[END]", 5);
    s_close(fw);
    
    if(fr >= 0)
    {
        // close then delete the old file
        s_close(fr);
        s_unlink(oldfilename);
        jo_free(oldfilename);
    }

    // change back to the current dir
    s_chdir(currentDirectory);
    return true;
}
bool writeUniqueIniLineAtEnd(const char * ini, const char * textline, int maxLines)
{

    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    char * oldfilename = jo_malloc(strlen(ini) + 4);
    sprintf(oldfilename, "%s.bak", ini);

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(ini, st, sizeof(statbuf));
    if(fr >= 0)
    {
        // file already exists, open it for reading
        s_rename(ini, oldfilename);

        // open old ini for reading
        fr = s_open(oldfilename, FA_READ | FA_OPEN_EXISTING);
        if (fr < 0)
        {
            // failed change back to the current dir
            jo_free(oldfilename);
            s_chdir(currentDirectory);
            return false;
        }
    }
    // open new favs ini for writing
    int fw = s_open(ini, FA_WRITE | FA_CREATE_NEW);
    if (fw < 0)
    {
        // change back to the current dir
        jo_free(oldfilename);
        s_chdir(currentDirectory);
        return false;
    }
    s_write(fw, "[START]\r\n", 9);
    int lines = 1;
    if(fr >= 0)
    {
        char * oneline = jo_malloc(256);
        uint32_t bytes;
        // find the start tag
        while(strncmp(oneline, "[START]", 7))
            oneline = s_gets(oneline, 256, fr, &bytes, st->size);
        // find the end tag and write everything inbetween unless it matches this line
        oneline = s_gets(oneline, 256, fr, &bytes, st->size);
        while(strncmp(oneline, "[END]", 5))
        {
            if(!strncmp(oneline, textline, strlen(textline)))
            {
                oneline = s_gets(oneline, 256, fr, &bytes, st->size);
                continue;
            }
            s_write(fw, oneline, strlen(oneline));
            s_write(fw, "\r\n", 2);
            lines++;
            if(lines >= maxLines)
                break;
            oneline = s_gets(oneline, 256, fr, &bytes, st->size);
        }
        jo_free(oneline);
    }
    // write our new item at the end
    s_write(fw, textline, strlen(textline));
    s_write(fw, "\r\n", 2);
    s_write(fw, "[END]", 5);
    s_close(fw);
    
    if(fr >= 0)
    {
        // close then delete the old file
        s_close(fr);
        s_unlink(oldfilename);
        jo_free(oldfilename);
    }

    // change back to the current dir
    s_chdir(currentDirectory);
    return true;
}
bool lineIsInIni(const char * ini, const char * textline)
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
        if(!strncmp(oneline, textline, strlen(textline)))
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
bool deleteIniLine(const char * ini, const char * textline)
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(ini, st, sizeof(statbuf));
    if(fr < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        return false;
    }
    char * oldfilename = jo_malloc(strlen(ini) + 4);
    sprintf(oldfilename, "%s.bak", ini);
    s_rename(ini, oldfilename);

    // open old favs ini for reading
    fr = s_open(oldfilename, FA_READ | FA_OPEN_EXISTING);
    if (fr < 0)
    {
        // change back to the current dir
        jo_free(oldfilename);
        s_chdir(currentDirectory);
        return false;
    }
    // open new favs ini for writing
    int fw = s_open(ini, FA_WRITE | FA_CREATE_NEW);
    if (fw < 0)
    {
        // change back to the current dir
        jo_free(oldfilename);
        s_chdir(currentDirectory);
        return false;
    }

    // read  the file and verify if the selected game is there
    char * oneline = jo_malloc(256);
    uint32_t bytes;
    while(strncmp(oneline, "[START]", 7))
    {
        oneline = s_gets(oneline, 256, fr, &bytes, st->size);
    }
    s_write(fw, "[START]\r\n", 9);
    oneline = s_gets(oneline, 256, fr, &bytes, st->size);
    while(strncmp(oneline, "[END]", 5))
    {
        if(!strncmp(oneline, textline, strlen(textline)))
        {
            oneline = s_gets(oneline, 256, fr, &bytes, st->size);
            continue;
        }
        s_write(fw, oneline, strlen(oneline));
        s_write(fw, "\r\n", 2);
        oneline = s_gets(oneline, 256, fr, &bytes, st->size);
    }
    s_close(fr);
    s_write(fw, "[END]", 9);
    s_close(fw);
    // delete the old file
    s_unlink(oldfilename);
    jo_free(oneline);
    jo_free(oldfilename);
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