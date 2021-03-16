#define debugLog(...)		do {sprintf(debug_buf, __VA_ARGS__); debug_print(debug_buf); } while(0)

extern char debug_buf[512];
extern void clearMessage();
extern void debug_print(char * buf);
extern void printCenterText(int row, char * text);
extern void printCenterProcessText(int row, char * text);
extern void outputTextWaitForController(char *text);
#define centerTextVblank(ypos, ...)		do {sprintf(debug_buf, __VA_ARGS__); printCenterProcessText(ypos, debug_buf); } while(0)
#define centerText(ypos, ...)		do {sprintf(debug_buf, __VA_ARGS__); printCenterText(ypos, debug_buf); } while(0)
#define controllerText(...)		do {sprintf(debug_buf, __VA_ARGS__); outputTextWaitForController(debug_buf); } while(0)