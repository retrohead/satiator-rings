#include "main.h"
#include "satiator_functions.h"


#ifdef DEBUG
char debug_buf[512];
void debug_print(char * txt)
{
    char * fn = "debuglog.txt";
    s_stat_t *st = (s_stat_t*)statbuf;
    int ret = s_stat(fn, st, sizeof(statbuf)-1);
    int fp;
    if(ret < 1) // new file needed
        fp = s_open(fn, FA_WRITE|FA_CREATE_ALWAYS);
    else
    { 
        // open exising and scan to the end
        fp = s_open(fn, FA_WRITE|FA_CREATE_ALWAYS);
        s_seek(fp, st->size - 1, 0);
    }
    sprintf(debug_buf, "%s\r\n", txt);
    satiatorWriteData(fp, debug_buf, strlen(debug_buf));
    s_close(fp);
}
#endif
