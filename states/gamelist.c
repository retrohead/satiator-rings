#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include "../main.h"
#include "routine_states.h"
#include "../satiator_functions.h"

#define GAME_LIST_MAX_ITEMS 20
#define GAME_LIST_MAX_ITEM_LEN 25
#define TEXT_SCROLL_DELAY 15 // higher = slower, max 255

enum routine_state_types game_list_state = ROUTINE_STATE_INITIALIZE;

int listOffset = 0;
bool textLeft = false;
bool truncatedList = true;

char gameIdStr[50];
int gameBoxTex;
int gameBoxSprite;
uint8_t textScrollDelay = 0;

int textScrollX = 0;

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

void displayGameListItem(const char * name, int ypos, bool selected, enum dirEntryType type)
{
    char nam[256];
    strcpy(nam,name);
    if(type == DIR_DIRECTORY)
    {
        // trim the ID from the folder name
        char * pos = strrchr(nam, '[');
        if(pos != JO_NULL)
            nam[pos - nam - 1] = '\0';
    }
    if(selected)
    {
        jo_nbg2_printf(1, ypos, "> ");
        int len = strlen(nam);
        if(type == DIR_DIRECTORY)
            len++;
        if(len >= GAME_LIST_MAX_ITEM_LEN)
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
                    if(GAME_LIST_MAX_ITEM_LEN + textScrollX >= len)
                        textLeft = true;
                }
            }
            char * scrollPos = &nam[textScrollX];
            strcpy(nam, scrollPos);
        }
    }
    else
        jo_nbg2_printf(1, ypos, "  ");
    if((nam[0] != '\0') && (nam[1] == '\0'))
        strcpy(nam, "/");
    nam[GAME_LIST_MAX_ITEM_LEN] = '\0'; // truncate to the max length
    if(type == DIR_DIRECTORY)
    {
        strcat(nam, "/");
        if(strlen(nam) >= GAME_LIST_MAX_ITEM_LEN)
            nam[GAME_LIST_MAX_ITEM_LEN - 1] = '\0';
    }
    while(strlen(nam) < GAME_LIST_MAX_ITEM_LEN)
        strcat(nam, " ");
    jo_nbg2_printf(3, ypos, nam);
}

void displayGameBox(char * fn)
{
    if(strlen(fn) > 1)
    {
        if(gameBoxTex != -1)
        {
            jo_sprite_free_from(spriteTex[gameBoxTex].texture_id);
            spriteTex[gameBoxTex].used = false;
            sprites[gameBoxSprite].used = false;
        }
        gameBoxTex = load_sprite_texture("BOX", fn);
        if(gameBoxTex > 0)
        {
            gameBoxSprite = create_sprite(gameBoxTex, 315 - getTextureWidth(gameBoxTex), 235 - getTextureHeight(gameBoxTex), 0, 1.0, 1.0, 0);
        }
    } else if(gameBoxTex != -1)
    {
        jo_sprite_free_from(spriteTex[gameBoxTex].texture_id);
        spriteTex[gameBoxTex].used = false;
        sprites[gameBoxSprite].used = false;
    }
}

void displayGameList()
{
    textScrollX = 0;
    textScrollDelay = TEXT_SCROLL_DELAY;
    textLeft = false;
    for(int i=listOffset;i < listOffset + GAME_LIST_MAX_ITEMS;i++)
    {
        if(i >= dirEntyCount)
        {
            displayGameListItem("", (i - listOffset) + 8, false, dirEntries[i].type);
            continue;
        }
        displayGameListItem(dirEntries[i].name, (i - listOffset) + 8, i==selectedDirEntry, dirEntries[i].type);
    }
    if(dirEntries[selectedDirEntry].type == DIR_DIRECTORY)
    {
        const char *brckt = strrchr(dirEntries[selectedDirEntry].name, '[');
        if (brckt) {
            const char *gameidpos = brckt + 1;
            char gameId[50];
            strcpy(gameId, gameidpos);
            gameId[strlen(gameId) - 1] = '\0'; // strip ending bracket

            if(strcmp(gameId, gameIdStr))
            {
                strcpy(gameIdStr, gameId);
                strcat(gameId, "S.TGA");
                displayGameBox(gameId);
            }
        } else
        {
            strcpy(gameIdStr, "");
            displayGameBox("");
        }

    } else
    {
        strcpy(gameIdStr, "");
        displayGameBox("");
    }
}

