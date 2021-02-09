#include <jo/jo.h>
#include <string.h>
#include <stdlib.h>
#include "sprite_manager.h"

spriteType sprites[MAX_SPRITES];
spriteTexType spriteTex[MAX_SPRITE_TEXTURES];

void init_sprites()
{
    int i;
    for(i=0;i<MAX_SPRITES;i++)
    {
        sprites[i].used = false;
    }
    for(i=0;i<MAX_SPRITE_TEXTURES;i++)
    {
        if(spriteTex[i].used)
        {
            // free the texture

        }
        spriteTex[i].used = false;
    }
}

int get_free_sprite()
{
    int i;
    for(i=0;i<MAX_SPRITES;i++)
    {
        if(!sprites[i].used)
        {
            sprites[i].used = true;
            return i;
        }
    }
    return -1;
}

int create_sprite(int sprite_tex_id, unsigned short x, unsigned short y, unsigned short z, double scale_x, double scale_y, unsigned short rot_angle)
{
    int spriteId = get_free_sprite();
    if(spriteId < 0)
    {
        return -1;
    }
    sprites[spriteId].sprite_tex_id = sprite_tex_id;
    sprites[spriteId].x = x;
    sprites[spriteId].y = y;
    sprites[spriteId].z = z;
    sprites[spriteId].scale_x = scale_x;
    sprites[spriteId].scale_y = scale_y;
    sprites[spriteId].rot_angle = rot_angle;
    return spriteId;
}

int get_free_texture()
{
    int i;
    for(i=0;i<MAX_SPRITES;i++)
    {
        if(!sprites[i].used)
        {
            sprites[i].used = true;
            return i;
        }
    }
    return -1;
}

int load_sprite_texture(char *directory, char *filename)
{
    int textureId = get_free_texture();
    if(textureId < 0)
    {
        return -1;
    }
    // load the texture into joengine
    spriteTex[textureId].texture_id = jo_sprite_add_tga(directory, filename, JO_COLOR_Transparent);
    return textureId;
}

void draw_sprites()
{
    int i;
    for(i=0;i<MAX_SPRITES;i++)
    {
        if(sprites[i].used)
        {
            jo_sprite_change_sprite_scale_xy(sprites[i].scale_x, sprites[i].scale_y);
            jo_sprite_draw3D_and_rotate(spriteTex[sprites[i].sprite_tex_id].texture_id, sprites[i].x - 160, sprites[i].y - 120, sprites[i].z, sprites[i].rot_angle);
        }
    }
}