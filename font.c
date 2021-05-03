#include <jo/jo.h>
#include <string.h>
#include <stdlib.h>
#include "font.h"
#include "theme.h"
#include "satiator_functions.h"

extern t_tga_error_code		__jo_tga_any_loader_from_stream(jo_raw_img *img, char *stream, const jo_color transparent_color, int *bits);

void load_font_satiator(const char *directory, const char *filename)
{
    jo_raw_img        img;
    img.data = NULL;
    jo_set_tga_palette_handling(theme_palette_handling);


    if(directory != NULL)
        s_chdir(directory);
    // try stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(filename, st, sizeof(statbuf));
    if (fr < 0)
    {
        if(directory != NULL)
            s_chdir(currentDirectory);
        return;
    }
    // allocate a buffer for the texture
    char * buf = (char *)jo_malloc_with_behaviour((st->size + 1) * sizeof(*buf), JO_MALLOC_TRY_REUSE_BLOCK);

    // try open the file
    fr = s_open(filename, FA_READ | FA_OPEN_EXISTING);
    if (fr < 0)
    {
        if(directory != NULL)
            s_chdir(currentDirectory);
        return;
    }

    uint32_t readBytes = 0;
    while(readBytes < st->size)
    {
        uint32_t readSize = S_MAXBUF;
        if(st->size - readBytes < readSize)
            readSize = st->size - readBytes;
        int ret = s_read(fr, buf + readBytes, readSize);
        if(ret != (int)readSize)
        {
            jo_free(buf);
            s_close(fr);
            if(directory != NULL)
                s_chdir(currentDirectory);
            return;
        }
        readBytes += readSize;
    }
    s_close(fr);


    JO_ZERO(buf[st->size]);
    int bits;
    __jo_tga_any_loader_from_stream(&img, buf, TRANSPARENT_COLOR_INDEX_IN_FONT_PALETTE, &bits);
    
    jo_vdp2_set_nbg2_8bits_font((jo_img_8bits *)&img, " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,'\"?!_*#$%&()+-/:;<=>[\\]^{|}~@", main_palette.id, false, true);
    jo_free_img(&img);
    jo_set_tga_palette_handling(JO_NULL);


    if(directory != NULL)
        s_chdir(currentDirectory);
}
void                    load_fonts(void)
{


    jo_img_8bits        img;
    img.data = NULL;
    jo_set_tga_palette_handling(theme_palette_handling);


    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FONT.TGA", TRANSPARENT_COLOR_INDEX_IN_FONT_PALETTE);
    
    jo_vdp2_set_nbg2_8bits_font(&img, " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,'\"?!_*#$%&()+-/:;<=>[\\]^{|}~@", main_palette.id, false, true);
    jo_free_img(&img);
    jo_set_tga_palette_handling(JO_NULL);
    
}