#include <jo/jo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "satiator_functions.h"

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

jo_palette          *theme_palette_handling(void)
{
    jo_create_palette(&main_palette);
    main_palette.data[2 + PAL_COL_FONT] = JO_COLOR_Red;
    main_palette.data[2 + PAL_COL_BG] = JO_COLOR_Red;
    main_palette.data[2 + PAL_COL_WHITE] = JO_COLOR_White;
    main_palette.data[2 + PAL_COL_SELECTOR] = JO_COLOR_RGB(251, 221, 221);
    return (&main_palette);
}