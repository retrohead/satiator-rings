#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../main.h"
#include "../ini.h"
#include "routine_states.h"
#include "../satiator_functions.h"

#define GAME_LIST_MAX_ITEMS 18
#define GAME_IMAGES_MAX_ITEMS 5
#define GAME_IMAGES_SCALE 0.75f
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

typedef struct
{
    int id;
    int tex;
    int sprite;
} gameBoxType;



bool textLeft = false;

gameBoxType gameBox;
int defaultBoxTex = -1;
uint8_t textScrollDelay = 0;
int selectedGameBoxSprite = 0;
int textScrollX = 0;

void displayGameListItemText(char * nam, int ypos, bool selected, enum dirEntryType type, bool triggersHeld)
{
    int maxLen = GAME_LIST_MAX_ITEM_LEN;
    if(options[OPTIONS_LIST_MODE] == GAME_VIEW_TEXT_ONLY)
        maxLen = 35;
    if(selected)
    {
        jo_nbg2_printf(1, ypos, "> ");
        int len = strlen(nam);
        if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
            len++;
        if(!triggersHeld)
        {
            if(len >= maxLen)
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
                        if(maxLen + textScrollX >= len)
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
    nam[maxLen] = '\0'; // truncate to the max length
    if((type == DIR_DIRECTORY) || (type == DIR_SHORTCUT_FOLDER))
    {
        if((int)strlen(nam) >= maxLen - 1)
            nam[maxLen - 1] = '\0';
        strcat(nam, "/");
    }
    int len = strlen(nam);
    while(len < 39) // clear the rest of the list
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
    displayGameListItemText(nam, ypos, selected, type, triggersHeld);
}
void clearGameBoxSprite()
{
    if(gameBox.tex != -1)
    {
        jo_sprite_free_from(spriteTex[gameBox.tex].texture_id);
        spriteTex[gameBox.tex].used = false;
        gameBox.tex = -1;
    }
    if(gameBox.sprite != -1)
        sprites[gameBox.sprite].used = false;
    gameBox.sprite = -1;
    gameBox.id = -1;
}
void animateBoxart()
{
    if(gameBox.sprite == -1)
        return;
    if(sprites[gameBox.sprite].velocity_y == 0.0f)
        return;

    // adjust speed according to velocity
    sprites[gameBox.sprite].speed_y -= sprites[gameBox.sprite].velocity_y;
    if(sprites[gameBox.sprite].speed_y <= -BOX_BOUNCE_MAX_SPEED)
    {
        // top of the bounce
        sprites[gameBox.sprite].speed_y = -BOX_BOUNCE_MAX_SPEED;
        sprites[gameBox.sprite].velocity_y = -sprites[gameBox.sprite].velocity_y;
    }
    else if(sprites[gameBox.sprite].speed_y > BOX_BOUNCE_MAX_SPEED)
    {
        // bottom of the bounce
        sprites[gameBox.sprite].speed_y = BOX_BOUNCE_MAX_SPEED;
        sprites[gameBox.sprite].velocity_y = -sprites[gameBox.sprite].velocity_y;
    }
    sprites[gameBox.sprite].y -= sprites[gameBox.sprite].speed_y;
}
void startBoxartBounce()
{
    if(gameBox.sprite == -1)
        return;
    sprites[gameBox.sprite].speed_y = BOX_BOUNCE_MAX_SPEED;
    sprites[gameBox.sprite].velocity_y = BOX_BOUNCE_VELOCITY;
}
void displayGameBox(int id, bool bounce, float x, float y, float scale_x, float scale_y, bool singleView)
{
    clearGameBoxSprite();
    if(options[OPTIONS_LIST_MODE] == GAME_VIEW_TEXT_ONLY)
        return;
    gameBox.id = id;

    if(gameBox.id >= 0)
    {
        char fn[20];
        int boxFolderID = 0;
        while(id >= 100)
        {
            id -= 100;
            boxFolderID++;
        }
        sprintf(fn, "%dS.TGA", id);
        char dir[50];
        if(boxFolderID > 0)
            sprintf(dir, "BOX%d", boxFolderID);
        else
            strcpy(dir, "BOX");
        gameBox.tex = load_sprite_texture(dir, fn);
    }

    if(gameBox.tex >= 0)
    {
        if(singleView)
        {
            x -= getTextureWidth(gameBox.tex);
            y -= getTextureHeight(gameBox.tex);
        }
        gameBox.sprite = create_sprite(gameBox.tex, x, y, 0, scale_x, scale_y, 0);
    } else
    {
        if(singleView)
        {
            x -= getTextureWidth(defaultBoxTex);
            y -= getTextureHeight(defaultBoxTex);
        }
        gameBox.sprite = create_sprite(defaultBoxTex, x, y, 0, scale_x, scale_y, 0);
    }
    if(bounce)
        startBoxartBounce();
}
void displayDirEntryItemGameBox(int entryId, bool bounce, float x, float y, float scale_x, float scale_y, bool singleView)
{
    if((dirEntries[entryId].type == DIR_DIRECTORY) || (dirEntries[entryId].type == DIR_SHORTCUT_FOLDER))
    {
        const char *brckt = strrchr(dirEntries[entryId].name, '[');
        if (brckt) {
            const char *gameidpos = brckt + 1;
            char gameId[10];
            strcpy(gameId, gameidpos);
            gameId[strlen(gameId) - 1] = '\0'; // strip ending bracket
            int id;
            sscanf(gameId, "%d", &id);
            if(id != gameBox.id)
                displayGameBox(id, bounce, x, y, scale_x, scale_y, singleView);
        } else
        {
            displayGameBox(-1, bounce, x, y, scale_x, scale_y, singleView);
        }
    } else
    {
        displayGameBox(-1, bounce, x, y, scale_x, scale_y, singleView);
    }
}
void updateBoxarts()
{
    displayDirEntryItemGameBox(selectedDirEntry, true, 310, 200, 1.0, 1.0, true);
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
        updateBoxarts();
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
        playSfx(SFX_SELECT, false);
        jo_nbg2_clear();
        jo_nbg2_printf(1, 6, "Favourites                                         ");
        displayTime();
        *display_type = GAME_LIST_FAVOURITES;
        clearGameBoxSprite();
        draw_sprites();
        slSynch();
        loadIniList("favs.ini", true);
        displayGameList(triggersHeld);
    }
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_MOVE, false);
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
            playSfx(SFX_SELECT, false);
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
        playSfx(SFX_SELECT, false);
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
        playSfx(SFX_SELECT, false);
        jo_nbg2_clear();
        displayTime();
        jo_nbg2_printf(1, 6, "Recent Play History                                ");
        *display_type = GAME_LIST_RECENT_HISTORY;
        clearGameBoxSprite();
        draw_sprites();
        slSynch();
        loadIniList("recent.ini", false);
        displayGameList(triggersHeld);
    }
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_MOVE, false);
        if(lineIsInIni("favs.ini", dirEntries[selectedDirEntry].name))
        {
            if(deleteIniLine("favs.ini", dirEntries[selectedDirEntry].name))
            {
                jo_nbg2_printf(1, 27, "Deleted from favourites                        ");
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
            playSfx(SFX_SELECT, false);
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
        playSfx(SFX_SELECT, false);
        *display_type = GAME_LIST_STANDARD;
        s_chdir("/");
        strcpy(currentDirectory, "/");
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
        playSfx(SFX_MOVE, false);
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
            playSfx(SFX_SELECT, false);
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
        playSfx(SFX_SELECT, false);
        *display_type = GAME_LIST_STANDARD;
        s_chdir("/");
        strcpy(currentDirectory, "/");
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
    int maxlistItems = GAME_LIST_MAX_ITEMS;
    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            routine_scene = 0;
            gameBox.tex = -1;
            gameBox.sprite = -1;
            gameBox.id = -1;
            triggersHeld = false;
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
            defaultBoxTex = load_sprite_texture("TEX", "SBOX.TGA");
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
                playSfx(SFX_SELECT, false);
            }
            if(pad_controllers[0].btn_z == BUTTON_STATE_NEWPRESS)
            {
                // changing display mode
                if(options[OPTIONS_LIST_MODE] == GAME_VIEW_MAX_COUNT - 1)
                    options[OPTIONS_LIST_MODE] = GAME_VIEW_TEXT_AND_IMAGE;
                else
                    options[OPTIONS_LIST_MODE]++;
                gameBox.id = -1;
                displayGameList(triggersHeld);
                playSfx(SFX_SELECT, false);
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
                                playSfx(SFX_MOVE, false);
                                draw_sprites();
                                slSynch();
                                
                            }
                        break;
                    case DOWN:
                            if(selectedDirEntry < dirEntyCount - 1)
                            {
                                selectedDirEntry ++;
                                if(selectedDirEntry - listOffset >= maxlistItems)
                                    listOffset++;
                                playSfx(SFX_MOVE, false);
                                draw_sprites();
                                slSynch();
                            }
                        break;
                }
                displayGameList(triggersHeld);
            }
            if(pad_controllers[0].btn_l == BUTTON_STATE_HELD)
            {
                if(selectedDirEntry > 0)
                {
                    playSfx(SFX_MOVE, true);
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
                    playSfx(SFX_MOVE, true);
                    selectedDirEntry ++;
                    if(selectedDirEntry - listOffset >= maxlistItems)
                        listOffset++;
                }
                triggersHeld = true;
                displayGameList(triggersHeld);
                clearGameBoxSprite();
            }
            if((pad_controllers[0].btn_r == BUTTON_STATE_IDLE) && (pad_controllers[0].btn_l == BUTTON_STATE_IDLE) && (triggersHeld))
            {
                triggersHeld = false;
                updateBoxarts();
            }
            break; 
        case ROUTINE_STATE_END:
            routine_scene = 0;
            game_list_state = ROUTINE_STATE_INITIALIZE;
            prog_state = exit_state;
            break;
    }
}