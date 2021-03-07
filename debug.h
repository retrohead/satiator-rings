#define debugLog(...)		do {sprintf(debug_buf, __VA_ARGS__); debug_print(debug_buf); } while(0)

extern char debug_buf[512];
extern void debug_print(char * buf);
extern void printCenterText(int row, char * text);
extern void printCenterProcessText(int row, char * text);
#define centerTextVblank(ypos, ...)		do {sprintf(debug_buf, __VA_ARGS__); printCenterProcessText(ypos, debug_buf); } while(0)