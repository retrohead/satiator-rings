#include "main.h"
#include "satiator_functions.h"

char debug_buf[512];
#ifdef DEBUG
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

void printCenterText(int row, char * text)
{
    int x = ((320 / 8) / 2) - 1;
    x -= strlen(text) / 2;
    for (int i=0;i< (320 / 8);i++)
    {
        if(i < x)
        {
            jo_nbg2_printf(i, row, " ");
        } else if(i - x < (int)strlen(text))
        {
            char c[2];
            c[0] = text[i-x];
            c[1] = '\0';
            jo_nbg2_printf(i, row, "%s", c);
        } else
        {
            jo_nbg2_printf(i, row, " ");
        }
    }
}
void printCenterProcessText(int row, char * text)
{
    printCenterText(row, text);
    draw_sprites();
    slSynch();
}
