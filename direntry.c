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
int dirEntryCount = 0;
int listOffset = 0;
bool truncatedList = true;
int listScrolldelay = 0;

bool textLeft = false;
int textScrollX = 0;
uint8_t textScrollDelay = 0;
dirEntry dirEntries[MAX_LOADED_DIR_ENTRIES];
int maxListLen = 38;

void setMaxListLength(int maxlength)
{
    maxListLen = maxlength;
}
void resetTextScroll()
{
    textScrollX = 0;
    textScrollDelay = TEXT_SCROLL_DELAY;
    textLeft = false;
}
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
    char * fn_a = strrchr(a->name, '/');
    if(!fn_a)
        fn_a = a->name;
    char * fn_b = strrchr(b->name, '/');
    if(!fn_b)
        fn_b = b->name;
    return strcmp(fn_a, fn_b);
}
void sortDirEntries(){
    qsort(dirEntries, dirEntryCount, sizeof(dirEntry), compareDirEntry);
}
void loadFileList(char * directory, int (*filter)(dirEntry *entry))
{
    if(selectionSprite != -1)
    {
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 250;
    }
    truncatedList = false;
    dirEntryCount = 0;
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
        if(dirEntries[dirEntryCount].name != NULL)
            jo_free(dirEntries[dirEntryCount].name);
        dirEntries[dirEntryCount].name = NULL;
        dirEntries[dirEntryCount].name = jo_malloc(strlen(st->name) + 5);
        strcpy(dirEntries[dirEntryCount].name, st->name);
        if (st->attrib & AM_DIR) {
            dirEntries[dirEntryCount].type = DIR_DIRECTORY;
        } else {
            strcpy(dirEntries[dirEntryCount].name, st->name);
            const char *dot = strrchr(dirEntries[dirEntryCount].name, '.');
            const char *extension = dot + 1;
            if (strncmp(extension, "cue", 3) && strncmp(extension, "iso", 3))
                dirEntries[dirEntryCount].type = DIR_FILE;
            else
                dirEntries[dirEntryCount].type = DIR_GAME;
        }

        if (filter && !filter(&dirEntries[dirEntryCount]))
            continue;

        if (!dirEntries[dirEntryCount].name)
        {
            return;
        }

        dirEntryCount++;
        if(dirEntryCount == MAX_LOADED_DIR_ENTRIES)
        {
            truncatedList = true;
            break;
        }
    }
    for(int i=dirEntryCount; i < MAX_LOADED_DIR_ENTRIES; i++)
    {
        if(dirEntries[i].name != NULL)
            jo_free(dirEntries[i].name);
        dirEntries[i].name = NULL;
        dirEntries[i].type = DIR_NULL;
    }
    sortDirEntries();
}
void moveDirEntrySelectionUp(int maxlistItems, int sfx, bool shortSelectionItem, bool drawSprites)
{
    if(selectedDirEntry > 0)
    {
        selectedDirEntry--;
        if(selectedDirEntry - listOffset  < 0)
            listOffset--;
        
    } else
    {
        selectedDirEntry = dirEntryCount - 1;
        listOffset = dirEntryCount - maxlistItems;
        if(listOffset < 0)
            listOffset = 0;
    }
    if(drawSprites)
    {
        playSfx(sfx, false);
        updateSelectionSprite(selectedDirEntry - listOffset + 5, shortSelectionItem);
        draw_sprites();
        slSynch();
    }
}
void moveDirEntrySelectionDown(int maxlistItems, int sfx, bool shortSelectionItem, bool drawSprites)
{
    if(selectedDirEntry < dirEntryCount - 1)
    {
        selectedDirEntry ++;
        if(selectedDirEntry - listOffset >= maxlistItems)
            listOffset++;
    } else
    {
        selectedDirEntry = 0;
        listOffset = 0;
    }
    if(drawSprites)
    {
        playSfx(sfx, false);
        updateSelectionSprite(selectedDirEntry - listOffset + 5, shortSelectionItem);
        draw_sprites();
        slSynch();
    }
}

void displayDirListItemText(char * nam, int ypos, bool selected, enum dirEntryType type, bool triggersHeld)
{
    if(selected)
    {
        int len = strlen(nam);
        if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
            len++;
        if(!triggersHeld)
        {
            if(len >= maxListLen)
            {
                textScrollDelay--;
                if(textScrollDelay == 0)
                {
                    if(textLeft)
                        textScrollX--;
                    else
                        textScrollX++;
                    textScrollDelay = TEXT_SCROLL_DELAY;
                    if(textLeft)
                    {
                        if(textScrollX == 0)
                            textLeft = false;
                    } else
                    {
                        if(maxListLen + textScrollX >= len)
                            textLeft = true;
                    }
                }
                char * scrollPos = &nam[textScrollX];
                strcpy(nam, scrollPos);
            }
        }
    }
    nam[maxListLen] = '\0'; // truncate to the max length
    if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
    {
        if((int)strlen(nam) >= maxListLen - 1)
            nam[maxListLen - 1] = '\0';
    }
    int len = strlen(nam);
    while(len < 39) // clear the rest of the list
    {
        strcat(nam, " ");
        len++;
    }
    jo_nbg2_printf(1, ypos, "%s", nam);
}
void displayDirListItem(const char * name, int ypos, bool selected, enum dirEntryType type, bool triggersHeld)
{
    char nam[1024];
    strcpy(nam, name);
    if(type == DIR_NULL)
        strcpy(nam,"");
    if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER) || (type == DIR_SHORTCUT_GAME))
    {
        // trim the leading directory
        const char * lastSlash = strrchr(nam, '/');
        if(lastSlash)
        {
            const char * fileName = lastSlash + 1;
            strcpy(nam, fileName);
        }
    }
    displayDirListItemText(nam, ypos, selected, type, triggersHeld);
}