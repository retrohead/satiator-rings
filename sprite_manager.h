#define MAX_SPRITES 100
#define MAX_SPRITE_TEXTURES 30

#define getSpriteWidth(spriteId) jo_sprite_get_width(spriteTex[sprites[spriteId].sprite_tex_id].texture_id)
#define getSpriteHeight(spriteId) jo_sprite_get_height(spriteTex[sprites[spriteId].sprite_tex_id].texture_id)
#define getTextureWidth(spriteTexId) jo_sprite_get_width(spriteTex[spriteTexId].texture_id)
#define getTextureHeight(spriteTexId) jo_sprite_get_height(spriteTex[spriteTexId].texture_id)

typedef struct 
{
    int sprite_tex_id;
    float x;
    float y;
    unsigned short z;
    float scale_x, scale_y, velocity_x, speed_x, velocity_y, speed_y;
    float rot_angle, rot_speed;
    bool isTransparent;
    bool used;
} spriteType;

typedef struct 
{
    unsigned short texture_id;
    bool used;
} spriteTexType;

extern spriteType sprites[MAX_SPRITES];
extern spriteTexType spriteTex[MAX_SPRITE_TEXTURES];

extern void init_sprites();
extern int load_sprite_texture(char *directory, char *filename);
extern int create_sprite(int sprite_tex_id, float x, float y, unsigned short z, float scale_x, float scale_y, float rot_angle);
extern void draw_sprites();