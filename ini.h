#define ONE_LINE_MAX_LEN 1024

bool loadIniList(char * fn, bool sort, char * addItemStr, bool addAtStart);
bool addItemToIni(char * ini, char * fn, bool addStart, bool keepList, bool sort);
bool loadIniListFirstLine(char * fn, char * destbuf);
void writeIniList(char * fn, char * deleteEntry);
