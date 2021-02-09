#define MAX_SPRITES 100
#define MAX_SPRITE_TEXTURES 30

typedef struct 
{
    unsigned short sprite_tex_id;
    unsigned short x;
    unsigned short y;
    unsigned short z;
    float scale_x, scale_y;
    unsigned short rot_angle;
    bool used;
} spriteType;

typedef struct 
{
    unsigned short texture_id;
    unsigned short orig_h;
    unsigned short orig_w;
    bool used;
} spriteTexType;

extern spriteType sprites[MAX_SPRITES];
extern spriteTexType spriteTex[MAX_SPRITE_TEXTURES];

extern void init_sprites();
extern int load_sprite_texture(char *directory, char *filename);
extern int create_sprite(int sprite_tex_id, unsigned short x, unsigned short y, unsigned short z, double scale_x, double scale_y, unsigned short rot_angle);
extern void draw_sprites();