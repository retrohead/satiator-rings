#include "font.h"
#include "theme.h"

void                    load_fonts(void)
{
    jo_img_8bits        img;
    img.data = NULL;
    jo_set_tga_palette_handling(theme_palette_handling);
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FONT.TGA", TRANSPARENT_COLOR_INDEX_IN_FONT_PALETTE);
    jo_vdp2_set_nbg2_8bits_font(&img, " ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,'\"?!_*#$%&()+-/:;<=>[\\]^{|}~", main_palette.id, false, true);
    jo_free_img(&img);
    jo_set_tga_palette_handling(JO_NULL);
}