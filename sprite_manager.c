#include <jo/jo.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

spriteType sprites[MAX_SPRITES];
spriteTexType spriteTex[MAX_SPRITE_TEXTURES];

void init_sprites()
{
    jo_sprite_free_all();
    int i;
    for(i=0;i<MAX_SPRITES;i++)
    {
        sprites[i].used = false;
    }
    for(i=0;i<MAX_SPRITE_TEXTURES;i++)
    {
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

int create_sprite(int sprite_tex_id, float x, float y, unsigned short z, float scale_x, float scale_y, float rot_angle)
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
    sprites[spriteId].rot_speed = 0;
    sprites[spriteId].speed_x = 0;
    sprites[spriteId].speed_y = 0;
    sprites[spriteId].velocity_x = 0;
    sprites[spriteId].velocity_y = 0;
    sprites[spriteId].isTransparent = false;

    return spriteId;
}

int get_free_texture()
{
    int i;
    for(i=0;i<MAX_SPRITE_TEXTURES;i++)
    {
        if(!spriteTex[i].used)
        {
            spriteTex[i].used = true;
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
    jo_sprite_enable_gouraud_shading();
    for(i=0;i<MAX_SPRITES;i++)
    {
        if(sprites[i].used)
        {
            jo_set_gouraud_shading_brightness(16);
            jo_sprite_change_sprite_scale_xy(sprites[i].scale_x, sprites[i].scale_y);
            
            if(sprites[i].isTransparent)
	            jo_sprite_enable_half_transparency();
            jo_sprite_draw3D_and_rotate(
                spriteTex[sprites[i].sprite_tex_id].texture_id, 
                (int)(sprites[i].x - 160 + (getSpriteWidth(i)/ 2)), 
                (int)(sprites[i].y - 120 + (getSpriteHeight(i)/ 2)), 
                sprites[i].z + 100, 
                (int)(sprites[i].rot_angle));
                
            if(sprites[i].isTransparent)
	            jo_sprite_disable_half_transparency();
            jo_sprite_restore_sprite_scale();
        }
    }
	jo_sprite_disable_gouraud_shading();
}