#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "satiator_functions.h"
#include "ini.h"

jo_palette main_palette;

themeType loadedTheme;
bool themeLoaded = false;

void applyThemeColour(int i)
{
    main_palette.data[2 + i] = JO_COLOR_RGB(loadedTheme.colours[i].r, loadedTheme.colours[i].g, loadedTheme.colours[i].b);
}
void applyTheme()
{
    for(int i=0;i<PAL_COL_COUNT;i++)
        applyThemeColour(i);
}

// change theme colours using real RGB values, max 255
void changeThemeCol(enum themePalEntryTypes type, int r, int g, int b)
{
    loadedTheme.colours[type].r = r;
    loadedTheme.colours[type].g = g;
    loadedTheme.colours[type].b = b;
    applyThemeColour(type);
}

void defaultThemeColour(enum themePalEntryTypes type)
{
    switch(type)
    {
        case PAL_COL_FONT:
            loadedTheme.colours[type].r = 255;
            loadedTheme.colours[type].g = 0;
            loadedTheme.colours[type].b = 0;
            break;
        case PAL_COL_BG:
            loadedTheme.colours[type].r = 255;
            loadedTheme.colours[type].g = 0;
            loadedTheme.colours[type].b = 0;
            break;
        case PAL_COL_SELECTOR:
            loadedTheme.colours[type].r = 251;
            loadedTheme.colours[type].g = 221;
            loadedTheme.colours[type].b = 221;
            break;
        case PAL_COL_WHITE:
            loadedTheme.colours[type].r = 255;
            loadedTheme.colours[type].g = 255;
            loadedTheme.colours[type].b = 255;
            break;
        case PAL_COL_BOX_BG:
            loadedTheme.colours[type].r = 255;
            loadedTheme.colours[type].g = 255;
            loadedTheme.colours[type].b = 255;
            break;
        case PAL_COL_COUNT: // remove warning in compiler
            break;
    }
}
void initTheme()
{
    for(int i=0;i<PAL_COL_COUNT;i++)
        defaultThemeColour(i);
}

jo_palette          *theme_palette_handling(void)
{
    jo_create_palette(&main_palette);
    main_palette.data[2 + PAL_COL_FONT] = JO_COLOR_Red;
    main_palette.data[2 + PAL_COL_BG] = JO_COLOR_Red;
    main_palette.data[2 + PAL_COL_BOX_BG] = JO_COLOR_White;
    main_palette.data[2 + PAL_COL_WHITE] = JO_COLOR_White;
    main_palette.data[2 + PAL_COL_SELECTOR] = JO_COLOR_RGB(251, 221, 221);
    return (&main_palette);
}

bool loadThemeFile(char * theme)
{
    s_chdir("/satiator-rings/themes");
    s_chdir(theme);
    char *fn = "theme.ini";
    s_stat_t *st = (s_stat_t*)statbuf;
    int fp = s_stat(fn, st, sizeof(statbuf));
    if (fp >=0)
    {
        fp = s_open(fn, FA_READ | FA_OPEN_EXISTING);
        if (fp >= 0) {
            char * oneline = jo_malloc(ONE_LINE_MAX_LEN);
            strcpy(oneline, "");
            uint32_t bytes = 0;
            while(strncmp(oneline, "[START]", 7))
            {
                oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
            }
            while(strncmp(oneline, "[END]", 5))
            {
                if(!strncmp(oneline, "font", 4))
                    sscanf(oneline, "font=%d,%d,%d", &loadedTheme.colours[PAL_COL_FONT].r, &loadedTheme.colours[PAL_COL_FONT].g, &loadedTheme.colours[PAL_COL_FONT].b);
                if(!strncmp(oneline, "bg", 2))
                    sscanf(oneline, "bg=%d,%d,%d", &loadedTheme.colours[PAL_COL_BG].r, &loadedTheme.colours[PAL_COL_BG].g, &loadedTheme.colours[PAL_COL_BG].b);
                if(!strncmp(oneline, "selector", 8))
                    sscanf(oneline, "selector=%d,%d,%d", &loadedTheme.colours[PAL_COL_SELECTOR].r, &loadedTheme.colours[PAL_COL_SELECTOR].g, &loadedTheme.colours[PAL_COL_SELECTOR].b);
                if(!strncmp(oneline, "boxbg", 5))
                    sscanf(oneline, "boxbg=%d,%d,%d", &loadedTheme.colours[PAL_COL_BOX_BG].r, &loadedTheme.colours[PAL_COL_BOX_BG].g, &loadedTheme.colours[PAL_COL_BOX_BG].b);
                oneline = s_gets(oneline, ONE_LINE_MAX_LEN, fp, &bytes, st->size);
                themeLoaded = true;
            }
            strcpy(loadedThemeName, theme);             
            jo_free(oneline);
            s_close(fp);
            s_chdir(currentDirectory);
            applyTheme();
            return true;
        }
    }
    s_chdir(currentDirectory);
    return false;
}