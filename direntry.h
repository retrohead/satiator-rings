#define MAX_LOADED_DIR_ENTRIES 5000
#define LIST_SCROLL_DELAY 30

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
extern int selectionSprite;
extern int listScrolldelay;

extern void initDirEntries();
extern int compareDirEntry(const void *pa, const void *pb);
extern void sortDirEntries();
extern int getGameIdFromDirectory(char * dir);
extern void boxartIdToTexturePath(int id, char * dir, char * fn);
extern void loadFileList(char * directory, int (*filter)(dirEntry *entry));

extern void loadSelectionSprite();
extern void updateSelectionSprite(int row, bool shortStyle);
extern void moveDirEntrySelectionUp(int maxlistItems, int sfx, bool shortSelectionItem);
extern void moveDirEntrySelectionDown(int maxlistItems, int sfx, bool shortSelectionItem);