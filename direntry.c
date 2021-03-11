#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "direntry.h"

int selectedDirEntry = 0;
char currentDirectory[1024];
int dirEntyCount = 0;
int listOffset = 0;
bool truncatedList = true;
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