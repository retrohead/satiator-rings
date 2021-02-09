
#include <jo/jo.h>

void load_background(char *fn)
{
    jo_img      bg;
    bg.data = NULL;
    jo_tga_loader(&bg, "BG", fn, JO_COLOR_Transparent);
    jo_set_background_sprite(&bg, 0, 0);
    jo_free_img(&bg);
}