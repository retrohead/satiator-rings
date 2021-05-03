#define ONE_LINE_MAX_LEN 1024

bool loadIniList(char * fn, bool sort, char * addItemStr, bool addAtStart, int maxItems);
bool addItemToIni(char * ini, char * fn, bool addStart, bool keepList, bool sort, int maxItems);
bool loadIniListFirstLine(char * fn, char * destbuf);
void writeIniList(char * fn, char * deleteEntry);
