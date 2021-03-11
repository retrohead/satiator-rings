#define MAX_LOADED_DIR_ENTRIES 5000

enum dirEntryType
{
    DIR_NULL,
    DIR_DIRECTORY,
    DIR_FILE,
    DIR_GAME,
    DIR_SHORTCUT_FOLDER,
    DIR_SHORTCUT_GAME,
};

typedef struct
{
    char * name;
    enum dirEntryType type;
} dirEntry;

extern dirEntry dirEntries[MAX_LOADED_DIR_ENTRIES];
extern int selectedDirEntry;
extern char currentDirectory[1024];
extern int dirEntyCount;
extern int listOffset;
extern bool truncatedList;

extern void initDirEntries();
extern int compareDirEntry(const void *pa, const void *pb);
extern void sortDirEntries();