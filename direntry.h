#define MAX_LOADED_DIR_ENTRIES 512

enum dirEntryType
{
    DIR_NULL,
    DIR_DIRECTORY,
    DIR_FILE,
    DIR_GAME
};

typedef struct
{
    char name[256];
    enum dirEntryType type;
} dirEntry;

extern dirEntry dirEntries[MAX_LOADED_DIR_ENTRIES];
extern int selectedDirEntry;
extern char currentDirectory[1024];
extern int dirEntyCount;