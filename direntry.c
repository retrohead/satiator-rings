#include "direntry.h"

int selectedDirEntry = 0;
char currentDirectory[1024];
int dirEntyCount = 0;
dirEntry dirEntries[MAX_LOADED_DIR_ENTRIES];