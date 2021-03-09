enum optionsType
{
    OPTIONS_LIST_MODE,
    OPTIONS_AUTO_PATCH
};

enum game_list_view_types
{
    GAME_VIEW_TEXT_AND_IMAGE,
    GAME_VIEW_TEXT_ONLY,
    GAME_VIEW_MAX_COUNT,
};

#define OPTIONS_COUNT 2

extern int options[OPTIONS_COUNT];
extern void initOptions();
extern void logic_options();