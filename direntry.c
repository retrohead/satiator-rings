#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "satiator_functions.h"
#include "sprite_manager.h"
#include "sound.h"
#include "gui.h"

int selectedDirEntry = 0;
char currentDirectory[1024];
int dirEntyCount = 0;
int listOffset = 0;
bool truncatedList = true;
int listScrolldelay = 0;
dirEntry dirEntries[MAX_LOADED_DIR_ENTRIES];

void initDirEntries()
{
    for(int i=0;i<MAX_LOADED_DIR_ENTRIES;i++)
    {
        dirEntries[i].name = NULL;
        dirEntries[i].type = DIR_NULL;
    }
}
int compareDirEntry(const void *pa, const void *pb) {
    const dirEntry *a = pa, *b = pb;
    if (a->type == DIR_DIRECTORY && b->type != DIR_DIRECTORY)
        return -1;
    if (a->type != DIR_DIRECTORY && b->type == DIR_DIRECTORY)
        return 1;
    if (a->type == DIR_NULL && b->type != DIR_NULL)
        return 1;
    if (a->type != DIR_NULL && b->type == DIR_NULL)
        return -1;
    return strcmp(a->name, b->name);
}
void sortDirEntries() {
    qsort(dirEntries, dirEntyCount, sizeof(dirEntry), compareDirEntry);
}
void loadFileList(char * directory, int (*filter)(dirEntry *entry))
{
    if(selectionSprite != -1)
    {
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 250;
    }
    truncatedList = false;
    dirEntyCount = 0;
    selectedDirEntry = 0;
    listOffset = 0;
    if (s_opendir(directory) != 0)
    {
            jo_nbg2_printf(0, 29, "could not open dir %s", directory);
            return;
    }
    s_stat_t *st = (s_stat_t*)statbuf;
    int len;
    while ((len = s_stat(NULL, st, sizeof(statbuf)-1)) > 0) {
        st->name[len] = 0;
        // UNIX hidden files, except .. when present
        if (st->name[0] == '.' && strcmp(st->name, ".."))
            continue;
        if(!strncmp(st->name, "satiator-rings", 14))
            continue;
        // thanks Windows
        if (!strncmp(st->name, "System Volume Information", 25))
            continue;
        if(dirEntries[dirEntyCount].name != NULL)
            jo_free(dirEntries[dirEntyCount].name);
        dirEntries[dirEntyCount].name = NULL;
        dirEntries[dirEntyCount].name = jo_malloc(strlen(st->name) + 5);
        strcpy(dirEntries[dirEntyCount].name, st->name);
        if (st->attrib & AM_DIR) {
            dirEntries[dirEntyCount].type = DIR_DIRECTORY;
        } else {
            strcpy(dirEntries[dirEntyCount].name, st->name);
            const char *dot = strrchr(dirEntries[dirEntyCount].name, '.');
            const char *extension = dot + 1;
            if (strncmp(extension, "cue", 3) && strncmp(extension, "iso", 3))
                dirEntries[dirEntyCount].type = DIR_FILE;
            else
                dirEntries[dirEntyCount].type = DIR_GAME;
        }

        if (filter && !filter(&dirEntries[dirEntyCount]))
            continue;

        if (!dirEntries[dirEntyCount].name)
        {
            return;
        }

        dirEntyCount++;
        if(dirEntyCount == MAX_LOADED_DIR_ENTRIES)
        {
            truncatedList = true;
            break;
        }
    }
    for(int i=dirEntyCount; i < MAX_LOADED_DIR_ENTRIES; i++)
    {
        if(dirEntries[i].name != NULL)
            jo_free(dirEntries[i].name);
        dirEntries[i].name = NULL;
        dirEntries[i].type = DIR_NULL;
    }
    sortDirEntries();
}
void moveDirEntrySelectionUp(int maxlistItems, int sfx, bool shortSelectionItem)
{
    if(selectedDirEntry > 0)
    {
        selectedDirEntry--;
        if(selectedDirEntry - listOffset  < 0)
            listOffset--;
        
    } else
    {
        selectedDirEntry = dirEntyCount - 1;
        listOffset = dirEntyCount - maxlistItems;
        if(listOffset < 0)
            listOffset = 0;
    }
    playSfx(sfx, false);
    updateSelectionSprite(selectedDirEntry - listOffset + 5, shortSelectionItem);
    draw_sprites();
    slSynch();
}
void moveDirEntrySelectionDown(int maxlistItems, int sfx, bool shortSelectionItem)
{
    if(selectedDirEntry < dirEntyCount - 1)
    {
        selectedDirEntry ++;
        if(selectedDirEntry - listOffset >= maxlistItems)
            listOffset++;
    } else
    {
        selectedDirEntry = 0;
        listOffset = 0;
    }
    playSfx(sfx, false);
    updateSelectionSprite(selectedDirEntry - listOffset + 5, shortSelectionItem);
    draw_sprites();
    slSynch();
}