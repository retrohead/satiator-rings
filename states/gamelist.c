#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../main.h"
#include "../ini.h"
#include "routine_states.h"
#include "../satiator_functions.h"

#define GAME_LIST_MAX_ITEMS 18
#define GAME_LIST_MAX_ITEM_LEN 25
#define TEXT_SCROLL_DELAY 15 // higher = slower, max 255
#define MAX_FAVOURITES 100
float BOX_BOUNCE_MAX_SPEED = 0.30f;
float BOX_BOUNCE_VELOCITY = 0.02f;

enum routine_state_types game_list_state = ROUTINE_STATE_INITIALIZE;

enum game_list_display_types
{
    GAME_LIST_STANDARD,
    GAME_LIST_FAVOURITES,
    GAME_LIST_RECENT_HISTORY,
};

bool textLeft = false;

char gameIdStr[50];
int gameBoxTex;
int gameBoxSprite;
uint8_t textScrollDelay = 0;

int textScrollX = 0;

void displayGameListItem(const char * name, int ypos, bool selected, enum dirEntryType type, bool triggersHeld)
{
    char nam[1024];
    strcpy(nam, name);
    if(type == DIR_NULL)
        strcpy(nam,"");
    if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER) || (type == DIR_SHORTCUT_GAME))
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
    {
        jo_nbg2_printf(1, ypos, "  ");
    }
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
    if(selected)
    {
        //jo_nbg2_printf(3, ypos, "                                                     ");
        jo_nbg2_printf(3, ypos, nam);
    } else
    {
        jo_nbg2_printf(3, ypos, nam);
        //jo_nbg3_printf(3, ypos, "                                                     ");
    }
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
void animateBoxart()
{
    if((gameBoxSprite == -1) || (gameBoxTex == -1))
        return;
    if(sprites[gameBoxSprite].velocity_y == 0.0f)
        return;

    // adjust speed according to velocity
    sprites[gameBoxSprite].speed_y -= sprites[gameBoxSprite].velocity_y;
    if(sprites[gameBoxSprite].speed_y <= -BOX_BOUNCE_MAX_SPEED)
    {
        // top of the bounce
        sprites[gameBoxSprite].speed_y = -BOX_BOUNCE_MAX_SPEED;
        sprites[gameBoxSprite].velocity_y = -sprites[gameBoxSprite].velocity_y;
    }
    else if(sprites[gameBoxSprite].speed_y > BOX_BOUNCE_MAX_SPEED)
    {
        // bottom of the bounce
        sprites[gameBoxSprite].speed_y = BOX_BOUNCE_MAX_SPEED;
        sprites[gameBoxSprite].velocity_y = -sprites[gameBoxSprite].velocity_y;
    }
    sprites[gameBoxSprite].y -= sprites[gameBoxSprite].speed_y;
}
void startBoxartBounce()
{
    if((gameBoxSprite == -1) || (gameBoxTex == -1))
        return;
    sprites[gameBoxSprite].speed_y = BOX_BOUNCE_MAX_SPEED;
    sprites[gameBoxSprite].velocity_y = BOX_BOUNCE_VELOCITY;
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
            gameBoxSprite = create_sprite(gameBoxTex, 310 - getTextureWidth(gameBoxTex), 200 - getTextureHeight(gameBoxTex), 0, 1.0, 1.0, 0);
            startBoxartBounce();
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
void clearMessage()
{
    jo_nbg2_printf(0, 27, "                                               ");
}
void displayGameList(bool triggersHeld)
{
    clearMessage();
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
void displayTime()
{
    jo_nbg2_printf(15, 4, "GAMELIST");
    if(dt.second % 2 == 0)
        jo_nbg2_printf(33, 4, "%02d %02d", dt.hour, dt.minute);
    else
        jo_nbg2_printf(33, 4, "%02d:%02d", dt.hour, dt.minute);
}
void logic_gamelist_standard(enum game_list_display_types * display_type, enum prog_state_types * exit_state, bool triggersHeld, int * depth)
{
    jo_nbg2_printf(1, 6, "%s                                                  ", currentDirectory);
    if(pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS)
    {
        jo_nbg2_clear();
        jo_nbg2_printf(1, 6, "Favourites                                         ");
        displayTime();
        *display_type = GAME_LIST_FAVOURITES;
        clearGameBoxSprite();
        draw_sprites();
        slSynch();
        loadIniList("favs.ini", true);
        strcpy(gameIdStr, "");
        displayGameList(triggersHeld);
    }
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        char fullpath[256];
        strcpy(fullpath, "");
        if(strcmp(currentDirectory, "/"))
        {
            strcpy(fullpath, currentDirectory);
            strcat(fullpath, "/");
        }
        strcat(fullpath, dirEntries[selectedDirEntry].name);
        if(!lineIsInIni("favs.ini", fullpath))
        {
            if(writeUniqueIniLineAtEnd("favs.ini", fullpath, MAX_FAVOURITES))
                jo_nbg2_printf(1, 27, "Item Added To Favourites                             ");
        }
        else
            jo_nbg2_printf(1, 27, "Already Added To Favourites                     ");
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
                    *depth = *depth + 1;
                    jo_nbg2_printf(1, 6, "%s                                                  ", currentDirectory);
                    clearGameBoxSprite();
                    draw_sprites();
                    slSynch();
                    loadFileList(".", satiatorExecutableFilter);
                    displayGameList(triggersHeld);
                    if((dirEntyCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
                    {
                        game_list_state = ROUTINE_STATE_END;
                        *exit_state = PROG_STATE_BOOT;
                    }
                }
            } else if(dirEntries[selectedDirEntry].type == DIR_GAME)
            {
                game_list_state = ROUTINE_STATE_END;
                *exit_state = PROG_STATE_BOOT;
            }
        }
    }
    if(pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
    {
        if(depth == 0)
        {
            game_list_state = ROUTINE_STATE_END;
            *exit_state = PROG_STATE_SPLASH;
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
            clearGameBoxSprite();
            draw_sprites();
            slSynch();
            loadFileList(".", satiatorExecutableFilter);
            displayGameList(triggersHeld);
        }
    }
}
void launchDirShortcut(enum game_list_display_types * display_type, enum prog_state_types * exit_state, int * depth)
{
    s_chdir("/");
    int ret = s_chdir(dirEntries[selectedDirEntry].name);
    if (ret != FR_OK) {
        jo_nbg2_printf(1, 27, "Could not open %s                     ", dirEntries[selectedDirEntry].name);
    } else
    {
        strcpy(currentDirectory, dirEntries[selectedDirEntry].name);
        *depth = 0;
        for(uint32_t i=0;i<strlen(currentDirectory);i++)
            if(currentDirectory[i] == '/')
                *depth = *depth + 1;
        loadFileList(".", satiatorExecutableFilter);
        displayGameList(false);
        if((dirEntyCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
        {
            game_list_state = ROUTINE_STATE_END;
            *exit_state = PROG_STATE_BOOT;
        }
        *display_type = GAME_LIST_STANDARD;
    }
}
void launchGameShortcut(enum game_list_display_types * display_type, enum prog_state_types * exit_state, int * depth)
{
    s_chdir("/");
    char * lastSlash = strrchr(dirEntries[selectedDirEntry].name, '/');
    *depth = 0;
    if(!lastSlash)
    {
        strcpy(currentDirectory, dirEntries[selectedDirEntry].name);
    } else
    {
        // need to change directory
        char * dir = jo_malloc(256);
        const char * fileName = lastSlash + 1;
        strcpy(dir, dirEntries[selectedDirEntry].name);
        dir[lastSlash - dir] = '\0';
        s_chdir(dir);
        strcpy(currentDirectory, dir);
        jo_free(dir);
        *depth = 0;
        for(uint32_t i=0;i<strlen(currentDirectory);i++)
            if(currentDirectory[i] == '/')
                *depth = *depth + 1;
        strcpy(dirEntries[selectedDirEntry].name, fileName);
    }
    game_list_state = ROUTINE_STATE_END;
    *exit_state = PROG_STATE_BOOT;
    *display_type = GAME_LIST_STANDARD;
}
void logic_gamelist_favourites(enum game_list_display_types * display_type, enum prog_state_types * exit_state, bool triggersHeld, int * depth)
{
    jo_nbg2_printf(1, 6, "Favourites                                         ");
    if(pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS)
    {
        jo_nbg2_clear();
        displayTime();
        jo_nbg2_printf(1, 6, "Recent Play History                                ");
        *display_type = GAME_LIST_RECENT_HISTORY;
        strcpy(gameIdStr, "");
        clearGameBoxSprite();
        draw_sprites();
        slSynch();
        loadIniList("recent.ini", false);
        displayGameList(triggersHeld);
    }
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        if(lineIsInIni("favs.ini", dirEntries[selectedDirEntry].name))
        {
            if(deleteIniLine("favs.ini", dirEntries[selectedDirEntry].name))
            {
                jo_nbg2_printf(1, 27, "Deleted from favourites                        ");
                strcpy(gameIdStr, "");
                dirEntries[selectedDirEntry].type = DIR_NULL;
                clearGameBoxSprite();
                draw_sprites();
                slSynch();
                loadIniList("favs.ini", true);
                displayGameList(triggersHeld);
            }
            else
                jo_nbg2_printf(1, 27, "Failed to delete                             ");
        }
    }
    
    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
    {
        if(dirEntyCount > 0)
        {
            if(dirEntries[selectedDirEntry].type == DIR_SHORTCUT_FOLDER)
            {
                launchDirShortcut(display_type, exit_state, depth);
            } else if(dirEntries[selectedDirEntry].type == DIR_SHORTCUT_GAME)
            {
                launchGameShortcut(display_type, exit_state, depth);
            }
        }
    }
    if (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS)
    {
        *display_type = GAME_LIST_STANDARD;
        s_chdir("/");
        strcpy(currentDirectory, "/");
        strcpy(gameIdStr, "");
        jo_nbg2_clear();
        displayTime();
        jo_nbg2_printf(1, 6, "%s                                                  ", currentDirectory);
        clearGameBoxSprite();
        draw_sprites();
        slSynch();
        loadFileList(".", satiatorExecutableFilter);
        displayGameList(triggersHeld);
    }
}
void logic_gamelist_recents(enum game_list_display_types * display_type, enum prog_state_types * exit_state, bool triggersHeld, int * depth)
{
    jo_nbg2_printf(1, 6, "Recent Play History                                ");
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        if(lineIsInIni("recent.ini", dirEntries[selectedDirEntry].name))
        {
            if(deleteIniLine("recent.ini", dirEntries[selectedDirEntry].name))
            {
                jo_nbg2_printf(1, 27, "Deleted from recents                        ");
                clearGameBoxSprite();
                draw_sprites();
                slSynch();
                loadIniList("recent.ini", false);
                displayGameList(triggersHeld);
            }
            else
                jo_nbg2_printf(1, 27, "Failed to delete                             ");
        }
    }
    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
    {
        if(dirEntyCount > 0)
        {
            if(dirEntries[selectedDirEntry].type == DIR_SHORTCUT_FOLDER)
            {
                launchDirShortcut(display_type, exit_state, depth);
            } else if(dirEntries[selectedDirEntry].type == DIR_SHORTCUT_GAME)
            {
                launchGameShortcut(display_type, exit_state, depth);
            }
        }
    }
    if((pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_b == BUTTON_STATE_NEWPRESS))
    {
        *display_type = GAME_LIST_STANDARD;
        s_chdir("/");
        strcpy(currentDirectory, "/");
        strcpy(gameIdStr, "");
        jo_nbg2_clear();
        displayTime();
        jo_nbg2_printf(1, 6, "%s                                                  ", currentDirectory);
        clearGameBoxSprite();
        draw_sprites();
        slSynch();
        loadFileList(".", satiatorExecutableFilter);
        displayGameList(triggersHeld);
    }
}
void logic_gamelist()
{
    static enum prog_state_types exit_state = PROG_STATE_SPLASH;
    static int depth = 0;
    static bool triggersHeld = false;
    static enum game_list_display_types display_type = GAME_LIST_STANDARD;

    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            gameBoxTex = -1;
            gameBoxSprite = -1;
            triggersHeld = false;
            strcpy(gameIdStr, "");
            switch(display_type)
            {
                case GAME_LIST_STANDARD:
                    loadFileList(".", satiatorExecutableFilter);
                    break;
                case GAME_LIST_FAVOURITES:
                    loadIniList("favs.ini", true);
                    break;
                case GAME_LIST_RECENT_HISTORY:
                    loadIniList("recent.ini", false);
                    break;
            }
            create_sprite(load_sprite_texture("TEX", "LOGO.TGA"), 5, 5, 1, 1.0, 1.0, 0);
            displayGameList(triggersHeld);
            game_list_state = ROUTINE_STATE_RUN;
            exit_state = PROG_STATE_SPLASH;
            break;
        case ROUTINE_STATE_RUN:
            displayTime();
            animateBoxart();
            switch(display_type)
            {
                case GAME_LIST_STANDARD:
                    logic_gamelist_standard(&display_type, &exit_state, triggersHeld, &depth);
                    break;
                case GAME_LIST_FAVOURITES:
                    logic_gamelist_favourites(&display_type, &exit_state, triggersHeld, &depth);
                    break;
                case GAME_LIST_RECENT_HISTORY:
                    logic_gamelist_recents(&display_type, &exit_state, triggersHeld, &depth);
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