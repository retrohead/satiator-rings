#include "font.h"

jo_palette font_palettes[2];
int fontpalsUsed = 0;

jo_palette          *font_palette_handling(void)
{
    if(fontpalsUsed >= 2)
        return;
    jo_create_palette(&font_palettes[fontpalsUsed]);
    fontpalsUsed++;
    return (&font_palettes[fontpalsUsed]);
}
void                    load_fonts(void)
{
    jo_img_8bits        img;
    img.data = NULL;
    jo_set_tga_palette_handling(font_palette_handling);
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FONT.TGA", TRANSPARENT_COLOR_INDEX_IN_FONT_PALETTE);
    jo_vdp2_set_nbg2_8bits_font(&img, " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,'\"?!_*#$%&()+-/:;<=>[\\]^{|}~", font_palettes[0].id, false, true);
    jo_free_img(&img);
}

void changeFontCol(jo_palette * palette, int col)
{
    palette->data[2] = col;
}