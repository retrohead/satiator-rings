#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "routine_states.h"
#include "gamelist.h"
#include "../satiator_functions.h"

#define GAME_LIST_MAX_ITEMS 20
#define GAME_LIST_MAX_ITEM_LEN 33

enum routine_state_types game_list_state = ROUTINE_STATE_INITIALIZE;

char currentDirectory[1024];
int dirEntyCount = 0;
int selectedEntry = 0;
int listOffset = 0;
int depth = 0;
bool truncatedList = true;
dirEntry dirEntries[MAX_LOADED_DIR_ENTRIES];

static int compareDirEntry(const void *pa, const void *pb) {
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

void displayGameListItem(char * name, int ypos, bool selected, enum dirEntryType type)
{
    if(selected)
        jo_nbg2_printf(0, ypos, "> ");
    else
        jo_nbg2_printf(0, ypos, "  ");
    char nam[256];
    strcpy(nam,name);
    if(nam[1] == '0')
        strcpy(nam, "/");
    nam[GAME_LIST_MAX_ITEM_LEN] = '\0'; // truncate to the max length
    if(type == DIR_DIRECTORY)
    {
        if(strlen(nam) >= GAME_LIST_MAX_ITEM_LEN)
            nam[GAME_LIST_MAX_ITEM_LEN - 1] = '\0';
        strcat(nam, "/");
    }
    while(strlen(nam) < GAME_LIST_MAX_ITEM_LEN)
        strcat(nam, " ");
    jo_nbg2_printf(2, ypos, nam);
}

void displayGameList()
{
    jo_nbg2_printf(0, 0, "GAMELIST                                   ");
    jo_nbg2_printf(0, 2, "%s                                         ", currentDirectory);

    for(int i=listOffset;i < listOffset + GAME_LIST_MAX_ITEMS;i++)
    {
        if(i >= dirEntyCount)
        {
            displayGameListItem("", (i - listOffset) + 4, false, dirEntries[i].type);
            continue;
        }
        displayGameListItem(dirEntries[i].name, (i - listOffset) + 4, i==selectedEntry, dirEntries[i].type);
    }
}

void loadGameList(char * directory, int (*filter)(dirEntry *entry))
{
    truncatedList = false;
    dirEntyCount = 0;
    selectedEntry = 0;
    listOffset = 0;
    if (s_opendir(directory))
            return;
    char statbuf[280];
    s_stat_t *st = (s_stat_t*)statbuf;
    int len;
    while ((len = s_stat(NULL, st, sizeof(statbuf)-1)) > 0) {
        st->name[len] = 0;
        // UNIX hidden files, except .. when present
        if (st->name[0] == '.' && strcmp(st->name, ".."))
            continue;
        // thanks Windows
        if (!strncmp(st->name, "System Volume Information", strlen("System Volume Information")))
            continue;
        strcpy(dirEntries[dirEntyCount].name, st->name);
        if (st->attrib & AM_DIR) {
            dirEntries[dirEntyCount].type = DIR_DIRECTORY;
        } else {
            strcpy(dirEntries[dirEntyCount].name, st->name);
            dirEntries[dirEntyCount].type = DIR_FILE;
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
        dirEntries[dirEntyCount].type = DIR_NULL;
    }
        
    sortDirEntries();
}

void launchSelectedGame()
{
    enum SATIATOR_ERROR_CODE ret = satiatorTryLaunchFile(dirEntries[selectedEntry].name);
    if(ret != SATIATIOR_SUCCESS)
        jo_nbg2_printf(0, 3, "iso2desc failed -%d", (int)ret);
}

void logic_gamelist()
{
    static enum prog_state_types exit_state = PROG_STATE_GAMELIST;

    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            strcpy(currentDirectory, ".");
            loadGameList(".", satiatorExecutableFilter);
            jo_nbg2_clear();
            jo_clear_background(JO_COLOR_White);
            displayGameList();
            game_list_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_SPLASH;
            break;
        case ROUTINE_STATE_RUN:

            if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
            {
                if(depth == 0)
                {
                    game_list_state = ROUTINE_STATE_END;
                    exit_state = PROG_STATE_SPLASH;
                    return;
                }
                depth--;
                int ret = s_chdir("..");
                if (ret != FR_OK) {
                    jo_nbg2_printf(0, 3, "Could not change dir to %s", dirEntries[selectedEntry].name);
                } else
                {
                    currentDirectory[(int)strlen(currentDirectory)-1] = '\0';
                    char * pos = strrchr(currentDirectory, '/');
                    if(pos != JO_NULL)
                        currentDirectory[pos - currentDirectory] = '\0';
                    else
                        strcpy(currentDirectory, ".");
                    loadGameList(".", satiatorExecutableFilter);
                    displayGameList();
                }
            }

            if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
            {
                switch(pad_controllers[0].direction_id)
                {
                    case LEFT:
                        break;
                    case RIGHT:
                        break;
                    case UP:
                        if(selectedEntry > 0)
                        {
                            selectedEntry--;
                            if(listOffset > 0)
                                listOffset--;
                        }
                        break;
                    case DOWN:
                        if(selectedEntry < dirEntyCount - 1)
                        {
                            selectedEntry ++;
                            if(selectedEntry >= GAME_LIST_MAX_ITEMS)
                                listOffset++;
                        }
                        break;
                }
                displayGameList();
            }

            if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
            {
                if(dirEntyCount > 0)
                {
                    if(dirEntries[selectedEntry].type == DIR_DIRECTORY)
                    {
                        if(currentDirectory[1] != '\0')
                        {
                            strcat(currentDirectory, "/");
                            strcat(currentDirectory, dirEntries[selectedEntry].name);
                        }
                        else
                            strcpy(currentDirectory, dirEntries[selectedEntry].name);
                        int ret = s_chdir(dirEntries[selectedEntry].name);
                        if (ret != FR_OK) {
                            currentDirectory[(int)strlen(currentDirectory)-1] = '\0';
                            currentDirectory[strstr("/" , currentDirectory) - currentDirectory] = '\0';
                            jo_nbg2_printf(0, 3, "Could not change dir to %s", dirEntries[selectedEntry].name);
                        } else
                        {
                            depth = depth + 1;
                            loadGameList(".", satiatorExecutableFilter);
                            displayGameList();
                        }
                    } else
                    {
                        launchSelectedGame();
                    }
                }
            }
            break;
        case ROUTINE_STATE_END:
            game_list_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}