#include "main.h"
#include "direntry.h"

int boxTexture = -1;
int boxCornerTexture = -1;
int selectionSprite = -1;
int guiTex = -1;

static unsigned char gui_texture_image[] =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void createGuiBoxes(char * headerTexture, bool withSelectionSprite)
{
    create_sprite(load_gui_texture(PAL_COL_BG), 160, 120, 3, 50.0, 50.0, 0);
    char * dir = jo_malloc(512);
    sprintf(dir, "/satiator-rings/themes/%s", loadedThemeName);

    boxTexture = load_gui_texture(PAL_COL_BOX_BG);
    boxCornerTexture = load_sprite_texture_satiator(dir, "CORNER.TGA");
    create_sprite(load_sprite_texture_satiator(dir, headerTexture), 0, 4, 1, 1, 1, 0);
    create_sprite(load_sprite_texture_satiator(dir, "RINGS.TGA"), 192, 4, 1, 1, 1, 0);
    
    // top text strip
    create_sprite(boxTexture, 156, 24, 0, 38.9, 1.35, 0);
    create_sprite(boxCornerTexture, 4, 22, 0, 1.1, 1.1, 0);
    create_sprite(boxCornerTexture, 5, 25, 0, 1.1, 1.1, 270);
    create_sprite(boxCornerTexture, 307, 23, 0, 1.1, 1.1, 90);
    create_sprite(boxCornerTexture, 307, 25, 0, 1.1, 1.1, 180);

     // center text strip
    create_sprite(boxTexture, 156, 124, 0, 38.9, 22.85, 0);
    create_sprite(boxCornerTexture, 4, 36, 0, 1.1, 1.1, 0);
    create_sprite(boxCornerTexture, 5, 211, 0, 1.1, 1.1, 270);
    create_sprite(boxCornerTexture, 307, 37, 0, 1.1, 1.1, 90);
    create_sprite(boxCornerTexture, 307, 211, 0, 1.1, 1.1, 180);

    // bottom text strip
    create_sprite(boxTexture, 131, 224, 0, 32.6, 1.35, 0);
    create_sprite(boxCornerTexture, 4, 222, 0, 1.1, 1.1, 0);
    create_sprite(boxCornerTexture, 5, 225, 0, 1.1, 1.1, 270);
    create_sprite(boxCornerTexture, 257, 223, 0, 1.1, 1.1, 90);
    create_sprite(boxCornerTexture, 257, 225, 0, 1.1, 1.1, 180);
    
    // bottom clock strip
    create_sprite(boxTexture, 288, 224, 0, 5.8, 1.35, 0);
    create_sprite(boxCornerTexture, 268, 222, 0, 1.1, 1.1, 0);
    create_sprite(boxCornerTexture, 269, 225, 0, 1.1, 1.1, 270);
    create_sprite(boxCornerTexture, 307, 223, 0, 1.1, 1.1, 90);
    create_sprite(boxCornerTexture, 307, 225, 0, 1.1, 1.1, 180);

    if(withSelectionSprite)
        selectionSprite = create_sprite(load_gui_texture(PAL_COL_SELECTOR), 320, 250, 0, 2, 1, 0);
    else
        selectionSprite = -1;
    
    jo_free(dir);
    draw_sprites();
    slSynch();
}
void updateSelectionSprite(int row, bool shortStyle, bool hidden)
{
    if(selectionSprite < 0)
        return;
    sprites[selectionSprite].y = (row * 8);
    sprites[selectionSprite].rot_angle = 0;
    if(hidden)
    {
        sprites[selectionSprite].scale_x = 0.01;
        sprites[selectionSprite].scale_y = 0.01;
        sprites[selectionSprite].x = 640;
        sprites[selectionSprite].y = 480;
        return;
    }
    if(shortStyle)
    {
        sprites[selectionSprite].scale_x = 0.68 * 40;
        sprites[selectionSprite].scale_y = 0.85;
        sprites[selectionSprite].x = 112;
    } else
    {
        sprites[selectionSprite].scale_x = 0.95 * 40;
        sprites[selectionSprite].scale_x += 0.1f;
        sprites[selectionSprite].scale_y = 0.85;
        sprites[selectionSprite].x = 156;
    }
}
void updateGuiPalette()
{
    if(themeLoaded)
    {
        for(int i=0;i <PAL_COL_COUNT; i++)
            main_palette.data[2 + i] = JO_COLOR_RGB(loadedTheme.colours[i].r, loadedTheme.colours[i].g, loadedTheme.colours[i].b);
    } else
    {
        // default colours
        main_palette.data[2 + PAL_COL_FONT] = JO_COLOR_Red;
        main_palette.data[2 + PAL_COL_SELECTOR] = JO_COLOR_RGB(251, 221, 221);
        main_palette.data[2 + PAL_COL_WHITE] = JO_COLOR_White;
        main_palette.data[2 + PAL_COL_BG] = JO_COLOR_Red;
        main_palette.data[2 + PAL_COL_BOX_BG] = JO_COLOR_White;
    }
}
int load_gui_texture(enum themePalEntryTypes palId)
{   
    jo_img_8bits gui_img;
    gui_img.data = gui_texture_image;
    gui_img.width = 8;
    gui_img.height = 8;
    for(int i=0;i<64;i++)
        gui_img.data[i] = palId + 3;
    int tex = get_free_texture();
    spriteTex[tex].texture_id = jo_sprite_add_8bits_image(&gui_img);
    updateGuiPalette();
    return tex;
}