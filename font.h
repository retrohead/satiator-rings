#include <jo/jo.h>

#define TRANSPARENT_COLOR_INDEX_IN_FONT_PALETTE     (2)

extern jo_palette font_palettes[2];

extern void load_fonts(void);
extern void changeFontCol(jo_palette * palette, int col);