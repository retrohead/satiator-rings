#include <jo/jo.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "satiator_functions.h"

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
bool cdFileExists(const char *directory,const char *filename)
{
    // check the file exists
    if (directory != JO_NULL)
        jo_fs_cd(directory);
    int fid = GFS_NameToId((Sint8 *)filename);
    if (directory != JO_NULL)
        jo_fs_cd(JO_PARENT_DIR);
    if (fid < 0)
    {
        return false;
    }
    return true;
}
int load_sprite_texture(const char *directory,const char *filename)
{
    if(!cdFileExists(directory, filename))
        return -1;
    int textureId = get_free_texture();
    if(textureId < 0)
    {
        return -1;
    }
    // load the texture into joengine
    int tex = jo_sprite_add_tga(directory, filename, JO_COLOR_Transparent);
    if(tex < 0)
    {
        spriteTex[textureId].used = false;
        return -1;
    }
    spriteTex[textureId].texture_id = tex;
    return textureId;
}
int load_sprite_texture_satiator(const char *directory, const char *filename)
{
    if(directory != NULL)
        s_chdir(directory);
    // try stat the file
    s_stat_t *st = (s_stat_t*)statbuf;
    int fr = s_stat(filename, st, sizeof(statbuf));
    if (fr < 0)
    {
        if(directory != NULL)
            s_chdir(currentDirectory);
        return -1;
    }
    // allocate a buffer for the texture
    char * buf = (char *)jo_malloc_with_behaviour((st->size + 1) * sizeof(*buf), JO_MALLOC_TRY_REUSE_BLOCK);
    // try open the file
    fr = s_open(filename, FA_READ | FA_OPEN_EXISTING);
    if (fr < 0)
    {
        if(directory != NULL)
            s_chdir(currentDirectory);
        return -1;
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
            return -1;
        }
        readBytes += readSize;
    }
    s_close(fr);
    int textureId = get_free_texture();
    if(textureId < 0)
    {
        jo_free(buf);
        if(directory != NULL)
            s_chdir(currentDirectory);
        return -1;
    }
    // load the texture into joengine
    int tex = jo_sprite_add_tga_from_stream(buf, JO_COLOR_Transparent);
    jo_free(buf);
    if(tex < 0)
    {
        spriteTex[textureId].used = false;
        if(directory != NULL)
            s_chdir(currentDirectory);
        return -1;
    }
    spriteTex[textureId].texture_id = tex;
    if(directory != NULL)
        s_chdir(currentDirectory);
    return textureId;
}
void draw_sprites()
{
    int i;
    jo_sprite_enable_gouraud_shading();
    for(i=0;i<MAX_SPRITES;i++)
    {
        if(sprites[i].used && sprites[i].sprite_tex_id != -1)
        {
            jo_set_gouraud_shading_brightness(16);
            if((sprites[i].scale_x != 1.0) || (sprites[i].scale_y != 1.0))
            jo_sprite_change_sprite_scale_xy(sprites[i].scale_x, sprites[i].scale_y);
            
            if(sprites[i].isTransparent)
	            jo_sprite_enable_half_transparency();
            if(sprites[i].rot_angle < 1.0)
            {
            jo_sprite_draw3D(
                spriteTex[sprites[i].sprite_tex_id].texture_id, 
                (int)(sprites[i].x - 160 + (getSpriteWidth(i) / 2)), 
                (int)(sprites[i].y - 120 + (getSpriteHeight(i) / 2)), 
                sprites[i].z + 101);
            }
            else
            {
            jo_sprite_draw3D_and_rotate(
                spriteTex[sprites[i].sprite_tex_id].texture_id, 
                (int)(sprites[i].x - 160 + (getSpriteWidth(i) / 2)), 
                (int)(sprites[i].y - 120 + (getSpriteHeight(i) / 2)), 
                sprites[i].z + 101, 
                (int)(sprites[i].rot_angle));
            }
                
            if(sprites[i].isTransparent)
	            jo_sprite_disable_half_transparency();
            jo_sprite_restore_sprite_scale();
        }
    }
	jo_sprite_disable_gouraud_shading();
}