
#include <jo/jo.h>
#include "satiator/iapetus/iapetus.h"
#include "background.h"
#include "sprite_manager.h"

jo_img      bg;
void load_background(const char *sub_dir, const char *fn)
{
    bg.data = NULL;
    jo_tga_loader(&bg, sub_dir, fn, JO_COLOR_Transparent);
    jo_set_background_sprite(&bg, 0, 0);
    jo_free_img(&bg);
}

void init_background()
{
    jo_core_set_screens_order(JO_NBG2_SCREEN, JO_SPRITE_SCREEN, JO_NBG3_SCREEN, JO_NBG1_SCREEN, JO_NBG0_SCREEN, JO_RBG0_SCREEN);
    jo_nbg2_clear();
}
