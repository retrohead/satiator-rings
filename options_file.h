enum optionsType
{
    OPTIONS_LIST_MODE,
    OPTIONS_AUTO_PATCH,
    OPTIONS_SOUND_VOLUME,
    OPTIONS_DESC_CACHE,
    OPTIONS_LIST_CATEGORY,
    /* end */
    OPTIONS_COUNT
};

enum game_list_view_types
{
    GAME_VIEW_TEXT_AND_IMAGE,
    GAME_VIEW_TEXT_ONLY,
    /* end */
    GAME_VIEW_MAX_COUNT
};

enum game_list_display_types
{
    GAME_LIST_STANDARD,
    GAME_LIST_FAVOURITES,
    GAME_LIST_RECENT_HISTORY,
    /* end */
    GAME_LIST_DISPLAY_MAX_COUNT
};

extern int options[OPTIONS_COUNT];
extern char loadedThemeName[32];
extern void initOptions();
extern bool saveOptions();