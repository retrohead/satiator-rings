#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../main.h"
#include "../ini.h"
#include "routine_states.h"
#include "../satiator_functions.h"

float BOX_BOUNCE_MAX_SPEED = 0.30f;
float BOX_BOUNCE_VELOCITY = 0.02f;

enum routine_state_types game_list_state = ROUTINE_STATE_INITIALIZE;

typedef struct
{
    char path[1024];
    int tex;
    int sprite;
} gameBoxType;


gameBoxType gameBox;
int defaultBoxTex = -1;
int selectedGameBoxSprite = 0;
int shadowSprite = 0;

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
    strcpy(gameBox.path, "");
    sprites[shadowSprite].x = 320;
    sprites[shadowSprite].y = 240;
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
    sprites[shadowSprite].scale_x -= sprites[gameBox.sprite].speed_y / 15;
    sprites[gameBox.sprite].y -= sprites[gameBox.sprite].speed_y;
}
void startBoxartBounce()
{
    if(gameBox.sprite == -1)
        return;
    sprites[gameBox.sprite].speed_y = BOX_BOUNCE_MAX_SPEED;
    sprites[gameBox.sprite].velocity_y = BOX_BOUNCE_VELOCITY;
}
void displayGameBox(char * path, bool bounce, float x, float y, float scale_x, float scale_y, bool singleView)
{
    clearGameBoxSprite();
    if(options[OPTIONS_LIST_MODE] == GAME_VIEW_TEXT_ONLY)
        return;
    if(!strcmp(path, ""))
        gameBox.tex = -1;
    else
        gameBox.tex = load_sprite_texture_satiator(path, "BOX.TGA");
    strcpy(gameBox.path, path);
    if(gameBox.tex >= 0)
    {
        if(singleView)
        {
            x -= getTextureWidth(gameBox.tex);
            y -= getTextureHeight(gameBox.tex);
        }
        gameBox.sprite = create_sprite(gameBox.tex, x, y, 0, scale_x, scale_y, 0);
        sprites[shadowSprite].x = x + 2;
        sprites[shadowSprite].y = y + getTextureHeight(gameBox.tex) + 5;
        sprites[shadowSprite].scale_x = 0.9;
        sprites[shadowSprite].scale_y = 0.9;
        if(getTextureWidth(gameBox.tex) != getTextureWidth(defaultBoxTex))
        {
            sprites[shadowSprite].scale_x = 0.7;
            sprites[shadowSprite].x -= 7;
        }
    } else
    {
        if(singleView)
        {
            x -= getTextureWidth(defaultBoxTex);
            y -= getTextureHeight(defaultBoxTex);
        }
        sprites[shadowSprite].x = x + 2;
        sprites[shadowSprite].y = y + getTextureHeight(defaultBoxTex) + 5;
        sprites[shadowSprite].scale_x = 0.9;
        sprites[shadowSprite].scale_y = 0.9;

        gameBox.sprite = create_sprite(defaultBoxTex, x, y, 0, scale_x, scale_y, 0);
    }
    if(bounce)
        startBoxartBounce();
}
void displayDirEntryItemGameBox(int entryId, bool bounce, float x, float y, float scale_x, float scale_y, bool singleView)
{
    if((dirEntries[entryId].type == DIR_DIRECTORY) || (dirEntries[entryId].type == DIR_SHORTCUT_FOLDER))
    {
        if(strcmp(dirEntries[entryId].name, gameBox.path))
            displayGameBox(dirEntries[entryId].name, bounce, x, y, scale_x, scale_y, singleView);
    } else
    {
        displayGameBox("", bounce, x, y, scale_x, scale_y, singleView);
    }
}
void updateBoxarts()
{
    displayDirEntryItemGameBox(selectedDirEntry, true, 310, 200, 1.0, 1.0, true);
}
void displayGameList(bool triggersHeld)
{
    clearMessage();
    resetTextScroll();

    setMaxListLength(GAME_LIST_MAX_ITEM_LEN);
    if(options[OPTIONS_LIST_MODE] == GAME_VIEW_TEXT_ONLY)
        setMaxListLength(38);

    for(int i=listOffset;i < listOffset + GAME_LIST_MAX_ITEMS;i++)
    {
        if(i >= dirEntryCount)
        {
            displayDirListItem("", (i - listOffset) + 5, false, dirEntries[i].type, triggersHeld);
            continue;
        }
        displayDirListItem(dirEntries[i].name, (i - listOffset) + 5, i==selectedDirEntry, dirEntries[i].type, triggersHeld);
    }
    updateSelectionSprite((selectedDirEntry- listOffset) + 5, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY));
    if(!triggersHeld)
        updateBoxarts();
}
void logic_gamelist_standard(enum game_list_display_types * display_type, enum prog_state_types * exit_state, bool triggersHeld, int * depth)
{
    jo_nbg2_printf(1, 3, "%s                                                  ", currentDirectory);
    if(pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_SELECT, false);
        jo_nbg2_clear();
        jo_nbg2_printf(1, 3, "Favourites                                         ");
        displayTime();
        *display_type = GAME_LIST_FAVOURITES;
        clearGameBoxSprite();
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 240;
        draw_sprites();
        slSynch();
        loadIniList("favs.ini", true, "", false);
        displayGameList(triggersHeld);
    }
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_OPTION, false);
        char * addFav = jo_malloc(1024);
        strcpy(addFav, "");
        if(strcmp(currentDirectory, "/"))
        {
            strcpy(addFav, currentDirectory);
            strcat(addFav, "/");
        }
        strcat(addFav, dirEntries[selectedDirEntry].name);
        jo_nbg2_clear();
        jo_nbg2_printf(1, 3, "Favourites                                         ");
        displayTime();
        *display_type = GAME_LIST_FAVOURITES;
        clearGameBoxSprite();
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 240;
        draw_sprites();
        slSynch();
        int result = addItemToIni("favs.ini", addFav, false, true, true);
        jo_free(addFav);
        displayGameList(triggersHeld);
        if(result)
            displayStatus("Item Added To Favourites");
        else
            displayStatus("Already Added To Favourites");
    }
    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
    {
        if(dirEntryCount > 0)
        {
            playSfx(SFX_SELECT, false);
            if(dirEntries[selectedDirEntry].type == DIR_DIRECTORY)
            {
                if(strlen(currentDirectory) > 1)
                    strcat(currentDirectory, "/");
                strcat(currentDirectory, dirEntries[selectedDirEntry].name);
                int ret = s_chdir(currentDirectory);
                if (ret != FR_OK) {
                    displayStatus("Could not open %s", currentDirectory);
                    currentDirectory[strstr("/" , currentDirectory) - currentDirectory] = '\0';
                } else
                {
                    *depth = *depth + 1;
                    jo_nbg2_printf(1, 3, "%s                                                  ", currentDirectory);
                    clearGameBoxSprite();
                    sprites[selectionSprite].x = 640;
                    sprites[selectionSprite].y = 240;
                    draw_sprites();
                    slSynch();
                    loadFileList(".", satiatorExecutableFilter);
                    displayGameList(triggersHeld);
                    if((dirEntryCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
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
        if(*depth == 0)
        {
            playSfx(SFX_BACK, false);
            return;
        }
        playSfx(SFX_SELECT, false);
        char * pos = strrchr(currentDirectory, '/');
        if(pos != JO_NULL)
            currentDirectory[pos - currentDirectory] = '\0';
        if(currentDirectory[0] == '\0')
            strcpy(currentDirectory, "/");
        int ret = s_chdir(currentDirectory);
        if (ret != FR_OK) {
            displayStatus("Could not change dir to parent");
        } else
        {
            *depth = *depth - 1;
            if(*depth < 0)
                *depth = 0;
            clearGameBoxSprite();
            sprites[selectionSprite].x = 640;
            sprites[selectionSprite].y = 240;
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
        displayStatus("Could not open %s", dirEntries[selectedDirEntry].name);
    } else
    {
        strcpy(currentDirectory, dirEntries[selectedDirEntry].name);
        *depth = 0;
        for(uint32_t i=0;i<strlen(currentDirectory);i++)
            if(currentDirectory[i] == '/')
                *depth = *depth + 1;
        loadFileList(".", satiatorExecutableFilter);
        displayGameList(false);
        if((dirEntryCount == 1) && (dirEntries[selectedDirEntry].type == DIR_GAME))
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
    jo_nbg2_printf(1, 3, "Favourites                                         ");
    if(pad_controllers[0].btn_x == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_SELECT, false);
        jo_nbg2_clear();
        displayTime();
        jo_nbg2_printf(1, 3, "Recent Play History                                ");
        *display_type = GAME_LIST_RECENT_HISTORY;
        clearGameBoxSprite();
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 240;
        draw_sprites();
        slSynch();
        loadIniList("recent.ini", false, "", false);
        displayGameList(triggersHeld);
    }
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_BACK, false);
        writeIniList("favs.ini", dirEntries[selectedDirEntry].name);
        dirEntries[selectedDirEntry].type = DIR_NULL;
        for(int i=selectedDirEntry; i < dirEntryCount;i++)
            dirEntries[i] = dirEntries[i + 1];
        if(selectedDirEntry > 0)
            selectedDirEntry--;
        dirEntryCount--;
        displayGameList(triggersHeld);
        displayStatus("Item deleted from favourites");
    }
    
    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
    {
        if(dirEntryCount > 0)
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
        s_chdir(currentDirectory);
        jo_nbg2_clear();
        displayTime();
        jo_nbg2_printf(1, 3, "%s                                                  ", currentDirectory);
        clearGameBoxSprite();
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 240;
        draw_sprites();
        slSynch();
        loadFileList(".", satiatorExecutableFilter);
        displayGameList(triggersHeld);
    }
}
void logic_gamelist_recents(enum game_list_display_types * display_type, enum prog_state_types * exit_state, bool triggersHeld, int * depth)
{
    jo_nbg2_printf(1, 3, "Recent Play History                                ");
    if(pad_controllers[0].btn_y == BUTTON_STATE_NEWPRESS)
    {
        playSfx(SFX_BACK, false);
        writeIniList("recent.ini", dirEntries[selectedDirEntry].name);
        dirEntries[selectedDirEntry].type = DIR_NULL;
        for(int i=selectedDirEntry; i < dirEntryCount;i++)
            dirEntries[i] = dirEntries[i + 1];
        if(selectedDirEntry > 0)
            selectedDirEntry--;
        dirEntryCount--;
        displayGameList(triggersHeld);
        displayStatus("Item deleted from recents");
    }
    if((pad_controllers[0].btn_a == BUTTON_STATE_NEWPRESS) || (pad_controllers[0].btn_c == BUTTON_STATE_NEWPRESS))
    {
        if(dirEntryCount > 0)
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
        jo_nbg2_printf(1, 3, "%s                                                  ", currentDirectory);
        clearGameBoxSprite();
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 240;
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
    static bool firstrun = true;
    int maxlistItems = GAME_LIST_MAX_ITEMS;
    switch(game_list_state)
    {
        case ROUTINE_STATE_INITIALIZE:
            applyTheme();
            gameBox.tex = -1;
            createGuiBoxes("GAME.TGA", true);
            defaultBoxTex = load_sprite_texture_satiator("/satiator-rings/gfx", "SBOX.TGA");
            strcpy(gameBox.path, "/satiator-rings/themes/");
            strcat(gameBox.path, loadedThemeName);
            shadowSprite = create_sprite(load_sprite_texture_satiator(gameBox.path, "SHDW.TGA"), 320, 240, 0, 1, 1, 0);
            strcpy(gameBox.path, "");
            routine_scene = 0;
            gameBox.sprite = -1;
            if(firstrun)
                display_type = options[OPTIONS_LIST_CATEGORY];
            firstrun = false;
            triggersHeld = false;
            switch(display_type)
            {
                case GAME_LIST_STANDARD:
                    loadFileList(".", satiatorExecutableFilter);
                    break;
                case GAME_LIST_FAVOURITES:
                    loadIniList("favs.ini", true, "", false);
                    break;
                case GAME_LIST_RECENT_HISTORY:
                    loadIniList("recent.ini", false, "", false);
                    break;
                case GAME_LIST_DISPLAY_MAX_COUNT:
                    break;
            }
            setMaxListLength(GAME_LIST_MAX_ITEM_LEN);
            if(options[OPTIONS_LIST_MODE] == GAME_VIEW_TEXT_ONLY)
                setMaxListLength(38);
            displayGameList(triggersHeld);
            displayStatus("Press start for options");
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
                case GAME_LIST_DISPLAY_MAX_COUNT:
                    break;
            }

            // all list controls
            if(dirEntryCount > 0)
            {
                // scroll the selected item text every frame
                displayDirListItem(dirEntries[selectedDirEntry].name, (selectedDirEntry - listOffset) + 5, true, dirEntries[selectedDirEntry].type, triggersHeld);
            }
            if(pad_controllers[0].btn_start == BUTTON_STATE_NEWPRESS)
            {
                game_list_state = ROUTINE_STATE_END;
                exit_state = PROG_STATE_MENU;
                playSfx(SFX_CHANGE, false);
            }
            if(pad_controllers[0].btn_z == BUTTON_STATE_NEWPRESS)
            {
                // changing display mode
                if(options[OPTIONS_LIST_MODE] == GAME_VIEW_MAX_COUNT - 1)
                    options[OPTIONS_LIST_MODE] = GAME_VIEW_TEXT_AND_IMAGE;
                else
                    options[OPTIONS_LIST_MODE]++;
                strcpy(gameBox.path, "");
                displayGameList(triggersHeld);
                playSfx(SFX_SLIDE, false);
            }
            
            if(pad_controllers[0].direction_status == BUTTON_STATE_HELD)
            {
                switch(pad_controllers[0].direction_id)
                {    
                    case UP:
                        if((listScrolldelay < LIST_SCROLL_DELAY) && !triggersHeld)
                        {
                            listScrolldelay++;
                            break;
                        }
                        listScrolldelay = 0;
                        moveDirEntrySelectionUp(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), true);
                        displayGameList(triggersHeld);
                        break;
                    case DOWN:
                        if((listScrolldelay < LIST_SCROLL_DELAY) && !triggersHeld)
                        {
                            listScrolldelay++;
                            break;
                        }
                        listScrolldelay = 0;
                        moveDirEntrySelectionDown(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), true);
                        displayGameList(triggersHeld);
                        break;
                }
            }
            if(pad_controllers[0].direction_status == BUTTON_STATE_NEWPRESS)
            {
                switch(pad_controllers[0].direction_id)
                {
                    case LEFT:
                    case DOWN_LEFT:
                    case UP_LEFT:
                        listScrolldelay = 0;
                        if(dirEntryCount > GAME_LIST_MAX_ITEMS)
                        {
                            for(int i=0;i< GAME_LIST_MAX_ITEMS - 1;i++)
                            {
                                moveDirEntrySelectionUp(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), false);
                                if(selectedDirEntry == 1)
                                    break;
                            }
                        }
                        moveDirEntrySelectionUp(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), true);
                        break;
                    case RIGHT:
                    case DOWN_RIGHT:
                    case UP_RIGHT:
                        listScrolldelay = 0;
                        if(dirEntryCount > GAME_LIST_MAX_ITEMS)
                        {
                            for(int i=0;i< GAME_LIST_MAX_ITEMS - 1;i++)
                            {
                                moveDirEntrySelectionDown(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), false);
                                if(selectedDirEntry == dirEntryCount - 2)
                                    break;
                            }
                        }
                        moveDirEntrySelectionDown(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), true);
                         break;
                    case UP:
                        listScrolldelay = 0;
                        moveDirEntrySelectionUp(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), true);
                        break;
                    case DOWN:
                        listScrolldelay = 0;
                        moveDirEntrySelectionDown(maxlistItems, SFX_MOVE, (options[OPTIONS_LIST_MODE] != GAME_VIEW_TEXT_ONLY), true);
                        break;
                }
                displayGameList(triggersHeld);
            }
            if(pad_controllers[0].btn_l == BUTTON_STATE_HELD)
            {
                playSfx(SFX_MOVE, true);
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
                triggersHeld = true;
                displayGameList(triggersHeld);
                clearGameBoxSprite();
            }
            if(pad_controllers[0].btn_r == BUTTON_STATE_HELD)
            {
                playSfx(SFX_MOVE, true);
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