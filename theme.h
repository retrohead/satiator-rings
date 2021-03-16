#include <jo/jo.h>
#include <stdint.h>

extern jo_palette main_palette;

extern jo_palette          *theme_palette_handling(void);

enum themePalEntryTypes
{
    PAL_COL_FONT,
    PAL_COL_SELECTOR,
    PAL_COL_WHITE,
    PAL_COL_RANDOM_BLUE,
    PAL_COL_BG,

    /* end */
    PAL_COL_COUNT
};
typedef struct
{
    int r;
    int g;
    int b;
} colourType;

typedef struct 
{
    colourType colours[PAL_COL_COUNT];
} themeType;

extern themeType loadedTheme;
extern bool themeLoaded;

extern void applyTheme();
extern void initTheme();
extern bool loadThemeFile(char * theme);