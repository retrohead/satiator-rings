#define MAX_LOADED_DIR_ENTRIES 5000
#define LIST_SCROLL_DELAY 7
#define TEXT_SCROLL_DELAY 15 // higher = slower, max 255

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
extern int dirEntryCount;
extern int listOffset;
extern bool truncatedList;
extern int listScrolldelay;

extern void initDirEntries();
extern int compareDirEntry(const void *pa, const void *pb);
extern void sortDirEntries();
extern void loadFileList(char * directory, int (*filter)(dirEntry *entry));
extern void loadSelectionSprite();
extern void moveDirEntrySelectionUp(int maxlistItems, int sfx, bool shortSelectionItem, bool drawSprites);
extern void moveDirEntrySelectionDown(int maxlistItems, int sfx, bool shortSelectionItem, bool drawSprites);
extern void displayDirListItem(const char * name, int ypos, bool selected, enum dirEntryType type, bool triggersHeld);
extern void displayDirListItemText(char * nam, int ypos, bool selected, enum dirEntryType type, bool triggersHeld);
extern void setMaxListLength(int maxlength);
extern void resetTextScroll();