void loadFileList(char * directory, int (*filter)(dirEntry *entry))
{
    truncatedList = false;
    dirEntyCount = 0;
    selectedDirEntry = 0;
    listOffset = 0;
    if (s_opendir(directory))
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
        // thanks Windows
        if (!strncmp(st->name, "System Volume Information", strlen("System Volume Information")))
            continue;
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
        dirEntries[i].type = DIR_NULL;
    }
        
    sortDirEntries();
}

void logic_gamelist()
{
    static enum prog_state_types exit_state = PROG_STATE_SPLASH;
    static int depth = 0;
    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            gameBoxTex = -1;
            gameBoxSprite = -1;
            strcpy(gameIdStr, "");
            loadFileList(".", satiatorExecutableFilter);
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);
            displayGameList();
            game_list_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_SPLASH;
            break;
        case ROUTINE_STATE_RUN:
            jo_nbg2_printf(15, 4, "GAMELIST");
            if(dt.second % 2 == 0)
                jo_nbg2_printf(33, 4, "%02d %02d", dt.hour, dt.minute);
            else
                jo_nbg2_printf(33, 4, "%02d:%02d", dt.hour, dt.minute);
            jo_nbg2_printf(1, 6, "%s                                         ", currentDirectory);
            if(dirEntyCount > 0)
            {
                displayGameListItem(dirEntries[selectedDirEntry].name, (selectedDirEntry - listOffset) + 8, true, dirEntries[selectedDirEntry].type);
            }
            if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
            {
                if(depth == 0)
                {
                    game_list_state = ROUTINE_STATE_END;
                    exit_state = PROG_STATE_SPLASH;
                    return;
                }
                char * pos = strrchr(currentDirectory, '/');
                if(pos != JO_NULL)
                    currentDirectory[pos - currentDirectory] = '\0';
                if(currentDirectory[0] == '\0')
                    strcpy(currentDirectory, "/");
                int ret = s_chdir(currentDirectory);
                if (ret != FR_OK) {
                    jo_nbg2_printf(0, 29, "Could not change dir to parent");
                } else
                {
                    depth--;
                    loadFileList(".", satiatorExecutableFilter);
                    displayGameList();
                }
            }
            if(pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS)
            {
                game_list_state = ROUTINE_STATE_END;
                exit_state = PROG_STATE_MENU;
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
                        if(selectedDirEntry > 0)
                        {
                            selectedDirEntry--;
                            if(selectedDirEntry - listOffset  < 0)
                                listOffset--;
                        }
                        break;
                    case DOWN:
                        if(selectedDirEntry < dirEntyCount - 1)
                        {
                            selectedDirEntry ++;
                            if(selectedDirEntry - listOffset >= GAME_LIST_MAX_ITEMS)
                                listOffset++;
                        }
                        break;
                }
                displayGameList();
            }
            if(pad_controllers[0].btn_l == BUTTON_STATE_HELD)
            {
                if(selectedDirEntry > 0)
                {
                    selectedDirEntry--;
                    if(selectedDirEntry - listOffset  < 0)
                        listOffset--;
                }
                displayGameList();
            }
            if(pad_controllers[0].btn_r == BUTTON_STATE_HELD)
            {
                if(selectedDirEntry < dirEntyCount - 1)
                {
                    selectedDirEntry ++;
                    if(selectedDirEntry - listOffset >= GAME_LIST_MAX_ITEMS)
                        listOffset++;
                }
                displayGameList();
            }

            if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
            {
                if(dirEntyCount > 0)
                {
                    if(dirEntries[selectedDirEntry].type == DIR_DIRECTORY)
                    {
                        if(strlen(currentDirectory) > 1)
                            strcat(currentDirectory, "/");
                        strcat(currentDirectory, dirEntries[selectedDirEntry].name);
                        int ret = s_chdir(currentDirectory);
                        if (ret != FR_OK) {
                            jo_nbg2_printf(0, 29, "failed open %s", currentDirectory);
                            currentDirectory[strstr("/" , currentDirectory) - currentDirectory] = '\0';
                        } else
                        {
                            depth = depth + 1;
                            loadFileList(".", satiatorExecutableFilter);
                            displayGameList();
                            if((dirEntyCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
                            {
                                //launchSelectedGame();
                                game_list_state = ROUTINE_STATE_END;
                                exit_state = PROG_STATE_BOOT;
                            }
                        }
                    } else if(dirEntries[selectedDirEntry].type == DIR_GAME)
                    {
                        //launchSelectedGame();
                        game_list_state = ROUTINE_STATE_END;
                        exit_state = PROG_STATE_BOOT;
                    }
                }
            }
            break;
        case ROUTINE_STATE_END:
            routine_scene = 0;
            game_list_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}