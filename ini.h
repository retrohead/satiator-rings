void loadIniList(char * fn, bool sort);
bool deleteIniLine(const char * ini, const char * textline);
bool lineIsInIni(const char * ini, const char * textline);
bool writeUniqueIniLineAtEnd(const char * ini, const char * textline, int maxLines);
bool writeUniqueIniLineAtStart(const char * ini, const char * textline, int maxLines);
