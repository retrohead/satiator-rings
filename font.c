#include <jo/jo.h>
#include "font.h"

jo_palette font_palette;
jo_palette          *font_palette_handling(void)
{
    jo_create_palette(&font_palette);
    return (&font_palette);
}

void                    load_nbg2_font(void)
{
    jo_img_8bits        img;
    img.data = NULL;
    jo_set_tga_palette_handling(font_palette_handling);
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FONT.TGA", TRANSPARENT_COLOR_INDEX_IN_FONT_PALETTE);
    //jo_vdp2_set_nbg2_8bits_font(&img, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,“”'\"?!_*#$%&()+-/:;<=>[\\]^{|}~ ", font_palette.id, false, true);
    jo_vdp2_set_nbg2_8bits_font(&img, " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,'\"?!_*#$%&()+-/:;<=>[\\]^{|}~", font_palette.id, false, true);
    jo_free_img(&img);
}