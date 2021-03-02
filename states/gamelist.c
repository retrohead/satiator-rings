#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../main.h"
#include "routine_states.h"
#include "../satiator_functions.h"

#define GAME_LIST_MAX_ITEMS 18
#define GAME_LIST_MAX_ITEM_LEN 25
#define TEXT_SCROLL_DELAY 15 // higher = slower, max 255

enum routine_state_types game_list_state = ROUTINE_STATE_INITIALIZE;

enum game_list_display_types
{
    GAME_LIST_STANDARD,
    GAME_LIST_FAVOURITES,
    GAME_LIST_RECENT_HISTORY,
};

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
void displayGameListItem(const char * name, int ypos, bool selected, enum dirEntryType type, bool triggersHeld)
{
    char nam[1024];
    strcpy(nam,name);
    if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
    {
        // trim the ID from the folder name
        char * pos = strrchr(nam, '[');
        if(pos != JO_NULL)
            nam[pos - nam - 1] = '\0';
        // trim the leading directory
        const char * lastSlash = strrchr(nam, '/');
        if(lastSlash)
        {
            const char * fileName = lastSlash + 1;
            strcpy(nam, fileName);
        }
    }
    if(selected)
    {
        jo_nbg2_printf(1, ypos, "> ");
        int len = strlen(nam);
        if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
            len++;
        if(!triggersHeld)
        {
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
    }
    else
        jo_nbg2_printf(1, ypos, "  ");
    if((nam[0] != '\0') && (nam[1] == '\0'))
        strcpy(nam, "/");
    nam[GAME_LIST_MAX_ITEM_LEN] = '\0'; // truncate to the max length
    if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
    {
        if(strlen(nam) >= GAME_LIST_MAX_ITEM_LEN - 1)
            nam[GAME_LIST_MAX_ITEM_LEN - 1] = '\0';
        strcat(nam, "/");
    }
    int len = strlen(nam);
    while(len < GAME_LIST_MAX_ITEM_LEN)
    {
        strcat(nam, " ");
        len++;
    }
    jo_nbg2_printf(3, ypos, nam);
}
void clearGameBoxSprite()
{
    if(gameBoxTex != -1)
    {
        jo_sprite_free_from(spriteTex[gameBoxTex].texture_id);
        spriteTex[gameBoxTex].used = false;
        sprites[gameBoxSprite].used = false;
        gameBoxTex = -1;
        gameBoxSprite = -1;
        strcpy(gameIdStr, "");
    }
}
void displayGameBox(char * idStr)
{
    int id;
    int boxID = 0;
    if(strcmp(idStr, ""))
    {
        clearGameBoxSprite();
        sscanf(idStr, "%d", &id);
        while(id >= 100)
        {
            id -= 100;
            boxID++;
        }
        sprintf(idStr, "%dS.TGA", id);
        char dir[50];
        if(boxID > 0)
            sprintf(dir, "BOX%d", boxID);
        else
            strcpy(dir, "BOX");
        gameBoxTex = load_sprite_texture(dir, idStr);
        if(gameBoxTex >= 0)
        {
            gameBoxSprite = create_sprite(gameBoxTex, 310 - getTextureWidth(gameBoxTex), 215 - getTextureHeight(gameBoxTex), 0, 1.0, 1.0, 0);
        }
    } else if(gameBoxTex != -1)
    {
        clearGameBoxSprite();
    }
}
void displaySelectedItemGameBox()
{
    if((dirEntries[selectedDirEntry].type == DIR_DIRECTORY) || (dirEntries[selectedDirEntry].type == DIR_SHORTCUT_FOLDER))
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
void displayGameList(bool triggersHeld)
{
    textScrollX = 0;
    textScrollDelay = TEXT_SCROLL_DELAY;
    textLeft = false;
    for(int i=listOffset;i < listOffset + GAME_LIST_MAX_ITEMS;i++)
    {
        if(i >= dirEntyCount)
        {
            displayGameListItem("", (i - listOffset) + 8, false, dirEntries[i].type, triggersHeld);
            continue;
        }
        displayGameListItem(dirEntries[i].name, (i - listOffset) + 8, i==selectedDirEntry, dirEntries[i].type, triggersHeld);
    }
    if(!triggersHeld)
        displaySelectedItemGameBox();
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
        if(!strcmp(st->name, "satiator-rings"))
            continue;
        // thanks Windows
        if (!strncmp(st->name, "System Volume Information", 25))
            continue;
        if(strlen(st->name) > 99)
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
bool writeSelectedItemFavourite()
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat("favs.ini", st, sizeof(statbuf));
    int size = -1;
    if(fp >= 0)
    {
        size = st->size;
        fp = s_open("favs.ini", FA_WRITE | FA_READ | FA_OPEN_ALWAYS);
    } else
    {
        fp = s_open("favs.ini", FA_WRITE | FA_CREATE_ALWAYS);
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
    s_write(fp, currentDirectory, strlen(currentDirectory));
    s_write(fp, "/", 1);
    s_write(fp, dirEntries[selectedDirEntry].name, strlen(dirEntries[selectedDirEntry].name));
    s_write(fp, "\r\n", 2);
    s_write(fp, "[END]", 5);
    s_close(fp);
    
    // change back to the current dir
    s_chdir(currentDirectory);
    jo_nbg2_printf(1, 27, "Item Added To Favourites                          ");
    return true;
}
bool selectedItemHasFavourite()
{
    if(strcmp("/", currentDirectory))
        s_chdir("/");
    s_chdir("satiator-rings");

    char fullpath[100];
    strcpy(fullpath, currentDirectory);
    strcat(fullpath, "/");
    strcat(fullpath, dirEntries[selectedDirEntry].name);

    // stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat("favs.ini", st, sizeof(statbuf));
    if(fp < 0)
    {
        // change back to the current dir
        s_chdir(currentDirectory);
        jo_nbg2_printf(1, 27, "Item Already Added To Favourites                     ");
        return false;
    }

    // open favs ini for reading
    fp = s_open("favs.ini", FA_READ | FA_OPEN_ALWAYS);
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
void loadIniList(char * fn, bool sort)
{
    strcpy(gameIdStr, "");
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
void logic_gamelist()
{
    static enum prog_state_types exit_state = PROG_STATE_SPLASH;
    static int depth = 0;
    static bool triggersHeld = false;
    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = GAME_LIST_STANDARD;
            gameBoxTex = -1;
            gameBoxSprite = -1;
            triggersHeld = false;
            strcpy(gameIdStr, "");
            loadFileList(".", satiatorExecutableFilter);
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);
            displayGameList(triggersHeld);
            game_list_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_SPLASH;
            break;
        case ROUTINE_STATE_RUN:
            jo_nbg2_printf(15, 4, "GAMELIST");
            if(dt.second % 2 == 0)
                jo_nbg2_printf(33, 4, "%02d %02d", dt.hour, dt.minute);
            else
                jo_nbg2_printf(33, 4, "%02d:%02d", dt.hour, dt.minute);
            
            switch(routine_scene)
            {
                case GAME_LIST_STANDARD:
                    jo_nbg2_printf(1, 6, "%s                         ", currentDirectory);
                    if(pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS)
                    {
                        jo_nbg2_printf(1, 6, "Favourites                                         ");
                        routine_scene = GAME_LIST_FAVOURITES;
                        loadIniList("favs.ini", true);
                        displayGameList(triggersHeld);
                    }
                    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
                    {
                        if(selectedItemHasFavourite())
                        {
                            // TODO: remove the shortcut
                        } else
                        {
                            writeSelectedItemFavourite();
                        }
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
                                    jo_nbg2_printf(1, 27, "Could not open %s             ", currentDirectory);
                                    currentDirectory[strstr("/" , currentDirectory) - currentDirectory] = '\0';
                                } else
                                {
                                    depth = depth + 1;
                                    loadFileList(".", satiatorExecutableFilter);
                                    displayGameList(triggersHeld);
                                    if((dirEntyCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
                                    {
                                        game_list_state = ROUTINE_STATE_END;
                                        exit_state = PROG_STATE_BOOT;
                                    }
                                }
                            } else if(dirEntries[selectedDirEntry].type == DIR_GAME)
                            {
                                game_list_state = ROUTINE_STATE_END;
                                exit_state = PROG_STATE_BOOT;
                            }
                        }
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
                            jo_nbg2_printf(1, 27, "Could not change dir to parent            ");
                        } else
                        {
                            depth--;
                            loadFileList(".", satiatorExecutableFilter);
                            displayGameList(triggersHeld);
                        }
                    }
                    break;
                case GAME_LIST_FAVOURITES:
                    if(pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS)
                    {
                        jo_nbg2_printf(1, 6, "Recent Play History                                ");
                        routine_scene = GAME_LIST_RECENT_HISTORY;
                        loadIniList("recent.ini", false);
                        displayGameList(triggersHeld);
                    }
                    
                    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
                    {
                        if(dirEntyCount > 0)
                        {
                            if(dirEntries[selectedDirEntry].type == DIR_SHORTCUT_FOLDER)
                            {
                                s_chdir("/");
                                int ret = s_chdir(dirEntries[selectedDirEntry].name);
                                if (ret != FR_OK) {
                                    jo_nbg2_printf(1, 27, "Could not open %s                     ", dirEntries[selectedDirEntry].name);
                                } else
                                {
                                    strcpy(currentDirectory, dirEntries[selectedDirEntry].name);
                                    depth = 0;
                                    for(uint32_t i=0;i<strlen(currentDirectory);i++)
                                        if(currentDirectory[i] == '/')
                                            depth++;
                                    loadFileList(".", satiatorExecutableFilter);
                                    displayGameList(triggersHeld);
                                    if((dirEntyCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
                                    {
                                        game_list_state = ROUTINE_STATE_END;
                                        exit_state = PROG_STATE_BOOT;
                                    }
                                    routine_scene = 0;
                                }
                            } else if(dirEntries[selectedDirEntry].type == DIR_SHORTCUT_GAME)
                            {
                                //
                            }
                        }
                    }
                    if (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
                    {
                        routine_scene = GAME_LIST_STANDARD;
                        s_chdir("/");
                        strcpy(currentDirectory, "/");
                        strcpy(gameIdStr, "");
                        loadFileList(".", satiatorExecutableFilter);
                        displayGameList(triggersHeld);
                    }
                    break;
                case GAME_LIST_RECENT_HISTORY:
                    if((pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS))
                    {
                        routine_scene = GAME_LIST_STANDARD;
                        s_chdir("/");
                        strcpy(currentDirectory, "/");
                        strcpy(gameIdStr, "");
                        loadFileList(".", satiatorExecutableFilter);
                        displayGameList(triggersHeld);
                    }
                    break;
            }

            // all list controls
            if(dirEntyCount > 0)
            {
                // scroll the selected item text every frame
                displayGameListItem(dirEntries[selectedDirEntry].name, (selectedDirEntry - listOffset) + 8, true, dirEntries[selectedDirEntry].type, triggersHeld);
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
                displayGameList(triggersHeld);
            }
            if(pad_controllers[0].btn_l == BUTTON_STATE_HELD)
            {
                if(selectedDirEntry > 0)
                {
                    selectedDirEntry--;
                    if(selectedDirEntry - listOffset  < 0)
                        listOffset--;
                }
                triggersHeld = true;
                displayGameList(triggersHeld);
                clearGameBoxSprite();
            }
            if(pad_controllers[0].btn_r == BUTTON_STATE_HELD)
            {
                if(selectedDirEntry < dirEntyCount - 1)
                {
                    selectedDirEntry ++;
                    if(selectedDirEntry - listOffset >= GAME_LIST_MAX_ITEMS)
                        listOffset++;
                }
                triggersHeld = true;
                displayGameList(triggersHeld);
                clearGameBoxSprite();
            }
            if((pad_controllers[0].btn_r == BUTTON_STATE_IDLE) && (pad_controllers[0].btn_l == BUTTON_STATE_IDLE) && (triggersHeld))
            {
                triggersHeld = false;
                displaySelectedItemGameBox();
            }
            break; 
        case ROUTINE_STATE_END:
            routine_scene = 0;
            game_list_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}