#define SCREEN_FADE_SPEED 0.75f // max 31

enum fadeTypes
{
    FADE_IN,
    FADE_OUT
};

extern enum fadeTypes screenFadeType;
extern bool screenIsFading;
extern float screen_transparency;
extern int screen_active;

extern void fadeInOut(enum fadeTypes fadeType);
extern void fadeScreen();
extern void draw_faded_screen();
extern void debugScreenFade();