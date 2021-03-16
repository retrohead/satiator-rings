enum optionsType
{
    OPTIONS_LIST_MODE,
    OPTIONS_AUTO_PATCH,
    OPTIONS_SOUND_VOLUME,
    OPTIONS_DESC_CACHE,
    /* end */
    OPTIONS_COUNT
};

enum game_list_view_types
{
    GAME_VIEW_TEXT_AND_IMAGE,
    GAME_VIEW_TEXT_ONLY,
    GAME_VIEW_MAX_COUNT,
};

extern int options[OPTIONS_COUNT];
extern void initOptions();
extern bool saveOptions();