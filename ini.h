void loadIniList(char * fn, bool sort);
bool deleteSelectedShortcutFromIni(const char * ini);
bool itemIsInIni(const char * ini, const char * fullpath);
bool writeItemShortcut(const char * ini, const char * fullpath);
